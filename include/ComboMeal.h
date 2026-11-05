#ifndef COMBO_MEAL_H
#define COMBO_MEAL_H

#include "MenuItem.h"
#include <vector>

class ComboMeal : public MenuItem {
private:
    std::vector<std::string> includedItems;
    double originalCombinedPrice;

public:
    ComboMeal(int id, const std::string& name, double comboPrice, 
              double originalPrice, const std::vector<std::string>& contents, int stock = 30);

    // Getters
    const std::vector<std::string>& getIncludedItems() const { return includedItems; }
    double getOriginalPrice() const { return originalCombinedPrice; }
    double getSavings() const { return originalCombinedPrice - price; }

    void addSubItem(const std::string& itemName) { includedItems.push_back(itemName); }

    // Polymorphic overrides
    std::string getItemType() const override { return "Combo"; }
    void displayCard() const override;
    void displayRow() const override;
    std::string serialize() const override;
};

#endif // COMBO_MEAL_H
