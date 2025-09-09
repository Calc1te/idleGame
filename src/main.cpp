#include "game.h"


int main() {
    game g;
    while (g.bIsRunning) {
        g.gameRun();
    }
    return 0;
}
