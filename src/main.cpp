#include "Game.h"


int main() {
    Game g;
    while (g.bIsRunning) {
        g.gameRun();
    }
    return 0;
}
