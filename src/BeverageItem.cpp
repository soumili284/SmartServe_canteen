#include "BeverageItem.h"
#include "UIHelper.h"
#include <iostream>
#include <sstream>

BeverageItem::BeverageItem(int id, const std::string& name, double price, ItemCategory category,
                           bool isHot, int volumeMl, bool caffeine, int stock)
    : MenuItem(id, name, price, category, stock),
      isHot(isHot), volumeMl(volumeMl), containsCaffeine(caffeine) {}

void BeverageItem::displayCard() const {
    std::string tempTag = isHot ? UI::YELLOW + "[HOT]" + UI::RESET : UI::CYAN + "[COLD]" + UI::RESET;

    std::cout << "\n+-------------------------------------------------------------+" << std::endl;
    std::cout << "| " << UI::BOLD << std::left << std::setw(40) << (name + " (" + std::to_string(id) + ")") 
              << UI::RESET << " " << tempTag << " |" << std::endl;
    std::cout << "| Category: " << std::left << std::setw(47) << CommonUtils::categoryToString(category) << " |" << std::endl;
    std::cout << "| Price: " << std::left << std::setw(50) << CommonUtils::formatCurrency(price) << " |" << std::endl;
    std::cout << "| Serving Size:" << std::left << std::setw(44) << (std::to_string(volumeMl) + " ml") << " |" << std::endl;
    std::cout << "| Caffeine:    " << std::left << std::setw(44) << (containsCaffeine ? "Yes" : "No (Caffeine-Free)") << " |" << std::endl;
    std::cout << "| Stock:       " << std::left << std::setw(44) << (isAvailable() ? std::to_string(stockQuantity) + " available" : "OUT OF STOCK") << " |" << std::endl;
    std::cout << "+-------------------------------------------------------------+" << std::endl;
}

void BeverageItem::displayRow() const {
    std::string tempTag = isHot ? UI::YELLOW + "[H]" + UI::RESET : UI::CYAN + "[C]" + UI::RESET;

    std::string rawStatus = isAvailable() ? 
        (stockQuantity < 10 ? std::to_string(stockQuantity) + " left" : "In Stock (" + std::to_string(stockQuantity) + ")") : 
        "Out of Stock";
    std::string stockColor = isAvailable() ? (stockQuantity < 10 ? UI::YELLOW : UI::GREEN) : UI::RED;

    std::cout << " | " << std::setw(4) << id
              << " | " << tempTag << " " << std::left << std::setw(23) << name.substr(0, 23)
              << " | " << std::left << std::setw(14) << CommonUtils::categoryToString(category)
              << " | " << std::right << std::setw(8) << std::fixed << std::setprecision(2) << price
              << " | " << stockColor << std::left << std::setw(20) << rawStatus << UI::RESET
              << " |" << std::endl;
}

std::string BeverageItem::serialize() const {
    std::ostringstream oss;
    oss << "BEV;" << id << ";" << name << ";" << price << ";" 
        << static_cast<int>(category) << ";" << (isHot ? 1 : 0) << ";"
        << volumeMl << ";" << (containsCaffeine ? 1 : 0) << ";" 
        << stockQuantity << ";" << (available ? 1 : 0);
    return oss.str();
}
