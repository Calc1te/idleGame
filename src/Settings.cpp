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
    std::cout << YELLOW << "directory + name" << RESET_COLOR << nl;
    std::string filePath;
    std::filesystem::path cwd = std::filesystem::current_path();
    std::string defaultPath = cwd.string();
    if (!defaultPath.empty() && defaultPath.back() != std::filesystem::path::preferred_separator) {
        defaultPath += std::filesystem::path::preferred_separator;
    }

#ifdef _WIN64
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    for (char c : defaultPath) {
        INPUT inputs[2] = {};

        inputs[0].EventType = KEY_EVENT;
        inputs[0].Event.KeyEvent.bKeyDown = TRUE;
        inputs[0].Event.KeyEvent.uChar.AsciiChar = c;
        inputs[0].Event.KeyEvent.wRepeatCount = 1;

        inputs[1].EventType = KEY_EVENT;
        inputs[1].Event.KeyEvent.bKeyDown = FALSE;
        inputs[1].Event.KeyEvent.uChar.AsciiChar = c;
        inputs[1].Event.KeyEvent.wRepeatCount = 1;

        DWORD written;
        WriteConsoleInputA(hInput, inputs, 2, &written);
    }
#else
    for (char c : defaultPath) {
        ioctl(STDIN_FILENO, TIOCSTI, &c);
    }
#endif

    std::getline(std::cin, filePath);

    if (filePath.empty()) {
        filePath = defaultPath + game->getuserName(); // Fallback
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
    o += std::to_string('_');
    o += std::to_string(data.uLevel) + "_";
    o += std::to_string(data.bLevel) + "_";
    o += data.userName;
    std::string bo = base64::encode(o);
    try {
        std::ofstream outFile(path);
        outFile << bo;
        outFile.close();
    } catch (const std::filesystem::filesystem_error& e) {
        game->statMessage = "save failed! " + std::string(e.what());
    }

}

void Settings::displaySettingsLoadSave() {
    game->setHalt(true);
    game->monitor->pause();
    std::cout << "\033[H\033[J";
    std::cout << title << nl;
    std::cout << YELLOW << "Where is the file?" << RESET_COLOR << nl;
    std::string filePath;
    std::filesystem::path cwd = std::filesystem::current_path();
    std::string defaultPath = cwd.string();
    if (!defaultPath.empty() && defaultPath.back() != std::filesystem::path::preferred_separator) {
        defaultPath += std::filesystem::path::preferred_separator;
    }

#ifdef _WIN64
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    for (char c : defaultPath) {
        INPUT inputs[2] = {};

        inputs[0].EventType = KEY_EVENT;
        inputs[0].Event.KeyEvent.bKeyDown = TRUE;
        inputs[0].Event.KeyEvent.uChar.AsciiChar = c;
        inputs[0].Event.KeyEvent.wRepeatCount = 1;

        inputs[1].EventType = KEY_EVENT;
        inputs[1].Event.KeyEvent.bKeyDown = FALSE;
        inputs[1].Event.KeyEvent.uChar.AsciiChar = c;
        inputs[1].Event.KeyEvent.wRepeatCount = 1;

        DWORD written;
        WriteConsoleInputA(hInput, inputs, 2, &written);
    }
#else
    for (char c : defaultPath) {
        ioctl(STDIN_FILENO, TIOCSTI, &c);
    }
#endif

    std::getline(std::cin, filePath);
    if (filePath.empty()) {
        filePath = defaultPath + game->getuserName(); // Fallback
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
