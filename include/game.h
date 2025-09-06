#ifndef GAME_H
#define GAME_H
#include <vector>
#include <string>

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
    bool bIsInputThreadRunning;
    InputMonitor *monitor;
    std::mutex stateMutex;
    std::vector<int> theFunnyNumber;
    game();
    ~game();
    void gameRun();
    void display();
    void displayNumber();
    void handleKey(int ch);
    static void clear_screen();
    std::vector<int> increment(int carry);
    std::vector<int> increment(const std::vector<int>& carry);
    std::vector<int> decrement(const std::vector<int>& cost);
    bool isSufficient(const std::vector<int>& cost);
    void click();



};
#endif //GAME_H
