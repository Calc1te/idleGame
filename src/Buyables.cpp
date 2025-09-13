//
// Created by Calcite on 2025/9/2.
//

#include "Buyables.h"
#include "Game.h"
#include <utility>
#include <cmath>

int upgrade::level = 1;
int building::level = 1;
std::vector<int> Buyables::initPrice = {10};
std::vector<int> Buyables::initBoost = {1};
static double COEFFICIENT = 1.832;


Buyables::Buyables(std::vector<int> price, std::vector<int> boost) {
    vPrice = std::move(price);
    vBoost = std::move(boost);

}
std::vector<int> getNext(int level, std::vector<int> vec) {
    using int64 = long long;
    constexpr int64 BASE = static_cast<int64>(std::numeric_limits<int>::max()) + 1LL;
    int64 carry = 0;

    vec.push_back(0);
    const double mul = 1.0 + COEFFICIENT / std::sqrt(static_cast<double>(level) + 1.0);

    for (auto it = vec.begin(); it != vec.end() - 1; ++it) {
        int &digit = *it;
        int64 temp = std::llround(static_cast<double>(digit) * mul) + carry;

        carry = temp / BASE;
        digit = static_cast<int>(temp % BASE);
    }

    if (carry > 0) {
        vec.back() = static_cast<int>(carry);
    } else {
        vec.pop_back();
    }

    return vec;
}



upgrade::upgrade(std::vector<int> price, std::vector<int> boost)
    : Buyables(std::move(price), std::move(boost)) {}

upgrade upgrade::next_buyable(const upgrade& u) {
    ++level;
    return {getNext(level, u.vPrice), getNext(level, u.vBoost)};
}


building::building(std::vector<int> price, std::vector<int> boost)
    : Buyables(std::move(price), std::move(boost)) {}

building building::next_buyable(const building& b) {
    ++level;
    return {getNext(level, b.vPrice), getNext(level, b.vBoost)};
}
