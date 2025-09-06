#ifndef INPUTMONITOR_H
#define INPUTMONITOR_H
#include <atomic>
#include <functional>
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



class InputMonitor {
public:
    using Callback = std::function<void(int)>;

    InputMonitor(): bRunning(false){}
    ~InputMonitor();
    void start(const Callback& callback);
    void stop();

private:
    std::atomic<bool> bRunning;
    std::thread monitorThread;
    void initInput();
    void resetInput();
    int pollInput();
#ifndef _WIN32
    struct termios oldt;
    int oldf;
#endif

};



#endif
