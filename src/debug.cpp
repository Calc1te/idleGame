#include <iostream>

#include "Game.h"
#include "Buyables.h"
#include "base64.h"

int main() {
    std::string input = "test";
    std::string encoded = base64::encode(input);
    std::string decoded = base64::decode(encoded);
    std::cout << input;
    std::cout << encoded;
    std::cout << decoded;
    return 0;
}
