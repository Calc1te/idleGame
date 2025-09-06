#include <vector>
#include <climits>
#include <iostream>
#include <thread>
#include "game.h"
#include "buyables.h"

#ifdef __linux__
#include <termcap.h>
#include <termios.h>
#include <fcntl.h>
#endif

#ifdef __APPLE__
#include <termcap.h>
#include <termios.h>
#include <fcntl.h>
#endif

#ifdef _WIN64
#include <conio.h>
#include <windows.h>
#endif


game::game() {
    std::cout << "\033[H\033[J";
    bIsRunning.store(true);
    theFunnyNumber.push_back(0);
    iClickIncrement = 1;
    bIsInputThreadRunning = false;
    monitor = new InputMonitor;
    monitor->start([this](int ch){this->handleKey(ch);});
}
game::~game() {
    theFunnyNumber.clear();
    bIsRunning = false;
    if (monitor) {
        monitor->stop();
        delete monitor;
        monitor = nullptr;
    }
    theFunnyNumber.clear();
    bIsRunning.store(false);
}

void game::gameRun() {
    std::cout << "\033[H";
    {
        std::lock_guard<std::mutex> lk(stateMutex);
        display();
    }
    iTimeCounter += 1;
    if (iTimeCounter == AUTO_INCREMENT_RATE) {
        std::lock_guard<std::mutex> lk(stateMutex);
        increment(1);
        iTimeCounter = 0;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FRAMERATE));
}
void game::display() {

    std::cout<<title<<std::endl;
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
void game::handleKey(int ch) {
    std::lock_guard<std::mutex> lk(stateMutex);
    std::cout<< "clicked:" << static_cast<char>(ch);
}
void game::clear_screen() {
    std::cout << "\033[H\033[J";
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
    increment(iClickIncrement);
}

