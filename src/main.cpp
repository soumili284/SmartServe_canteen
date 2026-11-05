#include "Common.h"
#include "Exceptions.h"
#include "MenuItem.h"
#include "FoodItem.h"
#include "BeverageItem.h"
#include "ComboMeal.h"
#include "Menu.h"
#include "OrderItem.h"
#include "Order.h"
#include "Payment.h"
#include "User.h"
#include "TokenManager.h"
#include "FileStorage.h"
#include "UIHelper.h"

#include <iostream>
#include <memory>
#include <iomanip>
#include <limits>
#include <vector>

// Forward declarations of menu controllers
void handleCustomerOrdering(std::shared_ptr<User> currentUser, Menu& menu, 
                            TokenManager& tokenMgr, FileStorage& storage, int& orderIdCounter);
void handleAdminPortal(Menu& menu, TokenManager& tokenMgr, FileStorage& storage);
void handleTokenLookup(const TokenManager& tokenMgr);
void handleViewMenuCategories(const Menu& menu);
void handleAddItemToCart(std::shared_ptr<Order> currentOrder, Menu& menu);
void handleModifyCart(std::shared_ptr<Order> currentOrder);
bool handleCheckout(std::shared_ptr<Order> currentOrder, Menu& menu, 
                    TokenManager& tokenMgr, FileStorage& storage);
void handleKitchenQueue(TokenManager& tokenMgr);
void handleMenuManagement(Menu& menu, FileStorage& storage);
void handleSalesReport(const FileStorage& storage, const TokenManager& tokenMgr);

int main() {
    UI::initConsole();
    Menu canteenMenu;
    FileStorage storage("data/menu.txt", "data/orders_history.txt", "receipts");
    TokenManager tokenManager(101);

    // Load menu from file, or fall back to default
    if (!storage.loadMenuFromFile(canteenMenu)) {
        std::cout << "[i] Initialized fresh canteen menu with default offerings." << std::endl;
    }

    int orderIdCounter = 1001;

    bool running = true;
    while (running) {
        UI::clearScreen();
        UI::printBanner();

        std::cout << "  Select User Role / Operation:\n" << std::endl;
        std::cout << "  [1] Student Portal (Order Food, Combos & Token Tracking)" << std::endl;
        std::cout << "  [2] Faculty / Staff Dining (Privilege Discount Ordering)" << std::endl;
        std::cout << "  [3] Canteen Admin & Kitchen Management (Staff Login)" << std::endl;
        std::cout << "  [4] Live Digital Token Queue Display Board" << std::endl;
        std::cout << "  [5] Track Existing Token Number" << std::endl;
        std::cout << "  [6] View Canteen Menu & Pricing" << std::endl;
        std::cout << "  [7] About SmartServe System" << std::endl;
        std::cout << "  [0] Exit Application" << std::endl;
        UI::printDivider('-', 60);

        int choice = UI::getIntInput("  Enter your option (0-7): ", 0, 7);

        switch (choice) {
            case 1: {
                // Student Login / Info
                UI::printHeader("STUDENT DINING LOGIN");
                std::string name = UI::getStringInput("Enter Student Full Name: ");
                std::string roll = UI::getStringInput("Enter College Roll No. (e.g., CS24-102): ");
                std::string dept = UI::getStringInput("Enter Department (e.g., CSE, IT, ME): ");
                int sem = UI::getIntInput("Enter Current Semester (1-8): ", 1, 8);
                std::string phone = UI::getStringInput("Enter Mobile Number: ");

                auto student = std::make_shared<Student>("STU-" + roll, name, phone, roll, dept, sem, 450.0);
                student->displayProfile();
                UI::pause("Press Enter to enter SmartServe Student Ordering...");

                handleCustomerOrdering(student, canteenMenu, tokenManager, storage, orderIdCounter);
                break;
            }
            case 2: {
                // Faculty Login
                UI::printHeader("FACULTY & STAFF DINING LOGIN");
                std::string name = UI::getStringInput("Enter Faculty Name: ");
                std::string empId = UI::getStringInput("Enter Employee ID (e.g., FAC-882): ");
                std::string dept = UI::getStringInput("Enter Department / School: ");
                std::string phone = UI::getStringInput("Enter Mobile Number: ");

                auto faculty = std::make_shared<Faculty>("FAC-" + empId, name, phone, empId, dept, 800.0);
                faculty->displayProfile();
                UI::pause("Press Enter to enter SmartServe Faculty Ordering...");

                handleCustomerOrdering(faculty, canteenMenu, tokenManager, storage, orderIdCounter);
                break;
            }
            case 3: {
                // Admin portal with password
                UI::printHeader("ADMIN & KITCHEN ACCESS");
                std::string pass = UI::getPasswordInput("Enter Admin Security Passcode [default: admin123]: ");
                if (pass == "admin123" || pass == "admin") {
                    UI::printSuccess("Authentication successful! Welcome Canteen Manager.");
                    UI::pause();
                    handleAdminPortal(canteenMenu, tokenManager, storage);
                } else {
                    UI::printError("Invalid passcode! Access denied.");
                    UI::pause();
                }
                break;
            }
            case 4: {
                // Digital Queue Display
                UI::clearScreen();
                tokenManager.displayLiveQueueBoard();
                UI::pause("Press Enter to return to main menu...");
                break;
            }
            case 5: {
                // Token Tracker
                UI::printHeader("TRACK YOUR ORDER TOKEN");
                handleTokenLookup(tokenManager);
                UI::pause();
                break;
            }
            case 6: {
                // View Menu
                UI::clearScreen();
                UI::printHeader("SMARTSERVE CANTEEN MENU");
                canteenMenu.displayAll();
                UI::pause("Press Enter to continue...");
                break;
            }
            case 7: {
                // About
                UI::clearScreen();
                UI::printHeader("ABOUT SMARTSERVE CANTEEN SYSTEM");
                std::cout << "  SmartServe is an Object-Oriented C++ Canteen Automation System.\n"
                          << "  Key Features:\n"
                          << "   * Object-Oriented Architecture (Inheritance, Polymorphism, Encapsulation)\n"
                          << "   * Multi-role customer support (Students 5% off, Faculty 10% off)\n"
                          << "   * Categorized Food, Beverages, and Combo Meal packages\n"
                          << "   * Real-time automated Token Generation & Kitchen Queue Management\n"
                          << "   * Multiple Payment Options (Cash, UPI QR, SmartCard Wallet, Cards)\n"
                          << "   * Dynamic Wait Time Estimator & Stock Decrement Tracking\n"
                          << "   * Printable Tax Invoices & Order History Logging\n"
                          << "   * Persistent Menu Data Storage\n" << std::endl;
                UI::pause();
                break;
            }
            case 0: {
                // Save and exit
                storage.saveMenuToFile(canteenMenu);
                std::cout << "\n" << UI::GREEN << "Thank you for using SmartServe Canteen System. Goodbye!" << UI::RESET << std::endl;
                running = false;
                break;
            }
        }
    }

    return 0;
}

// ---------------------------------------------------------------------
// Customer Ordering Sub-Menu
// ---------------------------------------------------------------------
void handleCustomerOrdering(std::shared_ptr<User> currentUser, Menu& menu, 
                            TokenManager& tokenMgr, FileStorage& storage, int& orderIdCounter) {
    int nextTokenNum = tokenMgr.getActiveOrderCount() + tokenMgr.getCompletedOrderCount() + 101;
    auto currentOrder = std::make_shared<Order>(orderIdCounter++, nextTokenNum, currentUser);

    bool ordering = true;
    while (ordering) {
        UI::clearScreen();
        std::cout << UI::BOLD << UI::CYAN << ">>> " << currentUser->getRoleName() 
                  << " Portal | Logged In: " << currentUser->getName() 
                  << " | Wallet: " << CommonUtils::formatCurrency(currentUser->getWalletBalance())
                  << " <<<" << UI::RESET << "\n" << std::endl;

        std::cout << "  [1] Browse Full Menu" << std::endl;
        std::cout << "  [2] Browse Menu by Category" << std::endl;
        std::cout << "  [3] Filter by Dietary (Veg / Non-Veg / Vegan)" << std::endl;
        std::cout << "  [4] Search Food by Name" << std::endl;
        std::cout << "  [5] Add Item to Order Cart" << std::endl;
        std::cout << "  [6] View Order Cart & Modify Items (" << currentOrder->getItems().size() << " item(s))" << std::endl;
        std::cout << "  [7] Apply Promo Coupon" << std::endl;
        std::cout << "  [8] Proceed to Checkout & Pay" << std::endl;
        std::cout << "  [9] Live Kitchen Queue Board" << std::endl;
        std::cout << "  [0] Cancel / Return to Role Menu" << std::endl;
        UI::printDivider('-', 65);

        int choice = UI::getIntInput("  Select an action (0-9): ", 0, 9);

        switch (choice) {
            case 1: {
                UI::clearScreen();
                UI::printHeader("FULL CANTEEN MENU");
                menu.displayAll();
                UI::pause();
                break;
            }
            case 2: {
                handleViewMenuCategories(menu);
                break;
            }
            case 3: {
                UI::printHeader("FILTER BY DIETARY PREFERENCE");
                std::cout << "  [1] Vegetarian Items Only" << std::endl;
                std::cout << "  [2] Non-Vegetarian Items Only" << std::endl;
                std::cout << "  [3] Vegan Items Only" << std::endl;
                int dChoice = UI::getIntInput("  Choose filter (1-3): ", 1, 3);
                DietaryType dt = (dChoice == 1) ? DietaryType::VEG : 
                                 ((dChoice == 2) ? DietaryType::NON_VEG : DietaryType::VEGAN);
                auto filtered = menu.getByDietary(dt);
                menu.displaySearchResults(filtered, CommonUtils::dietaryToString(dt) + " Menu");
                UI::pause();
                break;
            }
            case 4: {
                UI::printHeader("SEARCH CANTEEN MENU");
                std::string q = UI::getStringInput("Enter food or beverage name to search: ");
                auto res = menu.searchByName(q);
                menu.displaySearchResults(res, "Search Results for '" + q + "'");
                UI::pause();
                break;
            }
            case 5: {
                handleAddItemToCart(currentOrder, menu);
                break;
            }
            case 6: {
                handleModifyCart(currentOrder);
                break;
            }
            case 7: {
                UI::printHeader("APPLY CANTEEN PROMO COUPON");
                std::cout << "  Available Offers:\n"
                          << "   * CAMPUS50  - Flat Rs. 50 OFF on orders above Rs. 200\n"
                          << "   * WELCOME10 - 10% Extra Discount on entire cart\n"
                          << "   * SNACK5    - Flat Rs. 5 OFF on quick snacks\n" << std::endl;
                std::string coupon = UI::getStringInput("Enter coupon code: ");
                if (currentOrder->applyCoupon(coupon)) {
                    UI::printSuccess("Coupon '" + coupon + "' applied successfully!");
                    std::cout << "  Discount saved: " << CommonUtils::formatCurrency(currentOrder->getCouponDiscount()) << std::endl;
                } else {
                    UI::printError("Invalid coupon code or cart value does not meet conditions!");
                }
                UI::pause();
                break;
            }
            case 8: {
                if (currentOrder->isEmpty()) {
                    UI::printError("Your cart is empty! Please add food items before checkout.");
                    UI::pause();
                } else {
                    bool completed = handleCheckout(currentOrder, menu, tokenMgr, storage);
                    if (completed) {
                        ordering = false; // exit back to role menu
                    }
                }
                break;
            }
            case 9: {
                UI::clearScreen();
                tokenMgr.displayLiveQueueBoard();
                UI::pause();
                break;
            }
            case 0: {
                if (!currentOrder->isEmpty()) {
                    std::string ans = UI::getStringInput("You have items in your cart. Discard and return? (y/n): ");
                    if (ans == "y" || ans == "Y") {
                        ordering = false;
                    }
                } else {
                    ordering = false;
                }
                break;
            }
        }
    }
}

void handleViewMenuCategories(const Menu& menu) {
    UI::printHeader("BROWSE BY CATEGORY");
    std::cout << "  [1] Breakfast Favorites" << std::endl;
    std::cout << "  [2] Main Course / Lunch Meals" << std::endl;
    std::cout << "  [3] Snacks & Quick Bites" << std::endl;
    std::cout << "  [4] Hot & Cold Beverages" << std::endl;
    std::cout << "  [5] Sweet Treats & Desserts" << std::endl;
    std::cout << "  [6] Special Student Combo Meals" << std::endl;
    int catChoice = UI::getIntInput("  Select Category (1-6): ", 1, 6);
    ItemCategory cat = static_cast<ItemCategory>(catChoice);
    menu.displayCategory(cat);
    UI::pause();
}

void handleAddItemToCart(std::shared_ptr<Order> currentOrder, Menu& menu) {
    UI::printHeader("ADD ITEM TO CART");
    int itemId = UI::getIntInput("Enter Item ID to order: ", 1, 9999);
    auto item = menu.getItemById(itemId);

    if (!item) {
        UI::printError("Item with ID " + std::to_string(itemId) + " not found!");
        UI::pause();
        return;
    }

    if (!item->isAvailable()) {
        UI::printError("Sorry! '" + item->getName() + "' is currently OUT OF STOCK.");
        UI::pause();
        return;
    }

    item->displayCard();

    int qty = UI::getIntInput("Enter quantity (1 to " + std::to_string(item->getStockQuantity()) + "): ", 1, item->getStockQuantity());
    std::string notes = UI::getStringInput("Any special preparation note? (e.g. Less spicy / Extra sauce) [Press Enter to skip]: ", true);

    try {
        currentOrder->addItem(item, qty, notes);
        UI::printSuccess("Added " + std::to_string(qty) + "x '" + item->getName() + "' to your cart!");
    } catch (const CanteenException& e) {
        UI::printError(e.what());
    }

    UI::pause();
}

void handleModifyCart(std::shared_ptr<Order> currentOrder) {
    UI::clearScreen();
    UI::printHeader("YOUR ORDER CART");
    currentOrder->displayCart();

    if (currentOrder->isEmpty()) {
        UI::pause();
        return;
    }

    std::cout << "\n  Cart Actions:" << std::endl;
    std::cout << "  [1] Modify Item Quantity" << std::endl;
    std::cout << "  [2] Remove an Item from Cart" << std::endl;
    std::cout << "  [3] Clear Entire Cart" << std::endl;
    std::cout << "  [0] Back to Ordering" << std::endl;

    int act = UI::getIntInput("  Select action (0-3): ", 0, 3);
    if (act == 1) {
        int id = UI::getIntInput("Enter Item ID to modify: ", 1, 9999);
        int newQty = UI::getIntInput("Enter new quantity (0 to remove): ", 0, 50);
        if (currentOrder->updateItemQuantity(id, newQty)) {
            UI::printSuccess("Cart updated successfully!");
        } else {
            UI::printError("Item not found in your cart.");
        }
        UI::pause();
    } else if (act == 2) {
        int id = UI::getIntInput("Enter Item ID to remove: ", 1, 9999);
        if (currentOrder->removeItem(id)) {
            UI::printSuccess("Item removed from cart!");
        } else {
            UI::printError("Item not found in your cart.");
        }
        UI::pause();
    } else if (act == 3) {
        std::string confirm = UI::getStringInput("Are you sure you want to clear your cart? (y/n): ");
        if (confirm == "y" || confirm == "Y") {
            currentOrder->clearCart();
            UI::printSuccess("Cart cleared.");
        }
        UI::pause();
    }
}

bool handleCheckout(std::shared_ptr<Order> currentOrder, Menu& menu, 
                    TokenManager& tokenMgr, FileStorage& storage) {
    UI::clearScreen();
    UI::printHeader("ORDER CHECKOUT & BILL SUMMARY");
    currentOrder->calculateBill();
    currentOrder->displayCart();

    double totalPayable = currentOrder->getGrandTotal();
    std::cout << "\n" << UI::BOLD << UI::YELLOW << "Amount Due: " 
              << CommonUtils::formatCurrency(totalPayable) << UI::RESET << std::endl;

    std::cout << "\n  Select Payment Method:" << std::endl;
    std::cout << "  [1] Cash at Canteen Counter" << std::endl;
    std::cout << "  [2] UPI Payment / QR Scan (Instant)" << std::endl;
    std::cout << "  [3] Campus SmartCard Wallet (Balance: " 
              << CommonUtils::formatCurrency(currentOrder->getCustomer()->getWalletBalance()) << ")" << std::endl;
    std::cout << "  [4] Debit / Credit Card" << std::endl;
    std::cout << "  [0] Cancel Checkout & Continue Adding Items" << std::endl;

    int payChoice = UI::getIntInput("  Enter payment option (0-4): ", 0, 4);
    if (payChoice == 0) return false;

    std::shared_ptr<Payment> paymentMethod = nullptr;

    switch (payChoice) {
        case 1: { // Cash
            auto cash = std::make_shared<CashPayment>();
            std::cout << "\n[Counter Cash Desk] Amount to tender: " << CommonUtils::formatCurrency(totalPayable) << std::endl;
            double cashTendered = UI::getDoubleInput("Enter cash handed to cashier (Rs): ", totalPayable, 5000.0);
            cash->setCashDetails(cashTendered);
            if (cash->processPayment(totalPayable)) {
                paymentMethod = cash;
                UI::printSuccess("Cash accepted. Change returned: " + CommonUtils::formatCurrency(cash->getChangeGiven()));
            }
            break;
        }
        case 2: { // UPI
            std::cout << "\n==============================================" << std::endl;
            std::cout << "   SMARTSERVE CANTEEN - INSTANT BHIM UPI QR   " << std::endl;
            std::cout << "==============================================" << std::endl;
            std::cout << "      [   +--+  +----+  +--+   ]             " << std::endl;
            std::cout << "      [   |  |  |    |  |  |   ]             " << std::endl;
            std::cout << "      [   +--+  +----+  +--+   ]             " << std::endl;
            std::cout << "      [   SCAN & PAY: " << CommonUtils::formatCurrency(totalPayable) << "   ]             " << std::endl;
            std::cout << "      [   UPI ID: smartserve@campus   ]             " << std::endl;
            std::cout << "==============================================" << std::endl;

            std::string upiId = UI::getStringInput("Enter your UPI VPA ID (e.g. yourname@oksbi): ");
            auto upi = std::make_shared<UPIPayment>(upiId);
            if (upi->processPayment(totalPayable)) {
                paymentMethod = upi;
                UI::printSuccess("UPI payment authorized! Reference: " + upi->getTransactionReference());
            }
            break;
        }
        case 3: { // Wallet
            auto wallet = std::make_shared<WalletPayment>(currentOrder->getCustomer());
            if (wallet->processPayment(totalPayable)) {
                paymentMethod = wallet;
                UI::printSuccess("Debited from Campus Wallet! New Balance: " + 
                                 CommonUtils::formatCurrency(currentOrder->getCustomer()->getWalletBalance()));
            } else {
                UI::printError("Insufficient Wallet Balance! Available: " + 
                               CommonUtils::formatCurrency(currentOrder->getCustomer()->getWalletBalance()) + 
                               ", Required: " + CommonUtils::formatCurrency(totalPayable));
                std::string addFund = UI::getStringInput("Would you like to top-up wallet now? (y/n): ");
                if (addFund == "y" || addFund == "Y") {
                    double topup = UI::getDoubleInput("Enter amount to add to wallet: ", 50.0, 5000.0);
                    currentOrder->getCustomer()->addWalletBalance(topup);
                    UI::printSuccess("Wallet topped up! New balance: " + CommonUtils::formatCurrency(currentOrder->getCustomer()->getWalletBalance()));
                }
                return false;
            }
            break;
        }
        case 4: { // Card
            std::string cardNum = UI::getStringInput("Enter 16-Digit Card Number: ");
            std::string holder = UI::getStringInput("Enter Cardholder Name: ");
            std::string expiry = UI::getStringInput("Enter Expiry (MM/YY): ");
            std::string cvv = UI::getStringInput("Enter 3-Digit CVV: ");
            (void)expiry; (void)cvv;

            auto card = std::make_shared<CardPayment>(cardNum, holder);
            if (card->processPayment(totalPayable)) {
                paymentMethod = card;
                UI::printSuccess("Card transaction approved!");
            }
            break;
        }
    }

    if (!paymentMethod || !paymentMethod->isSuccessful()) {
        UI::printError("Payment could not be completed. Order not placed.");
        UI::pause();
        return false;
    }

    // Assign payment
    currentOrder->setPayment(paymentMethod);
    currentOrder->setStatus(OrderStatus::PLACED);

    // Deduct stock in menu
    for (const auto& ordItm : currentOrder->getItems()) {
        auto menuItm = menu.getItemById(ordItm.getItem()->getId());
        if (menuItm) {
            try {
                menuItm->reduceStock(ordItm.getQuantity());
            } catch (const CanteenException& e) {
                // Warning only if race condition
                UI::printWarning(e.what());
            }
        }
    }

    // Register with token manager
    tokenMgr.registerOrder(currentOrder);

    // Persist receipt & order log
    storage.saveReceiptToFile(*currentOrder);
    storage.logOrderToHistory(*currentOrder);
    storage.saveMenuToFile(menu);

    // Display confirmation and printable receipt
    UI::clearScreen();
    UI::printSuccess("ORDER PLACED SUCCESSFULLY!");
    std::cout << "\n" << UI::BG_BLUE << UI::BOLD << UI::WHITE 
              << " >>> YOUR TOKEN NUMBER IS: [ TK-" << currentOrder->getTokenNumber() << " ] <<< " 
              << UI::RESET << "\n" << std::endl;

    currentOrder->displayReceipt();

    std::cout << UI::CYAN << "[i] A digital copy of your bill has been saved to: receipts/receipt_TK" 
              << currentOrder->getTokenNumber() << ".txt" << UI::RESET << std::endl;
    UI::pause("Press Enter to continue...");
    return true;
}

// ---------------------------------------------------------------------
// Admin & Kitchen Management Sub-Menu
// ---------------------------------------------------------------------
void handleAdminPortal(Menu& menu, TokenManager& tokenMgr, FileStorage& storage) {
    bool adminSession = true;
    while (adminSession) {
        UI::clearScreen();
        UI::printHeader("CANTEEN ADMIN & KITCHEN CONTROL PANEL");
        std::cout << "  Active Kitchen Orders:  " << tokenMgr.getActiveOrderCount() << std::endl;
        std::cout << "  Completed Orders Today: " << tokenMgr.getCompletedOrderCount() << std::endl;
        std::cout << "  Total Menu Catalog:     " << menu.size() << " items" << std::endl;
        UI::printDivider('-', 60);

        std::cout << "  [1] Live Kitchen Queue Management (Advance Token Status)" << std::endl;
        std::cout << "  [2] Menu Inventory Management (Add, Edit, Restock, Delete)" << std::endl;
        std::cout << "  [3] Daily Sales & Revenue Analytics" << std::endl;
        std::cout << "  [4] View All Active & Completed Tokens" << std::endl;
        std::cout << "  [5] Force Save Menu Data to Disk" << std::endl;
        std::cout << "  [0] Logout Admin" << std::endl;
        UI::printDivider('-', 60);

        int choice = UI::getIntInput("  Enter admin option (0-5): ", 0, 5);

        switch (choice) {
            case 1: {
                handleKitchenQueue(tokenMgr);
                break;
            }
            case 2: {
                handleMenuManagement(menu, storage);
                break;
            }
            case 3: {
                handleSalesReport(storage, tokenMgr);
                break;
            }
            case 4: {
                UI::clearScreen();
                tokenMgr.displayLiveQueueBoard();
                UI::pause();
                break;
            }
            case 5: {
                if (storage.saveMenuToFile(menu)) {
                    UI::printSuccess("Menu successfully persisted to 'data/menu.txt'.");
                } else {
                    UI::printError("Failed to save menu to file!");
                }
                UI::pause();
                break;
            }
            case 0: {
                adminSession = false;
                break;
            }
        }
    }
}

void handleKitchenQueue(TokenManager& tokenMgr) {
    UI::clearScreen();
    UI::printHeader("KITCHEN QUEUE DISPATCHER");
    tokenMgr.displayLiveQueueBoard();

    if (tokenMgr.getActiveOrderCount() == 0) {
        UI::printInfo("No orders currently waiting in the kitchen queue.");
        UI::pause();
        return;
    }

    std::cout << "\n  Kitchen Actions:" << std::endl;
    std::cout << "  [1] Advance Order Status (Placed -> Preparing -> Ready -> Completed)" << std::endl;
    std::cout << "  [2] Inspect Order Details by Token" << std::endl;
    std::cout << "  [0] Back to Admin Menu" << std::endl;

    int act = UI::getIntInput("  Choose action (0-2): ", 0, 2);
    if (act == 1) {
        int token = UI::getIntInput("Enter Token Number to advance (e.g. 101): ", 1, 9999);
        auto ord = tokenMgr.getOrderByToken(token);
        if (!ord) {
            UI::printError("Token not found!");
        } else {
            OrderStatus prev = ord->getStatus();
            if (tokenMgr.advanceStatus(token)) {
                OrderStatus curr = ord->getStatus();
                UI::printSuccess("Updated TK-" + std::to_string(token) + ": [" + 
                                 CommonUtils::orderStatusToString(prev) + "] ---> [" + 
                                 CommonUtils::orderStatusToString(curr) + "]");
            } else {
                UI::printWarning("Order is already completed or cannot be advanced.");
            }
        }
        UI::pause();
    } else if (act == 2) {
        int token = UI::getIntInput("Enter Token Number to inspect: ", 1, 9999);
        tokenMgr.displayCustomerStatus(token);
        UI::pause();
    }
}

void handleMenuManagement(Menu& menu, FileStorage& storage) {
    UI::clearScreen();
    UI::printHeader("MENU INVENTORY MANAGEMENT");
    std::cout << "  [1] View Full Menu List" << std::endl;
    std::cout << "  [2] Add New Food Item" << std::endl;
    std::cout << "  [3] Add New Beverage Item" << std::endl;
    std::cout << "  [4] Add New Combo Deal" << std::endl;
    std::cout << "  [5] Update Item Price" << std::endl;
    std::cout << "  [6] Restock / Add Inventory" << std::endl;
    std::cout << "  [7] Toggle Item Availability (Mark In/Out of Stock)" << std::endl;
    std::cout << "  [8] Delete Item from Menu" << std::endl;
    std::cout << "  [0] Back to Admin Panel" << std::endl;

    int opt = UI::getIntInput("  Enter option (0-8): ", 0, 8);

    switch (opt) {
        case 1: {
            menu.displayAll();
            UI::pause();
            break;
        }
        case 2: { // Add Food
            int newId = menu.getNextAvailableId();
            std::cout << "\nAssigning Item ID: " << newId << std::endl;
            std::string name = UI::getStringInput("Enter Food Item Name: ");
            double price = UI::getDoubleInput("Enter Price (Rs): ", 1.0, 2000.0);
            
            std::cout << "Category: [1] Breakfast [2] Main Course [3] Snacks [5] Desserts" << std::endl;
            int catInt = UI::getIntInput("Select Category: ", 1, 5);
            ItemCategory cat = static_cast<ItemCategory>(catInt);

            std::cout << "Dietary: [1] Veg [2] Non-Veg [3] Vegan" << std::endl;
            int dietInt = UI::getIntInput("Select Dietary Type: ", 1, 3);
            DietaryType diet = (dietInt == 1) ? DietaryType::VEG : ((dietInt == 2) ? DietaryType::NON_VEG : DietaryType::VEGAN);

            std::string spice = UI::getStringInput("Spice Level (Mild / Medium / Spicy): ");
            int prep = UI::getIntInput("Estimated Prep Time in Minutes: ", 1, 60);
            int stock = UI::getIntInput("Initial Stock Quantity: ", 1, 500);

            auto food = std::make_shared<FoodItem>(newId, name, price, cat, diet, spice, prep, stock);
            menu.addItem(food);
            storage.saveMenuToFile(menu);
            UI::printSuccess("Food item '" + name + "' added successfully!");
            UI::pause();
            break;
        }
        case 3: { // Add Beverage
            int newId = menu.getNextAvailableId();
            std::string name = UI::getStringInput("Enter Beverage Name: ");
            double price = UI::getDoubleInput("Enter Price (Rs): ", 1.0, 1000.0);
            std::string hotStr = UI::getStringInput("Is it Hot beverage? (y/n): ");
            bool isHot = (hotStr == "y" || hotStr == "Y");
            int vol = UI::getIntInput("Volume in ml (e.g. 200, 300): ", 50, 1000);
            std::string caffStr = UI::getStringInput("Contains Caffeine? (y/n): ");
            bool caffeine = (caffStr == "y" || caffStr == "Y");
            int stock = UI::getIntInput("Initial Stock Quantity: ", 1, 500);

            auto bev = std::make_shared<BeverageItem>(newId, name, price, ItemCategory::BEVERAGES, isHot, vol, caffeine, stock);
            menu.addItem(bev);
            storage.saveMenuToFile(menu);
            UI::printSuccess("Beverage '" + name + "' added successfully!");
            UI::pause();
            break;
        }
        case 4: { // Add Combo
            int newId = menu.getNextAvailableId();
            std::string name = UI::getStringInput("Enter Combo Name: ");
            double comboPrice = UI::getDoubleInput("Enter Special Combo Price (Rs): ", 1.0, 3000.0);
            double origPrice = UI::getDoubleInput("Enter Original Combined Price (Rs): ", comboPrice, 5000.0);
            int itemCount = UI::getIntInput("How many items in this combo meal (1-5)? ", 1, 5);
            std::vector<std::string> subItems;
            for (int i = 0; i < itemCount; ++i) {
                subItems.push_back(UI::getStringInput("  Enter included item #" + std::to_string(i + 1) + ": "));
            }
            int stock = UI::getIntInput("Available Combo Batches: ", 1, 100);

            auto combo = std::make_shared<ComboMeal>(newId, name, comboPrice, origPrice, subItems, stock);
            menu.addItem(combo);
            storage.saveMenuToFile(menu);
            UI::printSuccess("Combo Meal '" + name + "' created successfully!");
            UI::pause();
            break;
        }
        case 5: { // Update Price
            int id = UI::getIntInput("Enter Item ID to modify price: ", 1, 9999);
            auto item = menu.getItemById(id);
            if (!item) {
                UI::printError("Item not found!");
            } else {
                std::cout << "Current price of '" << item->getName() << "' is " << CommonUtils::formatCurrency(item->getPrice()) << std::endl;
                double newPrice = UI::getDoubleInput("Enter new price (Rs): ", 1.0, 5000.0);
                menu.updateItemPrice(id, newPrice);
                storage.saveMenuToFile(menu);
                UI::printSuccess("Price updated to " + CommonUtils::formatCurrency(newPrice));
            }
            UI::pause();
            break;
        }
        case 6: { // Restock
            int id = UI::getIntInput("Enter Item ID to restock: ", 1, 9999);
            auto item = menu.getItemById(id);
            if (!item) {
                UI::printError("Item not found!");
            } else {
                std::cout << "Current stock of '" << item->getName() << "': " << item->getStockQuantity() << std::endl;
                int addStock = UI::getIntInput("Enter units to add: ", 1, 500);
                menu.updateItemStock(id, addStock);
                storage.saveMenuToFile(menu);
                UI::printSuccess("Restocked! New quantity: " + std::to_string(item->getStockQuantity()));
            }
            UI::pause();
            break;
        }
        case 7: { // Toggle Availability
            int id = UI::getIntInput("Enter Item ID to toggle availability: ", 1, 9999);
            if (menu.toggleAvailability(id)) {
                auto item = menu.getItemById(id);
                UI::printSuccess("Item '" + item->getName() + "' is now " + (item->isAvailable() ? "AVAILABLE" : "UNAVAILABLE"));
                storage.saveMenuToFile(menu);
            } else {
                UI::printError("Item not found!");
            }
            UI::pause();
            break;
        }
        case 8: { // Delete Item
            int id = UI::getIntInput("Enter Item ID to permanently remove: ", 1, 9999);
            auto item = menu.getItemById(id);
            if (item) {
                std::string confirm = UI::getStringInput("Confirm deleting '" + item->getName() + "'? (y/n): ");
                if (confirm == "y" || confirm == "Y") {
                    menu.removeItem(id);
                    storage.saveMenuToFile(menu);
                    UI::printSuccess("Item removed from canteen catalog.");
                }
            } else {
                UI::printError("Item not found!");
            }
            UI::pause();
            break;
        }
        case 0:
            break;
    }
}

void handleSalesReport(const FileStorage& storage, const TokenManager& tokenMgr) {
    UI::clearScreen();
    UI::printHeader("CANTEEN SALES & ANALYTICS REPORT");

    auto records = storage.readOrderHistory();
    double totalRevenue = 0.0;
    int totalOrdersCount = static_cast<int>(records.size());
    int studentOrders = 0;
    int facultyOrders = 0;

    for (const auto& line : records) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> parts;
        while (std::getline(ss, token, ';')) {
            parts.push_back(token);
        }
        if (parts.size() >= 8) {
            try {
                double amt = std::stod(parts[5]);
                totalRevenue += amt;
                if (parts[4] == "Student") studentOrders++;
                else if (parts[4] == "Faculty / Staff") facultyOrders++;
            } catch (...) {}
        }
    }

    std::cout << "\n+-------------------------------------------------------------+" << std::endl;
    std::cout << "| Total Completed Orders in System Log: " << std::left << std::setw(21) << totalOrdersCount << " |" << std::endl;
    std::cout << "| Active Queue Orders Today:           " << std::left << std::setw(21) << tokenMgr.getActiveOrderCount() << " |" << std::endl;
    std::cout << "| Total Revenue Generated:             " << UI::GREEN << UI::BOLD << std::left << std::setw(21) << CommonUtils::formatCurrency(totalRevenue) << UI::RESET << " |" << std::endl;
    std::cout << "| Student Customer Orders:             " << std::left << std::setw(21) << studentOrders << " |" << std::endl;
    std::cout << "| Faculty / Staff Orders:              " << std::left << std::setw(21) << facultyOrders << " |" << std::endl;
    std::cout << "+-------------------------------------------------------------+" << std::endl;

    if (!records.empty()) {
        std::cout << "\nRecent Transactions Log (Last 5 orders):" << std::endl;
        std::cout << "+---------------------+-------+--------+------------------+------------+" << std::endl;
        std::cout << "| Timestamp           | Token | Role   | Customer         | Amount     |" << std::endl;
        std::cout << "+---------------------+-------+--------+------------------+------------+" << std::endl;

        int startIdx = std::max(0, static_cast<int>(records.size()) - 5);
        for (size_t i = startIdx; i < records.size(); ++i) {
            std::stringstream ss(records[i]);
            std::string token;
            std::vector<std::string> parts;
            while (std::getline(ss, token, ';')) parts.push_back(token);
            if (parts.size() >= 8) {
                std::cout << "| " << std::left << std::setw(19) << parts[0]
                          << " | TK-" << std::setw(4) << parts[2]
                          << " | " << std::left << std::setw(6) << parts[4].substr(0, 6)
                          << " | " << std::left << std::setw(16) << parts[3].substr(0, 16)
                          << " | " << std::right << std::setw(10) << parts[5]
                          << " |" << std::endl;
            }
        }
        std::cout << "+---------------------+-------+--------+------------------+------------+" << std::endl;
    }

    UI::pause();
}

void handleTokenLookup(const TokenManager& tokenMgr) {
    int token = UI::getIntInput("Enter your Token Number (e.g., 101): ", 1, 9999);
    tokenMgr.displayCustomerStatus(token);
}
