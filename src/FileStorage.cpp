#include "FileStorage.h"
#include "FoodItem.h"
#include "BeverageItem.h"
#include "ComboMeal.h"
#include "UIHelper.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

FileStorage::FileStorage(const std::string& menuPath,
                         const std::string& historyPath,
                         const std::string& receiptFolder)
    : menuFilePath(menuPath), orderHistoryPath(historyPath), receiptsDir(receiptFolder) {}

bool FileStorage::saveMenuToFile(const Menu& menu) const {
    std::ofstream outFile(menuFilePath);
    if (!outFile.is_open()) {
        return false;
    }

    outFile << "# SmartServe Canteen Menu Data\n";
    outFile << "# Format: TYPE;id;name;price;...\n";

    for (const auto& item : menu.getAllItems()) {
        outFile << item->serialize() << "\n";
    }

    outFile.close();
    return true;
}

bool FileStorage::loadMenuFromFile(Menu& menu) const {
    std::ifstream inFile(menuFilePath);
    if (!inFile.is_open()) {
        menu.loadDefaultMenu();
        saveMenuToFile(menu);
        return false;
    }

    std::string line;
    bool hasItems = false;

    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> parts;

        while (std::getline(ss, token, ';')) {
            parts.push_back(token);
        }

        if (parts.empty()) continue;

        std::string type = parts[0];
        try {
            if (type == "FOOD" && parts.size() >= 10) {
                int id = std::stoi(parts[1]);
                std::string name = parts[2];
                double price = std::stod(parts[3]);
                ItemCategory cat = static_cast<ItemCategory>(std::stoi(parts[4]));
                DietaryType diet = static_cast<DietaryType>(std::stoi(parts[5]));
                std::string spice = parts[6];
                int prep = std::stoi(parts[7]);
                int stock = std::stoi(parts[8]);
                bool avail = (std::stoi(parts[9]) == 1);

                auto food = std::make_shared<FoodItem>(id, name, price, cat, diet, spice, prep, stock);
                food->setAvailable(avail);
                menu.addItem(food);
                hasItems = true;
            } else if (type == "BEV" && parts.size() >= 10) {
                int id = std::stoi(parts[1]);
                std::string name = parts[2];
                double price = std::stod(parts[3]);
                ItemCategory cat = static_cast<ItemCategory>(std::stoi(parts[4]));
                bool isHot = (std::stoi(parts[5]) == 1);
                int vol = std::stoi(parts[6]);
                bool caffeine = (std::stoi(parts[7]) == 1);
                int stock = std::stoi(parts[8]);
                bool avail = (std::stoi(parts[9]) == 1);

                auto bev = std::make_shared<BeverageItem>(id, name, price, cat, isHot, vol, caffeine, stock);
                bev->setAvailable(avail);
                menu.addItem(bev);
                hasItems = true;
            } else if (type == "COMBO" && parts.size() >= 8) {
                int id = std::stoi(parts[1]);
                std::string name = parts[2];
                double price = std::stod(parts[3]);
                ItemCategory cat = static_cast<ItemCategory>(std::stoi(parts[4]));
                (void)cat;
                double origPrice = std::stod(parts[5]);
                
                std::string includedStr = parts[6];
                std::vector<std::string> comboItems;
                std::stringstream iss(includedStr);
                std::string subItem;
                while (std::getline(iss, subItem, ',')) {
                    if (!subItem.empty()) comboItems.push_back(subItem);
                }

                int stock = std::stoi(parts[7]);
                bool avail = (parts.size() >= 9) ? (std::stoi(parts[8]) == 1) : true;

                auto combo = std::make_shared<ComboMeal>(id, name, price, origPrice, comboItems, stock);
                combo->setAvailable(avail);
                menu.addItem(combo);
                hasItems = true;
            }
        } catch (...) {
            // Skip corrupted line gracefully
            continue;
        }
    }

    inFile.close();

    if (!hasItems) {
        menu.loadDefaultMenu();
        saveMenuToFile(menu);
        return false;
    }

    return true;
}

bool FileStorage::saveReceiptToFile(const Order& order) const {
    std::string path = receiptsDir + "/receipt_TK" + std::to_string(order.getTokenNumber()) + ".txt";
    std::ofstream outFile(path);
    if (!outFile.is_open()) {
        return false;
    }

    outFile << order.generateReceiptString();
    outFile.close();
    return true;
}

bool FileStorage::logOrderToHistory(const Order& order) const {
    std::ofstream outFile(orderHistoryPath, std::ios::app);
    if (!outFile.is_open()) {
        return false;
    }

    outFile << order.getTimestamp() << ";"
            << order.getOrderId() << ";"
            << order.getTokenNumber() << ";"
            << (order.getCustomer() ? order.getCustomer()->getName() : "Guest") << ";"
            << (order.getCustomer() ? order.getCustomer()->getRoleName() : "Unknown") << ";"
            << std::fixed << std::setprecision(2) << order.getGrandTotal() << ";"
            << order.getItems().size() << ";"
            << (order.getPayment() ? order.getPayment()->getMethodName() : "Unknown") << "\n";

    outFile.close();
    return true;
}

std::vector<std::string> FileStorage::readOrderHistory() const {
    std::vector<std::string> records;
    std::ifstream inFile(orderHistoryPath);
    if (!inFile.is_open()) return records;

    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty()) {
            records.push_back(line);
        }
    }
    inFile.close();
    return records;
}
