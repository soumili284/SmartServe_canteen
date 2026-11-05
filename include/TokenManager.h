#ifndef TOKEN_MANAGER_H
#define TOKEN_MANAGER_H

#include "Order.h"
#include <map>
#include <vector>
#include <memory>

class TokenManager {
private:
    int currentTokenCounter;
    std::map<int, std::shared_ptr<Order>> activeOrders;     // Token -> Order
    std::vector<std::shared_ptr<Order>> completedOrders;

public:
    TokenManager(int startingToken = 101);

    int generateNextToken();
    void registerOrder(std::shared_ptr<Order> order);
    std::shared_ptr<Order> getOrderByToken(int tokenNumber) const;
    
    // Status transitions
    bool updateOrderStatus(int tokenNumber, OrderStatus newStatus);
    bool advanceStatus(int tokenNumber);

    // Live display board
    void displayLiveQueueBoard() const;
    void displayCustomerStatus(int tokenNumber) const;

    // Reporting
    int getActiveOrderCount() const { return static_cast<int>(activeOrders.size()); }
    int getCompletedOrderCount() const { return static_cast<int>(completedOrders.size()); }
    const std::map<int, std::shared_ptr<Order>>& getActiveOrders() const { return activeOrders; }
    const std::vector<std::shared_ptr<Order>>& getCompletedOrders() const { return completedOrders; }
};

#endif // TOKEN_MANAGER_H
