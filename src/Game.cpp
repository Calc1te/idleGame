#include <vector>
#include <climits>
#include <iostream>
#include <thread>
#include "Game.h"
#include "Settings.h"


Game::Game() {
    initWindow();
    currentState = MAIN;
    monitor = new InputMonitor;
    settings = new Settings(this);
    statMessage = "";
    bInputMode.store(false);
    bIsDisplayOnHalt.store(false);
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
void Game::initWindow() {
    int LINE_WIDTH = 0;

#ifdef _WIN64
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        LINE_HEIGHT = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        LINE_WIDTH  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    } else {
        LINE_HEIGHT = 25;
        LINE_WIDTH  = 80;
    }
#else
    struct winsize win;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == 0) {
        LINE_HEIGHT = win.ws_row;
        LINE_WIDTH  = win.ws_col;
    } else {
        LINE_HEIGHT = 25;
        LINE_WIDTH  = 80;
    }
#endif

    while (LINE_HEIGHT < 18 || LINE_WIDTH < 40) {
        std::cout << MAGENTA << "this terminal window seems too small" << nl;
        std::cout << MAGENTA << "current window size is "
                  << LINE_WIDTH << "*" << LINE_HEIGHT << nl;
        std::cout << MAGENTA << "try resize this window to continue?" << nl << nl;

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

#ifdef _WIN64
        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            LINE_HEIGHT = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            LINE_WIDTH  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        }
#else
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == 0) {
            LINE_HEIGHT = win.ws_row;
            LINE_WIDTH  = win.ws_col;
        }
#endif
        clear_screen();
    }
}

void Game::gameRun() {
    if (!bIsDisplayOnHalt.load()) {
        clear_screen();
        {
            std::lock_guard lk(stateMutex);
            display();
        }
    }else {
        clear_screen();
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
    if (!username.empty()) {
        std::cout<<MAGENTA<<"Hello, "<<username<<"!"<<RESET_COLOR<<nl;
    }
    displayNumber();
    std::cout<<nl<<YELLOW<<"auto increment is "<<GREEN<<iAutoIncrement<<YELLOW<<" per second"<<RESET_COLOR<<nl;
    std::cout<<YELLOW<<"press 'a' to contribute "<<GREEN<<iClickIncrement<<YELLOW<<" to this number"<<RESET_COLOR<<nl;
    displayMenu();
    std::cout<<nl<<statMessage<<nl;


}
void Game::displayNumber(){
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

void Game::displayMenu(){
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
            settings->displaySettings();
            break;
        default:
            displayMainMenu();
    }
}
void Game::displayMainMenu() {
    std::cout<<nl<<nl;
    std::cout<<"[u] Buy upgrade"<<nl<<nl;
    std::cout<<"[b] Buy buildings"<<nl<<nl;
    std::cout<<"[s] Settings"<<nl<<nl;
    std::cout<<"[e] Export save"<<nl<<nl;

}
void Game::displayUpgrade(){
    std::cout << YELLOW << "Upgrade Menu" << RESET_COLOR << nl << nl;
    if (buildings.size() > 6)std::cout << GREEN <<"..."<<RESET_COLOR<<nl;
    for (int i = 0; i < upgrades.size(); ++i) {
        if (upgrades.size() > LINE_HEIGHT - 13 && upgrades.size() - i > LINE_HEIGHT - 13) {
            continue;
        }
        std::cout << GREEN << "Upgrade " << i + 1 << ": "
                  << upgrades[i].vBoost[0] << " boost "
                  << " - Cost: " << upgrades[i].vPrice[0] << nl;
    }
    std::cout << nl;
    std::cout << "Press 'y' to buy, 'm' to return to main menu" << nl;
}

void Game::displayShop(){
    std::cout << YELLOW << "Shop Menu" << RESET_COLOR << nl << nl;
    if (buildings.size() > 8)std::cout << GREEN <<"...";
    for (int i = 0; i < buildings.size(); ++i) {
        if (upgrades.size() > LINE_HEIGHT - 13 && upgrades.size() - i > LINE_HEIGHT - 13) {
            continue;
        }
        std::cout << GREEN << "Building " << i + 1 << ": "
                  << buildings[i].vBoost[0] << " boost "
                  << " - Cost: " << buildings[i].vPrice[0] << nl;
    }

    std::cout << nl;
    std::cout << "Press 'y' to buy, 'm' to return to main menu" << nl;
}



void Game::handleKey(int ch) {
    if (bInputMode.load()){return;}
    std::lock_guard lk(stateMutex);
    switch (ch) {
        case 'a':
            increment(iClickIncrement);
            break;
        case 'u':
            if (currentState==SETTINGS) {
                settings->thisSettingPage = Settings::username;
                break;
            }
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
            break;
        case 'l':
            if (currentState == SETTINGS)settings->thisSettingPage = Settings::loadSave;
            break;
        case 'e':
            if (currentState == SETTINGS)settings->thisSettingPage = Settings::exportSave;
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

bool Game::isSufficient(const std::vector<int>& cost) const {
    if (theFunnyNumber.size() < cost.size()) return false;
    for (int i = static_cast<int>(cost.size()) - 1; i >= 0; --i) {
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

    upgrade& u = upgrades[upgrades.size() - 1];
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

    building& b = buildings[buildings.size() - 1];
    if (!isSufficient(b.vPrice)) {
        statMessage = "too poor";
        return;
    }

    decrement(b.vPrice);

     iAutoIncrement += b.vBoost[0];

    buildings.push_back(building::next_buyable(b));

    statMessage = "success!";
}

void Game::setHalt(bool active) {
    bInputMode.store(active);
    bIsDisplayOnHalt.store(active);
}