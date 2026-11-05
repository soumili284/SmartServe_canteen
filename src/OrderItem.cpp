#include "OrderItem.h"
#include "Exceptions.h"
#include "UIHelper.h"
#include <iostream>
#include <iomanip>

OrderItem::OrderItem(std::shared_ptr<MenuItem> menuItem, int qty, const std::string& instructions)
    : item(menuItem), quantity(qty), specialInstructions(instructions) {
    if (qty <= 0) {
        throw InvalidQuantityException(qty);
    }
}

void OrderItem::setQuantity(int qty) {
    if (qty <= 0) {
        throw InvalidQuantityException(qty);
    }
    quantity = qty;
}

void OrderItem::addQuantity(int additionalQty) {
    if (additionalQty <= 0) {
        throw InvalidQuantityException(additionalQty);
    }
    quantity += additionalQty;
}

double OrderItem::getSubtotal() const {
    if (item) {
        return item->getPrice() * quantity;
    }
    return 0.0;
}

void OrderItem::displayRow(int itemIndex) const {
    std::string itemName = item ? item->getName() : "Unknown Item";
    double unitPrice = item ? item->getPrice() : 0.0;

    std::cout << " | " << std::setw(3) << itemIndex
              << " | " << std::left << std::setw(28) << itemName.substr(0, 28)
              << " | " << std::right << std::setw(5) << quantity
              << " | " << std::right << std::setw(10) << std::fixed << std::setprecision(2) << unitPrice
              << " | " << std::right << std::setw(10) << std::fixed << std::setprecision(2) << getSubtotal()
              << " |" << std::endl;

    if (!specialInstructions.empty()) {
        std::cout << " |     Note: " << std::left << std::setw(56) << ("\"" + specialInstructions + "\"") << " |" << std::endl;
    }
}
