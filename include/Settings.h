//
// Created by Calcite on 2025/9/9.
//

#ifndef SETTINGS_H
#define SETTINGS_H
#include "InputMonitor.h"
#include <vector>

class Game;
class Settings {
public:
    int thisSettingPage;
    Game* game = nullptr;
    enum settingState{menu, username, exportSave, loadSave};
    Settings(Game* g);
    ~Settings();
    void loadSettings();
    void saveSettings();
    void displaySettings();

    void displaySettingsMainMenu();


    void displaySettingsExportSave(); // $username.ids

    void displaySettingsLoadSave();

    void setUsername();

    void confirm(int option);

    struct saveData {
        std::vector<int> theFunnyNumber;
        int uLevel;
        int bLevel;
    };

};



#endif //SETTINGS_H
