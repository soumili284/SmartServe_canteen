#include "Menu.h"
#include "UIHelper.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

Menu::Menu() {
    // Empty constructor; loadDefaultMenu() or loadMenuFromFile() will populate
}

void Menu::loadDefaultMenu() {
    items.clear();

    // Breakfast
    items.push_back(std::make_shared<FoodItem>(101, "Masala Dosa", 60.0, ItemCategory::BREAKFAST, DietaryType::VEG, "Medium", 8, 40));
    items.push_back(std::make_shared<FoodItem>(102, "Idli Sambar (2 pcs)", 40.0, ItemCategory::BREAKFAST, DietaryType::VEG, "Mild", 5, 50));
    items.push_back(std::make_shared<FoodItem>(103, "Indori Poha with Sev", 35.0, ItemCategory::BREAKFAST, DietaryType::VEG, "Mild", 4, 45));
    items.push_back(std::make_shared<FoodItem>(104, "Aloo Paratha with Curd", 50.0, ItemCategory::BREAKFAST, DietaryType::VEG, "Medium", 10, 35));

    // Main Course
    items.push_back(std::make_shared<FoodItem>(201, "Special Veg Thali", 120.0, ItemCategory::MAIN_COURSE, DietaryType::VEG, "Medium", 12, 30));
    items.push_back(std::make_shared<FoodItem>(202, "Hyderabadi Chicken Biryani", 160.0, ItemCategory::MAIN_COURSE, DietaryType::NON_VEG, "Spicy", 10, 35));
    items.push_back(std::make_shared<FoodItem>(203, "Paneer Butter Masala + Naan", 140.0, ItemCategory::MAIN_COURSE, DietaryType::VEG, "Medium", 12, 25));
    items.push_back(std::make_shared<FoodItem>(204, "Dilli Rajma Chawal Bowl", 80.0, ItemCategory::MAIN_COURSE, DietaryType::VEG, "Medium", 6, 40));

    // Snacks
    items.push_back(std::make_shared<FoodItem>(301, "Crispy Samosa (2 pcs)", 30.0, ItemCategory::SNACKS, DietaryType::VEG, "Spicy", 3, 60));
    items.push_back(std::make_shared<FoodItem>(302, "Veg Cheese Grilled Sandwich", 70.0, ItemCategory::SNACKS, DietaryType::VEG, "Mild", 7, 30));
    items.push_back(std::make_shared<FoodItem>(303, "Paneer Tikka Kathi Roll", 85.0, ItemCategory::SNACKS, DietaryType::VEG, "Spicy", 8, 25));
    items.push_back(std::make_shared<FoodItem>(304, "Peri Peri French Fries", 65.0, ItemCategory::SNACKS, DietaryType::VEGAN, "Spicy", 6, 40));

    // Beverages
    items.push_back(std::make_shared<BeverageItem>(401, "Adrak Masala Chai", 15.0, ItemCategory::BEVERAGES, true, 150, true, 100));
    items.push_back(std::make_shared<BeverageItem>(402, "South Indian Filter Coffee", 25.0, ItemCategory::BEVERAGES, true, 180, true, 80));
    items.push_back(std::make_shared<BeverageItem>(403, "Thick Cold Coffee w/ Cream", 55.0, ItemCategory::BEVERAGES, false, 300, true, 50));
    items.push_back(std::make_shared<BeverageItem>(404, "Chilled Fresh Lime Soda", 35.0, ItemCategory::BEVERAGES, false, 250, false, 60));

    // Desserts
    items.push_back(std::make_shared<FoodItem>(501, "Hot Gulab Jamun (2 pcs)", 40.0, ItemCategory::DESSERTS, DietaryType::VEG, "Mild", 2, 40));
    items.push_back(std::make_shared<FoodItem>(502, "Warm Brownie with Vanilla", 75.0, ItemCategory::DESSERTS, DietaryType::VEG, "Mild", 4, 25));

    // Combos
    std::vector<std::string> combo1 = {"Veg Cheese Sandwich", "Peri Peri Fries", "Cold Coffee"};
    items.push_back(std::make_shared<ComboMeal>(601, "Campus Hunger Buster Combo", 160.0, 190.0, combo1, 20));

    std::vector<std::string> combo2 = {"Masala Dosa", "Idli (1 pc)", "Filter Coffee"};
    items.push_back(std::make_shared<ComboMeal>(602, "Morning South Indian Combo", 95.0, 115.0, combo2, 25));
}

void Menu::addItem(std::shared_ptr<MenuItem> item) {
    if (item) {
        items.push_back(item);
    }
}

bool Menu::removeItem(int id) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if ((*it)->getId() == id) {
            items.erase(it);
            return true;
        }
    }
    return false;
}

std::shared_ptr<MenuItem> Menu::getItemById(int id) const {
    for (const auto& item : items) {
        if (item->getId() == id) {
            return item;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<MenuItem>> Menu::searchByName(const std::string& query) const {
    std::vector<std::shared_ptr<MenuItem>> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    for (const auto& item : items) {
        std::string lowerName = item->getName();
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        if (lowerName.find(lowerQuery) != std::string::npos) {
            results.push_back(item);
        }
    }
    return results;
}

std::vector<std::shared_ptr<MenuItem>> Menu::getByCategory(ItemCategory cat) const {
    std::vector<std::shared_ptr<MenuItem>> results;
    for (const auto& item : items) {
        if (item->getCategory() == cat) {
            results.push_back(item);
        }
    }
    return results;
}

std::vector<std::shared_ptr<MenuItem>> Menu::getByDietary(DietaryType dt) const {
    std::vector<std::shared_ptr<MenuItem>> results;
    for (const auto& item : items) {
        auto food = std::dynamic_pointer_cast<FoodItem>(item);
        if (food && food->getDietaryType() == dt) {
            results.push_back(item);
        }
    }
    return results;
}

bool Menu::updateItemPrice(int id, double newPrice) {
    auto item = getItemById(id);
    if (item) {
        item->setPrice(newPrice);
        return true;
    }
    return false;
}

bool Menu::updateItemStock(int id, int addedStock) {
    auto item = getItemById(id);
    if (item) {
        item->restock(addedStock);
        return true;
    }
    return false;
}

bool Menu::toggleAvailability(int id) {
    auto item = getItemById(id);
    if (item) {
        item->setAvailable(!item->isAvailable());
        return true;
    }
    return false;
}

int Menu::getNextAvailableId() const {
    int maxId = 100;
    for (const auto& item : items) {
        if (item->getId() > maxId) {
            maxId = item->getId();
        }
    }
    return maxId + 1;
}

static void printMenuTableHeader() {
    std::cout << "+------+-----------------------------+----------------+----------+----------------------+" << std::endl;
    std::cout << "| " << UI::BOLD << std::setw(4) << "ID"
              << " | " << std::left << std::setw(27) << "Item Name"
              << " | " << std::left << std::setw(14) << "Category"
              << " | " << std::right << std::setw(8) << "Price"
              << " | " << std::left << std::setw(20) << "Stock Status"
              << UI::RESET << " |" << std::endl;
    std::cout << "+------+-----------------------------+----------------+----------+----------------------+" << std::endl;
}

void Menu::displayAll() const {
    if (items.empty()) {
        UI::printWarning("No items available in the canteen menu.");
        return;
    }

    std::cout << "\nLegend: " << UI::GREEN << "[V] Veg" << UI::RESET << " | "
              << UI::RED << "[N] Non-Veg" << UI::RESET << " | "
              << UI::GREEN << "[L] Vegan" << UI::RESET << " | "
              << UI::YELLOW << "[H] Hot" << UI::RESET << " | "
              << UI::CYAN << "[C] Cold" << UI::RESET << " | "
              << UI::MAGENTA << "[CB] Combo" << UI::RESET << "\n" << std::endl;

    printMenuTableHeader();
    for (const auto& item : items) {
        item->displayRow();
    }
    std::cout << "+------+-----------------------------+----------------+----------+----------------------+" << std::endl;
}

void Menu::displayCategory(ItemCategory cat) const {
    auto catItems = getByCategory(cat);
    if (catItems.empty()) {
        UI::printWarning("No items found under category: " + CommonUtils::categoryToString(cat));
        return;
    }

    std::cout << "\n>>> " << UI::BOLD << UI::CYAN << CommonUtils::categoryToString(cat) << UI::RESET << " <<<" << std::endl;
    printMenuTableHeader();
    for (const auto& item : catItems) {
        item->displayRow();
    }
    std::cout << "+------+-----------------------------+----------------+----------+----------------------+" << std::endl;
}

void Menu::displaySearchResults(const std::vector<std::shared_ptr<MenuItem>>& resultList, const std::string& title) const {
    if (resultList.empty()) {
        UI::printWarning("No items matched your search criteria: " + title);
        return;
    }

    std::cout << "\n>>> " << UI::BOLD << UI::CYAN << title << " (" << resultList.size() << " found)" << UI::RESET << " <<<" << std::endl;
    printMenuTableHeader();
    for (const auto& item : resultList) {
        item->displayRow();
    }
    std::cout << "+------+-----------------------------+----------------+----------+----------------------+" << std::endl;
}
