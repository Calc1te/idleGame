cmake_minimum_required(VERSION 3.29)
project(idleGame)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    add_definitions(-DNOMINMAX)
endif()
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(idleGame src/main.cpp
        src/Game.cpp
        include/Game.h
        src/Buyables.cpp
        include/Buyables.h
        src/InputMonitor.cpp
        include/InputMonitor.h
        src/Settings.cpp
        include/Settings.h)
add_executable(debug src/debug.cpp
        src/Game.cpp
        include/Game.h
        src/Buyables.cpp
        include/Buyables.h
        src/InputMonitor.cpp
        include/InputMonitor.h
        src/Settings.cpp
        include/Settings.h)
