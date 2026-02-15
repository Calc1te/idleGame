//
// Created by Calcite on 26-2-15.
//

#include "base64.h"
#include <vector>

char base64::base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


std::string base64::encode(const std::string& input) {
    int ch_ptr = 0;
    std::string output;
    while (ch_ptr < input.size()) {
        const char c1 = input[ch_ptr];
        const char c2 = (ch_ptr + 1 < input.size()) ? input[ch_ptr + 1] : 0;
        const char c3 = (ch_ptr + 2 < input.size()) ? input[ch_ptr + 2] : 0;
        int val = (c1 << 16) + (c2 << 8) + c3;
        // int is 32 bit

        const char o1 = base64_chars[(val & 0xFC0000) >> 18];
        const char o2 = base64_chars[(val & 0x3F000) >> 12];
        const char o3 = base64_chars[(val & 0xFC0) >> 6];
        const char o4 = base64_chars[val & 0x3F];
        output += o1;
        output += o2;
        output += (ch_ptr + 1 < input.size()) ? o3 : '=';
        output += (ch_ptr + 2 < input.size()) ? o4 : '=';
        ch_ptr += 3;
    }
    return output;
}

std::string base64::decode(const std::string& input) {
    std::string output;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            output.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return output;
}