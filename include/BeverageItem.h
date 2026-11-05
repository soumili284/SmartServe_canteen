#ifndef BEVERAGE_ITEM_H
#define BEVERAGE_ITEM_H

#include "MenuItem.h"

class BeverageItem : public MenuItem {
private:
    bool isHot;
    int volumeMl;
    bool containsCaffeine;

public:
    BeverageItem(int id, const std::string& name, double price, ItemCategory category,
                 bool isHot, int volumeMl, bool caffeine = false, int stock = 60);

    // Getters
    bool getIsHot() const { return isHot; }
    int getVolumeMl() const { return volumeMl; }
    bool getContainsCaffeine() const { return containsCaffeine; }

    // Polymorphic overrides
    std::string getItemType() const override { return "Beverage"; }
    void displayCard() const override;
    void displayRow() const override;
    std::string serialize() const override;
};

#endif // BEVERAGE_ITEM_H
