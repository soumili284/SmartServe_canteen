#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include "Menu.h"
#include "Order.h"
#include <string>
#include <vector>

class FileStorage {
private:
    std::string menuFilePath;
    std::string orderHistoryPath;
    std::string receiptsDir;

public:
    FileStorage(const std::string& menuPath = "data/menu.txt",
                const std::string& historyPath = "data/orders_history.txt",
                const std::string& receiptFolder = "receipts/");

    bool saveMenuToFile(const Menu& menu) const;
    bool loadMenuFromFile(Menu& menu) const;
    bool saveReceiptToFile(const Order& order) const;
    bool logOrderToHistory(const Order& order) const;
    std::vector<std::string> readOrderHistory() const;
};

#endif // FILE_STORAGE_H
