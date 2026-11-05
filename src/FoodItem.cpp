#include "FoodItem.h"
#include "UIHelper.h"
#include <iostream>
#include <sstream>

FoodItem::FoodItem(int id, const std::string& name, double price, ItemCategory category,
                   DietaryType dietary, const std::string& spice, int prepTime, int stock)
    : MenuItem(id, name, price, category, stock), 
      dietaryType(dietary), spiceLevel(spice), prepTimeMinutes(prepTime) {}

void FoodItem::displayCard() const {
    std::string dietTag = (dietaryType == DietaryType::VEG) ? 
        UI::GREEN + "[VEG]" + UI::RESET : (dietaryType == DietaryType::VEGAN ? 
        UI::GREEN + "[VEGAN]" + UI::RESET : UI::RED + "[NON-VEG]" + UI::RESET);

    std::cout << "\n+-------------------------------------------------------------+" << std::endl;
    std::cout << "| " << UI::BOLD << std::left << std::setw(40) << (name + " (" + std::to_string(id) + ")") 
              << UI::RESET << " " << dietTag << " |" << std::endl;
    std::cout << "| Category: " << std::left << std::setw(47) << CommonUtils::categoryToString(category) << " |" << std::endl;
    std::cout << "| Price: " << std::left << std::setw(50) << CommonUtils::formatCurrency(price) << " |" << std::endl;
    std::cout << "| Spice Level: " << std::left << std::setw(44) << spiceLevel << " |" << std::endl;
    std::cout << "| Prep Time:   " << std::left << std::setw(44) << (std::to_string(prepTimeMinutes) + " mins") << " |" << std::endl;
    std::cout << "| Stock:       " << std::left << std::setw(44) << (isAvailable() ? std::to_string(stockQuantity) + " available" : "OUT OF STOCK") << " |" << std::endl;
    std::cout << "+-------------------------------------------------------------+" << std::endl;
}

void FoodItem::displayRow() const {
    std::string dietTag = (dietaryType == DietaryType::VEG) ? 
        UI::GREEN + "[V]" + UI::RESET : (dietaryType == DietaryType::VEGAN ? 
        UI::GREEN + "[L]" + UI::RESET : UI::RED + "[N]" + UI::RESET);

    std::string rawStatus = isAvailable() ? 
        (stockQuantity < 10 ? std::to_string(stockQuantity) + " left" : "In Stock (" + std::to_string(stockQuantity) + ")") : 
        "Out of Stock";
    std::string stockColor = isAvailable() ? (stockQuantity < 10 ? UI::YELLOW : UI::GREEN) : UI::RED;

    std::cout << " | " << std::setw(4) << id
              << " | " << dietTag << " " << std::left << std::setw(23) << name.substr(0, 23)
              << " | " << std::left << std::setw(14) << CommonUtils::categoryToString(category)
              << " | " << std::right << std::setw(8) << std::fixed << std::setprecision(2) << price
              << " | " << stockColor << std::left << std::setw(20) << rawStatus << UI::RESET
              << " |" << std::endl;
}

std::string FoodItem::serialize() const {
    std::ostringstream oss;
    oss << "FOOD;" << id << ";" << name << ";" << price << ";" 
        << static_cast<int>(category) << ";" << static_cast<int>(dietaryType) << ";"
        << spiceLevel << ";" << prepTimeMinutes << ";" << stockQuantity << ";" << (available ? 1 : 0);
    return oss.str();
}
