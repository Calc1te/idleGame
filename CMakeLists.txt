cmake_minimum_required(VERSION 3.29)
project(idleGame)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(idleGame src/main.cpp
        src/game.cpp
        include/game.h
        src/buyables.cpp
        include/buyables.h
        src/InputMonitor.cpp
        include/InputMonitor.h
        src/settings.cpp
        include/settings.h)
add_executable(debug src/debug.cpp
        src/game.cpp
        include/game.h
        src/buyables.cpp
        include/buyables.h
        src/InputMonitor.cpp
        include/InputMonitor.h
        src/settings.cpp
        include/settings.h)
