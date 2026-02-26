#include "Settings.h"
#include "Game.h"
#include "DisplayUtils.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include "base64.h"

#ifdef _WIN64
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

Settings::Settings(Game* g) {
    thisSettingPage = menu;
    game = g;
}
Settings::~Settings() = default;

void Settings::loadSettings() {

}
void Settings::saveSettings() {

}
void Settings::displaySettings() {
    switch (thisSettingPage) {
        case menu:
            displaySettingsMainMenu();
            break;
        case username:
            setUsername();
            break;
        case exportSave:
            displaySettingsExportSave();
            break;
        case loadSave:
            displaySettingsLoadSave();
            break;
        default:
            thisSettingPage = menu;
            displaySettingsMainMenu();
            break;
    }
}
void Settings::displaySettingsMainMenu() {
    std::cout << YELLOW << "Settings Menu" << RESET_COLOR << nl << nl;

    std::cout << YELLOW<< "[u] set user name" << RESET_COLOR << nl;
    std::cout << YELLOW<< "[e] export save" << RESET_COLOR << nl;
    std::cout << YELLOW<< "[l] load save" << RESET_COLOR << nl;
    std::cout << nl;
}

void Settings::displaySettingsExportSave() {
    game->setHalt(true);
    game->monitor->pause();
    std::cout << "\033[H\033[J";
    std::cout << title << nl;
    std::cout << YELLOW << "Where do you want to save your file?" << RESET_COLOR << nl;

    std::filesystem::path cwd = std::filesystem::current_path();
    std::string defaultPath = cwd.string();
    if (!defaultPath.empty() && defaultPath.back() != std::filesystem::path::preferred_separator) {
        defaultPath += std::filesystem::path::preferred_separator;
    }
    std::string fullDefault = defaultPath + game->getuserName();

    // Use the new readLine function which handles input buffer display and editing
    std::string filePath = game->monitor->readLine("Path: ", fullDefault);

    if (filePath.length() < 4 || filePath.substr(filePath.length() - 4) != ".ids") {
        filePath += ".ids";
    }

    exportSaveFile(filePath);

    game->monitor->resume();
    game->setHalt(false);
    thisSettingPage = menu;
}

void Settings::exportSaveFile(std::filesystem::path path) {
    saveData data = {
            .theFunnyNumber = game->theFunnyNumber,
            .uLevel = upgrade::level,
            .bLevel = building::level,
            .userName = game->getuserName()
    };
    std::string o;
    for (int num : data.theFunnyNumber) {
        o += std::to_string(num) + "-";
    }
    o += "_";
    o += std::to_string(data.uLevel) + "_";
    o += std::to_string(data.bLevel) + "_";
    o += data.userName;
    std::string bo = base64::encode(o);
    try {
        std::ofstream outFile(path);
        if (!outFile.is_open()) {
            game->statMessage = "save failed! Could not open file: " + path.string();
            return;
        }
        outFile << bo;
        outFile.close();
        game->statMessage = "save file to " + path.string() + "!";
    } catch (const std::exception& e) {
        game->statMessage = "save failed! " + std::string(e.what());
    }

}

void Settings::displaySettingsLoadSave() {
    game->setHalt(true);
    game->monitor->pause();
    std::cout << "\033[H\033[J";
    std::cout << title << nl;
    std::cout << YELLOW << "Where is the file?" << RESET_COLOR << nl;

    std::filesystem::path cwd = std::filesystem::current_path();
    std::string defaultPath = cwd.string();
    if (!defaultPath.empty() && defaultPath.back() != std::filesystem::path::preferred_separator) {
        defaultPath += std::filesystem::path::preferred_separator;
    }
    std::string fullDefault = defaultPath + game->getuserName();

    std::string filePath = game->monitor->readLine("Path: ", fullDefault);

    if (filePath.empty()) {
        filePath = fullDefault;
    }

    importSaveFile(filePath);
    game->monitor->resume();
    game->setHalt(false);
    thisSettingPage = menu;
}

void Settings::importSaveFile(std::filesystem::path path) {
    try {
        std::ifstream inFile(path);
        std::string encodedData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();

        std::string decodedData = base64::decode(encodedData);
        size_t pos = 0;
        std::vector<int> theFunnyNumber;
        while ((pos = decodedData.find('-')) != std::string::npos) {
            theFunnyNumber.push_back(std::stoi(decodedData.substr(0, pos)));
            decodedData.erase(0, pos + 1);
        }
        pos = decodedData.find('_');
        int uLevel = std::stoi(decodedData.substr(0, pos));
        decodedData.erase(0, pos + 1);
        pos = decodedData.find('_');
        int bLevel = std::stoi(decodedData.substr(0, pos));
        decodedData.erase(0, pos + 1);
        std::string userName = decodedData;

        game->theFunnyNumber = theFunnyNumber;
        upgrade::level = uLevel;
        building::level = bLevel;
        game->username = userName;
        game->statMessage = "load successful!";
    } catch (const std::filesystem::filesystem_error& e) {
        game->statMessage = "load failed! " + std::string(e.what());
    }
}

void Settings::setUsername() {
    // halt display and input monitor to let user type something
    game->setHalt(true);
    game->monitor->pause();
    std::cout << "\033[H\033[J";
    std::cout << title << nl;
    std::string userName;
    std::cout<<"what's your name?"<< nl;
    std::getline(std::cin, userName);
    // remember to reactivate input monitor
    game->monitor->resume();
    game->setHalt(false);
    thisSettingPage = menu;
    game->username = userName;


}

void Settings::confirm(int idx) {

}
