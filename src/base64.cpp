//
// Created by Calcite on 26-2-15.
//

#include "../include/base64.h"
#include <vector>

char base64::base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


std::string base64::encode(const std::string& input) {
    int ch_ptr = 0;
    std::string output;
    std::vector<int> T(128, -1);
    for (int i = 0; i < 64; i++) T[i] = base64_chars[i];
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            output.push_back(base64_chars[val >> valb & 0x3F]); // Rshift 2
            valb -= 6;
        }
    }

    if (valb > -6) {
        output.push_back(base64_chars[val >> valb] & 0x3F);
    }
    while (output.size() % 4) {
        output.push_back('=');
    }
    return output;
}

std::string base64::decode(const std::string& input) {
    std::string output;
    std::vector<int> T(128, -1);
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            output.push_back(static_cast<char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return output;
}