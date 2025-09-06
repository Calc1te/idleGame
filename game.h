#ifndef GAME_H
#define GAME_H
#include <vector>
class game {
public:
    const std::string title =
"  __  __ _  ____  ____  ___  ____  ____\n"
" (  )(  ( \\(_  _)(  __)/ __)(  __)(  _ \\\n"
"  )( /    /  )(   ) _)( (_ \\ ) _)  )   /\n"
" (__)\\_)__) (__) (____)\\___/(____)(__\\_)\n";
    bool bIsRunning;
    int FRAMERATE = 20;
    int timeCounter = 0;
    int clickIncrement;
    std::vector<int> theFunnyNumber;
    game();
    ~game();
    void gameRun();
    void display();
    void displayNumber();
    static void clear_screen();
    std::vector<int> increment(int carry);
    std::vector<int> increment(const std::vector<int>& carry);
    std::vector<int> decrement(const std::vector<int>& cost);
    bool isSufficient(const std::vector<int>& cost);
    void click();
    int inputMonitor();



};
#endif //GAME_H
