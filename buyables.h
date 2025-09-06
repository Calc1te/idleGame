#ifndef BUYABLES_H
#define BUYABLES_H
#include <vector>

class buyables {
    public:
    std::vector<int> vPrice;
    std::vector<int> vBoost;
    buyables(std::vector<int> price, std::vector<int> boost);
};
class upgrade : public buyables {
public:
    static int level;
    upgrade(std::vector<int> price, std::vector<int> boost);
    static upgrade next_buyable();
};
class building : public buyables {
public:
    static int level;
    building(std::vector<int> price, std::vector<int> boost);
    static building next_buyable();
};


#endif
