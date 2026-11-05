#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

#include "MenuItem.h"

class FoodItem : public MenuItem {
private:
    DietaryType dietaryType;
    std::string spiceLevel; // "Mild", "Medium", "Spicy"
    int prepTimeMinutes;

public:
    FoodItem(int id, const std::string& name, double price, ItemCategory category,
             DietaryType dietary, const std::string& spice = "Medium", 
             int prepTime = 10, int stock = 50);

    // Getters
    DietaryType getDietaryType() const { return dietaryType; }
    std::string getSpiceLevel() const { return spiceLevel; }
    int getPrepTimeMinutes() const { return prepTimeMinutes; }

    // Setters
    void setSpiceLevel(const std::string& spice) { spiceLevel = spice; }
    void setPrepTimeMinutes(int minutes) { prepTimeMinutes = minutes; }

    // Polymorphic overrides
    std::string getItemType() const override { return "Food"; }
    void displayCard() const override;
    void displayRow() const override;
    std::string serialize() const override;
};

#endif // FOOD_ITEM_H
