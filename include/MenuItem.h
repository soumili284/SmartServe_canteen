#ifndef MENU_ITEM_H
#define MENU_ITEM_H

#include "Common.h"
#include <string>
#include <iostream>
#include <iomanip>

class MenuItem {
protected:
    int id;
    std::string name;
    double price;
    ItemCategory category;
    int stockQuantity;
    bool available;

public:
    MenuItem(int id, const std::string& name, double price, ItemCategory category, int stock = 50);
    virtual ~MenuItem() = default;

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }
    ItemCategory getCategory() const { return category; }
    int getStockQuantity() const { return stockQuantity; }
    bool isAvailable() const { return available && stockQuantity > 0; }

    // Setters / Mutators
    void setPrice(double newPrice);
    void setStockQuantity(int newStock);
    void setAvailable(bool status) { available = status; }
    void reduceStock(int quantity);
    void restock(int quantity);

    // Pure virtual functions (Runtime Polymorphism)
    virtual std::string getItemType() const = 0;
    virtual std::string serialize() const = 0;
    virtual void displayCard() const = 0;

    // Virtual function with default implementation
    virtual void displayRow() const;

    // Operator Overloading
    bool operator==(const MenuItem& other) const {
        return this->id == other.id;
    }

    friend std::ostream& operator<<(std::ostream& os, const MenuItem& item);
};

#endif // MENU_ITEM_H
