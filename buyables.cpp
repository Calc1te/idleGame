//
// Created by Calcite on 2025/9/2.
//

#include "buyables.h"
#include "game.h"
#include <utility>

int upgrade::level = 1;
int building::level = 1;
static double COEFFICIENT = 1.832;
std::vector<int> initPrice = {10};
std::vector<int> initBoost = {2};

buyables::buyables(std::vector<int> price, std::vector<int> boost) {
    vPrice = std::move(price);
    vBoost = std::move(boost);
}
std::vector<int> getNext(int level, std::vector<int> vec) {
    int carry = 0;
    vec.push_back(0);
    for (auto it = vec.begin(); it != vec.end() - 1; ++it) {
        int &digit = *it;

        long tempSum = static_cast<int>(digit*(1+COEFFICIENT/std::sqrt(level+1))) + carry;
        carry = 0;
        if (tempSum > INT_MAX) {
            carry = static_cast<int>(tempSum/INT_MAX);
            tempSum %= INT_MAX;
        }
        digit = static_cast<int>(tempSum);
    }
    if (vec.back()==0) {
        vec.pop_back();
    }
    return vec;
};


upgrade::upgrade(std::vector<int> price, std::vector<int> boost)
    : buyables(std::move(price), std::move(boost)) {}

upgrade upgrade::next_buyable() {
    ++level;
    return {getNext(level, initPrice), getNext(level, initBoost)};
}

building::building(std::vector<int> price, std::vector<int> boost)
    : buyables(std::move(price), std::move(boost)) {}

building building::next_buyable() {
    ++level;
    return {getNext(level, initPrice), getNext(level, initBoost)};
}
