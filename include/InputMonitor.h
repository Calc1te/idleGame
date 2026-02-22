#ifndef INPUTMONITOR_H
#define INPUTMONITOR_H
#include <atomic>
#include <functional>
#include <thread>
#include <string>
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

    InputMonitor(): paused(false), bRunning(false){}
    ~InputMonitor();
    std::atomic<bool> paused;
    void start(const Callback& callback);
    void stop();
    void pause();
    void resume();
    void prePathInput();
    void postPathInput();
    std::string readLine(const std::string& prompt, const std::string& defaultInput = "");

private:
    std::atomic<bool> bRunning;
    std::thread monitorThread;
    void initInput();
    void resetInput();
    int pollInput();
    char pollPath();
#ifdef _WIN32
#else
    struct termios oldt;
    int oldf;
#endif

};



#endif
