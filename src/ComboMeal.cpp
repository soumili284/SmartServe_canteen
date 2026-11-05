#include "ComboMeal.h"
#include "UIHelper.h"
#include <iostream>
#include <sstream>

ComboMeal::ComboMeal(int id, const std::string& name, double comboPrice, 
                     double originalPrice, const std::vector<std::string>& contents, int stock)
    : MenuItem(id, name, comboPrice, ItemCategory::COMBOS, stock),
      includedItems(contents), originalCombinedPrice(originalPrice) {}

void ComboMeal::displayCard() const {
    std::cout << "\n+-------------------------------------------------------------+" << std::endl;
    std::cout << "| " << UI::BOLD << std::left << std::setw(42) << (name + " (" + std::to_string(id) + ")") 
              << UI::MAGENTA << "[COMBO DEAL]" << UI::RESET << " |" << std::endl;
    std::cout << "| Category: " << std::left << std::setw(47) << CommonUtils::categoryToString(category) << " |" << std::endl;
    std::cout << "| Combo Price: " << std::left << std::setw(44) << CommonUtils::formatCurrency(price) << " |" << std::endl;
    std::cout << "| Regular Value: " << std::left << std::setw(42) << CommonUtils::formatCurrency(originalCombinedPrice) << " |" << std::endl;
    std::cout << "| You Save:    " << UI::GREEN << std::left << std::setw(44) << CommonUtils::formatCurrency(getSavings()) << UI::RESET << " |" << std::endl;
    std::cout << "| Included Items:                                             |" << std::endl;
    for (const auto& itm : includedItems) {
        std::cout << "|   * " << std::left << std::setw(54) << itm << " |" << std::endl;
    }
    std::cout << "| Stock:       " << std::left << std::setw(44) << (isAvailable() ? std::to_string(stockQuantity) + " available" : "OUT OF STOCK") << " |" << std::endl;
    std::cout << "+-------------------------------------------------------------+" << std::endl;
}

void ComboMeal::displayRow() const {
    std::string tag = UI::MAGENTA + "[CB]" + UI::RESET;

    std::string rawStatus = isAvailable() ? 
        (stockQuantity < 10 ? std::to_string(stockQuantity) + " left" : "In Stock (" + std::to_string(stockQuantity) + ")") : 
        "Out of Stock";
    std::string stockColor = isAvailable() ? (stockQuantity < 10 ? UI::YELLOW : UI::GREEN) : UI::RED;

    std::cout << " | " << std::setw(4) << id
              << " | " << tag << " " << std::left << std::setw(23) << name.substr(0, 23)
              << " | " << std::left << std::setw(14) << CommonUtils::categoryToString(category)
              << " | " << std::right << std::setw(8) << std::fixed << std::setprecision(2) << price
              << " | " << stockColor << std::left << std::setw(20) << rawStatus << UI::RESET
              << " |" << std::endl;
}

std::string ComboMeal::serialize() const {
    std::ostringstream oss;
    oss << "COMBO;" << id << ";" << name << ";" << price << ";" 
        << static_cast<int>(category) << ";" << originalCombinedPrice << ";";
    
    for (size_t i = 0; i < includedItems.size(); ++i) {
        oss << includedItems[i];
        if (i + 1 < includedItems.size()) {
            oss << ",";
        }
    }
    oss << ";" << stockQuantity << ";" << (available ? 1 : 0);
    return oss.str();
}
