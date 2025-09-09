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

#include "buyables.h"
#include "InputMonitor.h"

class game {
public:
    const std::string title =
"  __  __ _  ____  ____  ___  ____  ____\n"
" (  )(  ( \\(_  _)(  __)/ __)(  __)(  _ \\\n"
"  )( /    /  )(   ) _)( (_ \\ ) _)  )   /\n"
" (__)\\_)__) (__) (____)\\___/(____)(__\\_)\n";
    std::atomic<bool> bIsRunning;
    const int FRAMERATE = 40;
    const int AUTO_INCREMENT_RATE = 40;
    int iTimeCounter = 0;
    int iClickIncrement;
    int iAutoIncrement;
    bool bIsInputThreadRunning;
    enum STATE{MAIN, UPGRADE, SHOP, SETTINGS};
    int currentState;
    InputMonitor *monitor;
    std::mutex stateMutex;
    std::vector<int> theFunnyNumber;
    std::vector<upgrade> upgrades;
    std::vector<building> buildings;
    game();
    ~game();
    void gameRun();

    void display();
    void displayNumber();
    void displayMenu();
    void displaySettings();
    void displayMainMenu();
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
