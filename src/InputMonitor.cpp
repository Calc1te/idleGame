#include "InputMonitor.h"

#include <iostream>
#include <thread>
#ifdef __linux__
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#endif

#ifdef __APPLE__
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#endif

#ifdef _WIN64
#include <conio.h>
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#ifdef store
#undef store
#endif
#endif

void InputMonitor::start(const Callback& callback)
{
    if (bRunning)
        return;
    bRunning = true;
    monitorThread = std::thread([this, callback]() {
        initInput();
        while (bRunning) {
            int ch  = this->pollInput();
            if (ch!=-1) {
                callback(ch);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        resetInput();
    });
}
void InputMonitor::stop() {
    bRunning = false;
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
}
InputMonitor::~InputMonitor() {
    stop();
}

#ifdef _WIN32
void InputMonitor::initInput(){}
void InputMonitor::resetInput(){}
int InputMonitor::pollInput() {
    if (_kbhit()) {
        return _getch();
    }
    return -1;
}
void InputMonitor::pause() { paused.store(true); resetInput(); }
void InputMonitor::resume() { initInput(); paused.store(false); }
std::string InputMonitor::readLine(const std::string& prompt, const std::string& defaultInput) {
    std::string buffer = defaultInput;
    std::cout << prompt << buffer << std::flush;

    while (true) {
        int ch = _getch();

        if (ch == '\r') { // Enter
            std::cout << std::endl;
            break;
        } else if (ch == 8) { // Backspace (ASCII 8)
            if (!buffer.empty()) {
                buffer.pop_back();
                std::cout << "\b \b" << std::flush;
            }
        } else if (ch >= 32 && ch < 127) { // Printable characters
            buffer += static_cast<char>(ch);
            std::cout << static_cast<char>(ch) << std::flush;
        }
    }
    std::cout << std::endl; // Newline on exit
    return buffer;
}
#else
void InputMonitor::initInput() {
    struct termios newt = {};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // set non-blocking
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
}

void InputMonitor::resetInput() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
}

int InputMonitor::pollInput() {
    int ch = getchar();
    if (ch != EOF) return ch;
    return -1;
}

void InputMonitor::pause() { paused.store(true); resetInput(); }
void InputMonitor::resume() { initInput(); paused.store(false); }
std::string InputMonitor::readLine(const std::string& prompt, const std::string& defaultInput) {
    struct termios oldt = {}, newt = {};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::string buffer = defaultInput;
    std::cout << prompt << buffer << std::flush;

    while (true) {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            if (ch == '\n') { // Enter
                std::cout << std::endl;
                break;
            } else if (ch == 127 || ch == '\b') { // Backspace
                if (!buffer.empty()) {
                    buffer.pop_back();
                    // Basic ANSI code: Move cursor back, overwrite with space, move back again
                    std::cout << "\b \b" << std::flush;
                }
            } else if (static_cast<unsigned char>(ch) >= 32 && ch != 127) {
                buffer += ch;
                std::cout << ch << std::flush;
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return buffer;
}
#endif