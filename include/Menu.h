#ifndef MENU_H
#define MENU_H

#include "MenuItem.h"
#include "FoodItem.h"
#include "BeverageItem.h"
#include "ComboMeal.h"
#include "Exceptions.h"
#include <vector>
#include <memory>
#include <string>

class Menu {
private:
    std::vector<std::shared_ptr<MenuItem>> items;

public:
    Menu();

    // Population
    void loadDefaultMenu();
    void addItem(std::shared_ptr<MenuItem> item);
    bool removeItem(int id);

    // Queries
    std::shared_ptr<MenuItem> getItemById(int id) const;
    std::vector<std::shared_ptr<MenuItem>> searchByName(const std::string& query) const;
    std::vector<std::shared_ptr<MenuItem>> getByCategory(ItemCategory cat) const;
    std::vector<std::shared_ptr<MenuItem>> getByDietary(DietaryType dt) const;
    const std::vector<std::shared_ptr<MenuItem>>& getAllItems() const { return items; }

    // Admin Modifications
    bool updateItemPrice(int id, double newPrice);
    bool updateItemStock(int id, int addedStock);
    bool toggleAvailability(int id);

    // Display
    void displayAll() const;
    void displayCategory(ItemCategory cat) const;
    void displaySearchResults(const std::vector<std::shared_ptr<MenuItem>>& resultList, const std::string& title) const;

    int getNextAvailableId() const;
    size_t size() const { return items.size(); }
};

#endif // MENU_H
