cmake_minimum_required(VERSION 3.29)
project(idleGame)

set(CMAKE_CXX_STANDARD 20)

add_executable(idleGame main.cpp
        game.cpp
        game.h
        buyables.cpp
        buyables.h)
