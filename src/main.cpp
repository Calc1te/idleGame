#include <iostream>
#include "game.h"
#include <climits>

#include "InputMonitor.h"


int main() {
    game g;
    while (true) {
        g.gameRun();
    }
    // InputMonitor monitor;
    // monitor.start([](int key) {
    //     std::cout << "Key pressed: " << (char)key << std::endl;
    // });
    // while (true) {
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }
    //
    // monitor.stop();
    return 0;
}
