#ifndef GAME_H
#define GAME_H



#include <vector>
#include <string>
#include <mutex>


#ifdef _WIN64
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "DisplayUtils.h"
#include "Buyables.h"
#include "InputMonitor.h"

class Settings;
class Game {
public:
    std::string username;
    std::string statMessage;
    std::atomic<bool> bIsRunning;
    std::atomic<bool> bInputMode;
    std::atomic<bool> bIsDisplayOnHalt;
    const int FRAMERATE = 40;
    const int AUTO_INCREMENT_RATE = 40;
    const int CLICK_COOLDOWN = 10;
    int iTimeCounter = 0;
    bool bSpammingFlag = false;
    int iClickIncrement;
    int iAutoIncrement;
    int iOptionIdx;
    int LINE_HEIGHT = 0;
    bool bIsInputThreadRunning;
    void (Game::*buyConfirm)(int idx);
    void (Settings::*settingConfirm)(int idx);
    enum STATE{MAIN, UPGRADE, SHOP, SETTINGS};
    void backToMain();
    int currentState;
    InputMonitor *monitor;
    Settings *settings;
    std::mutex stateMutex;
    std::vector<int> theFunnyNumber;
    std::vector<upgrade> upgrades;
    std::vector<building> buildings;
    Game();
    ~Game();
    void gameRun();
    void initWindow();
    void setHalt(bool active);
    void display();
    void displayNumber();
    void displayMenu();
    void displayUpgrade();
    void displayShop();
    static void displayMainMenu();

    void handleKey(int ch);
    static void clear_screen();

    std::vector<int> increment(int carry);
    std::vector<int> increment(const std::vector<int>& carry);
    std::vector<int> decrement(const std::vector<int>& cost);
    bool isSufficient(const std::vector<int>& cost) const;
    void click();

    void buyUpgrade(int idx);
    void buyBuilding(int idx);


};
#endif //GAME_H
