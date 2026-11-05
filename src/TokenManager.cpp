#include "TokenManager.h"
#include "UIHelper.h"
#include <iostream>
#include <iomanip>

TokenManager::TokenManager(int startingToken) 
    : currentTokenCounter(startingToken) {}

int TokenManager::generateNextToken() {
    int token = currentTokenCounter++;
    return token;
}

void TokenManager::registerOrder(std::shared_ptr<Order> order) {
    if (order) {
        activeOrders[order->getTokenNumber()] = order;
    }
}

std::shared_ptr<Order> TokenManager::getOrderByToken(int tokenNumber) const {
    auto it = activeOrders.find(tokenNumber);
    if (it != activeOrders.end()) {
        return it->second;
    }
    for (const auto& ord : completedOrders) {
        if (ord->getTokenNumber() == tokenNumber) {
            return ord;
        }
    }
    return nullptr;
}

bool TokenManager::updateOrderStatus(int tokenNumber, OrderStatus newStatus) {
    auto it = activeOrders.find(tokenNumber);
    if (it != activeOrders.end()) {
        it->second->setStatus(newStatus);
        if (newStatus == OrderStatus::COMPLETED || newStatus == OrderStatus::CANCELLED) {
            completedOrders.push_back(it->second);
            activeOrders.erase(it);
        }
        return true;
    }
    return false;
}

bool TokenManager::advanceStatus(int tokenNumber) {
    auto it = activeOrders.find(tokenNumber);
    if (it == activeOrders.end()) return false;

    auto ord = it->second;
    switch (ord->getStatus()) {
        case OrderStatus::PLACED:
            ord->setStatus(OrderStatus::PREPARING);
            return true;
        case OrderStatus::PREPARING:
            ord->setStatus(OrderStatus::READY_FOR_PICKUP);
            return true;
        case OrderStatus::READY_FOR_PICKUP:
            ord->setStatus(OrderStatus::COMPLETED);
            completedOrders.push_back(ord);
            activeOrders.erase(it);
            return true;
        default:
            return false;
    }
}

void TokenManager::displayLiveQueueBoard() const {
    std::cout << "\n==========================================================================" << std::endl;
    std::cout << "             SMARTSERVE CANTEEN - LIVE DIGITAL TOKEN DISPLAY              " << std::endl;
    std::cout << "==========================================================================" << std::endl;

    std::vector<int> preparingTokens;
    std::vector<int> readyTokens;
    std::vector<int> placedTokens;

    for (const auto& pair : activeOrders) {
        if (pair.second->getStatus() == OrderStatus::PLACED) {
            placedTokens.push_back(pair.first);
        } else if (pair.second->getStatus() == OrderStatus::PREPARING) {
            preparingTokens.push_back(pair.first);
        } else if (pair.second->getStatus() == OrderStatus::READY_FOR_PICKUP) {
            readyTokens.push_back(pair.first);
        }
    }

    std::cout << "\n " << UI::BOLD << UI::YELLOW << "[ IN KITCHEN / PREPARING ]" << UI::RESET << std::endl;
    if (preparingTokens.empty() && placedTokens.empty()) {
        std::cout << "   (No active orders currently cooking)" << std::endl;
    } else {
        std::cout << "   Cooking Now: ";
        for (int t : preparingTokens) {
            std::cout << UI::BOLD << "TK-" << t << "  " << UI::RESET;
        }
        if (!placedTokens.empty()) {
            std::cout << "\n   Queued Next: ";
            for (int t : placedTokens) {
                std::cout << "TK-" << t << "  ";
            }
        }
        std::cout << std::endl;
    }

    std::cout << "\n " << UI::BOLD << UI::GREEN << "[ READY FOR PICKUP AT COUNTER ]" << UI::RESET << std::endl;
    if (readyTokens.empty()) {
        std::cout << "   (No orders awaiting pickup right now)" << std::endl;
    } else {
        std::cout << "   >>> ";
        for (int t : readyTokens) {
            std::cout << UI::BOLD << UI::BG_GREEN << UI::WHITE << " TK-" << t << " " << UI::RESET << "  ";
        }
        std::cout << " <<<" << std::endl;
        std::cout << "   " << UI::GREEN << "* Please proceed to Counter with your Token slip to collect your order!" << UI::RESET << std::endl;
    }

    std::cout << "\n [ RECENTLY COMPLETED ]" << std::endl;
    if (completedOrders.empty()) {
        std::cout << "   (No orders completed yet today)" << std::endl;
    } else {
        std::cout << "   ";
        int count = 0;
        for (auto rit = completedOrders.rbegin(); rit != completedOrders.rend() && count < 6; ++rit, ++count) {
            std::cout << "TK-" << (*rit)->getTokenNumber() << " (Done)  ";
        }
        std::cout << std::endl;
    }

    std::cout << "==========================================================================" << std::endl;
}

void TokenManager::displayCustomerStatus(int tokenNumber) const {
    auto ord = getOrderByToken(tokenNumber);
    if (!ord) {
        UI::printError("Token 'TK-" + std::to_string(tokenNumber) + "' not found in today's system.");
        return;
    }

    std::cout << "\n+------------------ TOKEN STATUS TRACKER ------------------+" << std::endl;
    std::cout << "| Token Number:   " << UI::BOLD << "TK-" << std::left << std::setw(34) << ord->getTokenNumber() << UI::RESET << " |" << std::endl;
    std::cout << "| Order ID:       #" << std::left << std::setw(38) << ord->getOrderId() << " |" << std::endl;
    std::cout << "| Customer:       " << std::left << std::setw(39) << (ord->getCustomer() ? ord->getCustomer()->getName() : "Guest") << " |" << std::endl;
    std::cout << "| Placed At:      " << std::left << std::setw(39) << ord->getTimestamp() << " |" << std::endl;

    std::string statusText = CommonUtils::orderStatusToString(ord->getStatus());
    std::string color = UI::YELLOW;
    if (ord->getStatus() == OrderStatus::READY_FOR_PICKUP) color = UI::GREEN;
    else if (ord->getStatus() == OrderStatus::COMPLETED) color = UI::CYAN;

    std::cout << "| Live Status:    " << color << UI::BOLD << std::left << std::setw(39) << statusText << UI::RESET << " |" << std::endl;
    std::cout << "| Total Amount:   " << std::left << std::setw(39) << CommonUtils::formatCurrency(ord->getGrandTotal()) << " |" << std::endl;
    std::cout << "| Items Ordered:  " << std::left << std::setw(39) << (std::to_string(ord->getItems().size()) + " item(s)") << " |" << std::endl;
    if (ord->getStatus() != OrderStatus::COMPLETED) {
        std::cout << "| Est. Wait Time: ~" << std::left << std::setw(38) << (std::to_string(ord->getEstimatedPrepMinutes()) + " mins") << " |" << std::endl;
    }
    std::cout << "+----------------------------------------------------------+" << std::endl;

    if (ord->getStatus() == OrderStatus::READY_FOR_PICKUP) {
        UI::printSuccess(">> DING! Your order is ready! Please collect it from Counter #1 <<");
    }
}
