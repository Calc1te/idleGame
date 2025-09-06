#include <termios.h>
#include <vector>
#include <climits>
#include <iostream>
#include <termcap.h>
#include "game.h"
#include "buyables.h"

#include <thread>


game::game() {
    std::cout << "\033[H\033[J";
    bIsRunning = true;
    theFunnyNumber.push_back(0);
    clickIncrement = 1;
}
game::~game() {
    theFunnyNumber.clear();
    bIsRunning = false;
}

void game::gameRun() {
    std::cout << "\033[H";
    display();
    timeCounter += 1;
    if (timeCounter == 20) {
        increment(1);
        timeCounter = 0;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FRAMERATE));
}
void game::display() {

    std::cout<< title<<std::endl;
    displayNumber();

}
void game::displayNumber() {
    std::string number;
    switch (theFunnyNumber.size()) {
        case 1:
            number = std::to_string(theFunnyNumber[0]);
            break;
        case 2:
            number = std::to_string(theFunnyNumber[1]) + " INT_MAX and " + std::to_string(theFunnyNumber[0]);
            break;
        case 3:
            number = std::to_string(theFunnyNumber[2]) + " INT_MAX of " + number = std::to_string(theFunnyNumber[1]) + " INT_MAX and " + std::to_string(theFunnyNumber[0]);
            break;
        default:
            return;
    }
    std::cout << "now you have "+ number + " points"<<std::endl;
}



std::vector<int> game::increment(int carry) {
    if (carry == 0) return theFunnyNumber;
    for (size_t idx = 0; idx < theFunnyNumber.size() && carry != 0; ++idx) {
        constexpr long long BASE = INT_MAX;
        long long sum = static_cast<long long>(theFunnyNumber[idx]) + static_cast<long long>(carry);
        theFunnyNumber[idx] = static_cast<int>(sum % BASE);
        carry = static_cast<int>(sum / BASE);
    }
    if (carry != 0) {
        theFunnyNumber.push_back(carry);
    }
    return theFunnyNumber;
}

std::vector<int> game::increment(const std::vector<int>& carry) {
    const size_t n = std::max(theFunnyNumber.size(), carry.size());
    theFunnyNumber.resize(n, 0);

    long long c = 0;
    for (size_t i = 0; i < n; ++i) {
        constexpr long long BASE = INT_MAX;
        const long long a = theFunnyNumber[i];
        const long long b = (i < carry.size() ? carry[i] : 0);
        const long long sum = a + b + c;
        theFunnyNumber[i] = static_cast<int>(sum % BASE);
        c = sum / BASE;
    }
    if (c > 0) {
        theFunnyNumber.push_back(static_cast<int>(c));
    }
    return theFunnyNumber;
}


std::vector<int> game::decrement(const std::vector<int>& cost) {
    int carry = 0;
    for (size_t i = 0; i < cost.size() || carry; ++i) {
        if (i >= theFunnyNumber.size()) break;
        long long cur = static_cast<long long>(theFunnyNumber[i]) - (i < cost.size() ? cost[i] : 0) - carry;
        if (cur < 0) {
            cur += INT_MAX;
            carry = 1;
        } else {
            carry = 0;
        }
        theFunnyNumber[i] = (int)cur;
    }
    while (!theFunnyNumber.empty() && theFunnyNumber.back() == 0) {
        theFunnyNumber.pop_back();
    }
    return theFunnyNumber;
}

bool game::isSufficient(const std::vector<int>& cost) {
    if (theFunnyNumber.size() < cost.size()) return false;
    for (int i = cost.size() - 1; i >= 0; --i) {
        if (theFunnyNumber[i] < cost[i]) return false;
        if (theFunnyNumber[i] > cost[i]) return true;
    }
    return true;
}

void game::click() {
    increment(clickIncrement);
}

int game::inputMonitor() {
    int in;

    struct termios oldt, newt;
    tcgetattr(0, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VTIME] = 0;
    tcgetattr(0, &oldt);
    newt.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &newt);
    in = getchar();
    tcsetattr(0, TCSANOW, &oldt);

    return in;
}
