#include <vector>
#include <climits>
#include <iostream>
#include <thread>
#include "Game.h"
#include "Settings.h"

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


Game::Game() {
    currentState = MAIN;
    monitor = new InputMonitor;
    settings = new Settings;
    statMessage = "";
    clear_screen();
    upgrades.emplace_back(upgrade::initPrice, upgrade::initBoost);
    buildings.emplace_back(building::initPrice, building::initBoost);
    bIsRunning.store(true);
    theFunnyNumber.push_back(0);
    iClickIncrement = 1;
    iAutoIncrement = 1;
    iOptionIdx = 0;
    bIsInputThreadRunning = false;
    buyConfirm = nullptr;
    settingConfirm = nullptr;
    monitor->start([this](int ch){this->handleKey(ch);});
}
Game::~Game() {
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

void Game::gameRun() {
    clear_screen();
    {
        std::lock_guard lk(stateMutex);
        display();
    }
    iTimeCounter += 1;
    if (iTimeCounter == AUTO_INCREMENT_RATE) {
        std::lock_guard lk(stateMutex);
        increment(iAutoIncrement);
        iTimeCounter = 0;
    }
    if (iTimeCounter % CLICK_COOLDOWN == 0) bIsInputThreadRunning = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FRAMERATE));
}
void Game::display(){

    std::cout<<title<<nl;
    displayNumber();
    std::cout<<nl<<YELLOW<<"auto increment is "<<GREEN<<iAutoIncrement<<YELLOW<<" per second"<<RESET_COLOR<<nl;
    std::cout<<YELLOW<<"press 'a' to contribute "<<GREEN<<iClickIncrement<<YELLOW<<" to this number"<<RESET_COLOR<<nl;
    displayMenu();
    std::cout<<nl<<statMessage<<nl;


}
void Game::displayNumber() {
    std::string number;
    switch (theFunnyNumber.size()) {
        case 1:
            number = std::to_string(theFunnyNumber[0]);
            break;
        case 2:
            number = std::to_string(theFunnyNumber[1]) + " INT_MAX and " + std::to_string(theFunnyNumber[0]);
            break;
        case 3:
            number = std::to_string(theFunnyNumber[2]) + " INT_MAX of "
                     + std::to_string(theFunnyNumber[1]) + " INT_MAX and "
                     + std::to_string(theFunnyNumber[0]);
            break;
        default:
            return;
    }
    std::cout << YELLOW << "Now you have " << GREEN << number << YELLOW << " points" << RESET_COLOR << nl;
}

void Game::displayMenu() {
    switch (currentState) {
        case MAIN:
            displayMainMenu();
            break;
        case UPGRADE:
            displayUpgrade();
            break;
        case SHOP:
            displayShop();
            break;
        case SETTINGS:
            displaySettings();
            break;
        default:
            displayMainMenu();
    }
}
void Game::displayMainMenu() {
    std::cout<<nl<<nl;
    std::cout<<"Buy upgrade"<<nl<<nl;
    std::cout<<"Buy buildings"<<nl<<nl;
    std::cout<<"Settings"<<nl<<nl;
    std::cout<<"Export save"<<nl<<nl;

}
void Game::displayUpgrade() {
    std::cout << YELLOW << "Upgrade Menu" << RESET_COLOR << nl << nl;

    for (int i = 0; i < upgrades.size(); ++i) {
        std::cout << GREEN << "Upgrade " << i + 1 << ": "
                  << upgrades[i].vBoost[0] << " boost "
                  << " - Cost: " << upgrades[i].vPrice[0] << nl;
    }
    std::cout << nl;
    std::cout << "Press 'y' to buy, 'm' to return to main menu" << nl;
}

void Game::displayShop() {
    std::cout << YELLOW << "Shop Menu" << RESET_COLOR << nl << nl;

    for (int i = 0; i < buildings.size(); ++i) {
        std::cout << GREEN << "Building " << i + 1 << ": "
                  << buildings[i].vBoost[0] << " boost "
                  << " - Cost: " << buildings[i].vPrice[0] << nl;
    }

    std::cout << nl;
    std::cout << "Press 'y' to buy, 'm' to return to main menu" << nl;
}

void Game::displaySettings() {
    std::cout << YELLOW << "Settings Menu" << RESET_COLOR << nl << nl;

    std::cout << GREEN << "1. Toggle Sound" << RESET_COLOR << nl;
    std::cout << GREEN << "2. Toggle Fullscreen" << RESET_COLOR << nl;
    std::cout << GREEN << "3. Change Language" << RESET_COLOR << nl;
    std::cout << GREEN << "4. Back to Main Menu" << RESET_COLOR << nl;

    std::cout << nl;
}

void Game::handleKey(int ch) {
    std::lock_guard lk(stateMutex);
    switch (ch) {
        case 'a':
            increment(iClickIncrement);
            break;
        case 'u':
            currentState = UPGRADE;
            buyConfirm = &Game::buyUpgrade;
            break;
        case 'b':
            currentState = SHOP;
            buyConfirm = &Game::buyBuilding;
            break;
        case 's':
            currentState = SETTINGS;
            break;
        case 'y':
            if (currentState == SETTINGS)settings->confirm(iOptionIdx);
            if (buyConfirm) (this->*buyConfirm)(iOptionIdx);
            break;
        case 'm':
            backToMain();
        default: break;
    }
}
void Game::clear_screen() {
    std::cout << "\033[H\033[J";
}

void Game::backToMain() {
    currentState = MAIN;
    statMessage = "";
    buyConfirm = nullptr;
}

std::vector<int> Game::increment(int carry) {
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

std::vector<int> Game::increment(const std::vector<int>& carry) {
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


std::vector<int> Game::decrement(const std::vector<int>& cost) {
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
        theFunnyNumber[i] = static_cast<int>(cur);
    }
    while (!theFunnyNumber.empty() && theFunnyNumber.back() == 0) {
        theFunnyNumber.pop_back();
    }
    return theFunnyNumber;
}

bool Game::isSufficient(const std::vector<int>& cost) {
    if (theFunnyNumber.size() < cost.size()) return false;
    for (int i = cost.size() - 1; i >= 0; --i) {
        if (theFunnyNumber[i] < cost[i]) return false;
        if (theFunnyNumber[i] > cost[i]) return true;
    }
    return true;
}

void Game::click() {
    if (!bSpammingFlag) {
        increment(iClickIncrement);
        bSpammingFlag = true;
    }
}


void Game::buyUpgrade(int idx) {
    if (idx < 0 || idx >= upgrades.size()) return;

    upgrade& u = upgrades[idx];
    if (!isSufficient(u.vPrice)) {
        statMessage = "too poor";
        return;
    }

    decrement(u.vPrice);

    iClickIncrement += u.vBoost[0];

    upgrades.push_back(upgrade::next_buyable(u));

    statMessage = "success!";
}


void Game::buyBuilding(int idx) {
    if (idx < 0 || idx >= buildings.size()) return;

    building& b = buildings[idx];
    if (!isSufficient(b.vPrice)) {
        statMessage = "too poor";
        return;
    }

    decrement(b.vPrice);

     iAutoIncrement += b.vBoost[0];

    buildings.push_back(building::next_buyable(b));

    statMessage = "success!";
}