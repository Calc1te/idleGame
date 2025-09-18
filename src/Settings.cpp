#include "Settings.h"
#include "Game.h"
#include "DisplayUtils.h"
#include <iostream>

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

}
void Settings::displaySettingsLoadSave() {

}

std::string Settings::setUsername() {
    // halt display and input monitor to let user type something
    game->setHalt(true);
    game->monitor->pause();
    std::cout << "\033[H\033[J";
    std::cout << title << nl;
    std::string userName;
    std::cout<<"what's your name?"<< nl;
    std::getline(std::cin, userName);
    game->monitor->resume();
    game->setHalt(false);
    thisSettingPage = menu;
    return userName;
}

void Settings::confirm(int idx){}
