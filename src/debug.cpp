#include <iostream>

#include "Game.h"
#include "Buyables.h"

int main() {
    int level = 0;
    building *build = new building(std::vector{10}, std::vector{1});
    while (level<20) {
        *build = building::next_buyable(*build);
        std::cout << build->vBoost[0]<<std::endl;
        std::cout << build->vPrice[0]<<std::endl;
        level++;

    }
    return 0;
}
