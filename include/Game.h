#ifndef GAME_H
#define GAME_H

#define nl std::endl
#define RESET_COLOR "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#include <vector>
#include <string>

#include "Buyables.h"
#include "InputMonitor.h"
#include "Settings.h"

class Game {
public:
    const std::string title =
 "\033[31m  __  __ _  ____  ____  ___  ____  ____\n"
"\033[33m (  )(  ( \\(_  _)(  __)/ __)(  __)(  _ \\\n"
"\033[32m  )( /    /  )(   ) _)( (_ \\ ) _)  )   /\n"
"\033[34m (__)\\_)__) (__) (____)\\___/(____)(__\\_)\n";
    std::string statMessage;
    std::atomic<bool> bIsRunning;
    const int FRAMERATE = 40;
    const int AUTO_INCREMENT_RATE = 40;
    const int CLICK_COOLDOWN = 10;
    int iTimeCounter = 0;
    bool bSpammingFlag = false;
    int iClickIncrement;
    int iAutoIncrement;
    int iOptionIdx;
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

    void display();
    void displayNumber();
    void displayMenu();
    static void displaySettings();
    static void displayMainMenu();
    void displayUpgrade();
    void displayShop();

    void handleKey(int ch);
    static void clear_screen();

    std::vector<int> increment(int carry);
    std::vector<int> increment(const std::vector<int>& carry);
    std::vector<int> decrement(const std::vector<int>& cost);
    bool isSufficient(const std::vector<int>& cost);
    void click();

    void buyUpgrade(int idx);
    void buyBuilding(int idx);


};
#endif //GAME_H
