#ifndef BUYABLES_H
#define BUYABLES_H
#include <vector>

class buyables {
    public:
    std::vector<int> vPrice;
    std::vector<int> vBoost;
    static std::vector<int> initPrice;
    static std::vector<int> initBoost;
    buyables(std::vector<int> price, std::vector<int> boost);
    buyables getBuyableList();
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
