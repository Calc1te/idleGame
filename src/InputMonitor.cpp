#include "InputMonitor.h"

#include <thread>
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
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
#else
void InputMonitor::initInput() {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

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
#endif