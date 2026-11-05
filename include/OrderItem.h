#ifndef ORDER_ITEM_H
#define ORDER_ITEM_H

#include "MenuItem.h"
#include <memory>
#include <string>

class OrderItem {
private:
    std::shared_ptr<MenuItem> item;
    int quantity;
    std::string specialInstructions;

public:
    OrderItem(std::shared_ptr<MenuItem> menuItem, int qty, const std::string& instructions = "");

    std::shared_ptr<MenuItem> getItem() const { return item; }
    int getQuantity() const { return quantity; }
    std::string getInstructions() const { return specialInstructions; }

    void setQuantity(int qty);
    void addQuantity(int additionalQty);
    void setInstructions(const std::string& instructions) { specialInstructions = instructions; }

    double getSubtotal() const;
    void displayRow(int itemIndex) const;

    bool operator==(const OrderItem& other) const {
        return item->getId() == other.item->getId();
    }
};

#endif // ORDER_ITEM_H
