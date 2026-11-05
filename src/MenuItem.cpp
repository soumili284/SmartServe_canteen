#include "MenuItem.h"
#include "Exceptions.h"
#include "UIHelper.h"
#include <iomanip>

MenuItem::MenuItem(int id, const std::string& name, double price, ItemCategory category, int stock)
    : id(id), name(name), price(price), category(category), stockQuantity(stock), available(true) {
    if (price < 0.0) {
        throw InvalidInputException("Item price cannot be negative");
    }
    if (stock < 0) {
        stockQuantity = 0;
    }
}

void MenuItem::setPrice(double newPrice) {
    if (newPrice < 0.0) {
        throw InvalidInputException("Item price cannot be negative");
    }
    price = newPrice;
}

void MenuItem::setStockQuantity(int newStock) {
    if (newStock < 0) {
        throw InvalidInputException("Stock quantity cannot be negative");
    }
    stockQuantity = newStock;
    if (stockQuantity == 0) {
        available = false;
    } else {
        available = true;
    }
}

void MenuItem::reduceStock(int quantity) {
    if (quantity <= 0) {
        throw InvalidQuantityException(quantity);
    }
    if (quantity > stockQuantity) {
        throw OutOfStockException(name, quantity, stockQuantity);
    }
    stockQuantity -= quantity;
    if (stockQuantity == 0) {
        available = false;
    }
}

void MenuItem::restock(int quantity) {
    if (quantity > 0) {
        stockQuantity += quantity;
        available = true;
    }
}

void MenuItem::displayRow() const {
    std::string rawStatus = isAvailable() ? 
        (stockQuantity < 10 ? std::to_string(stockQuantity) + " left" : "In Stock (" + std::to_string(stockQuantity) + ")") : 
        "Out of Stock";
    std::string color = isAvailable() ? (stockQuantity < 10 ? UI::YELLOW : UI::GREEN) : UI::RED;

    std::cout << " | " << std::setw(4) << id
              << " | " << std::left << std::setw(27) << name.substr(0, 27)
              << " | " << std::left << std::setw(14) << CommonUtils::categoryToString(category)
              << " | " << std::right << std::setw(8) << std::fixed << std::setprecision(2) << price
              << " | " << color << std::left << std::setw(20) << rawStatus << UI::RESET
              << " |" << std::endl;
}

std::ostream& operator<<(std::ostream& os, const MenuItem& item) {
    os << "[" << item.id << "] " << item.name << " - " 
       << CommonUtils::formatCurrency(item.price) 
       << " (" << CommonUtils::categoryToString(item.category) << ")";
    return os;
}
