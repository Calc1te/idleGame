//
// Created by Calcite on 2025/9/15.
//
#ifndef DISPLAYUTILS_H
#define DISPLAYUTILS_H

#pragma once
#include <string>

#define nl std::endl
#define RESET_COLOR "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

const std::string title =
"\033[31m  __  __ _  ____  ____  ___  ____  ____\n"
"\033[33m (  )(  ( \\(_  _)(  __)/ __)(  __)(  _ \\\n"
"\033[32m  )( /    /  )(   ) _)( (_ \\ ) _)  )   /\n"
"\033[34m (__)\\_)__) (__) (____)\\___/(____)(__\\_)\n";

#endif //DISPLAYUTILS_H
