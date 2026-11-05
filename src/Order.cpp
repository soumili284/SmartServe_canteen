#include "Order.h"
#include "UIHelper.h"
#include <iomanip>
#include <sstream>
#include <cmath>

Order::Order(int orderId, int tokenNumber, std::shared_ptr<User> customer)
    : orderId(orderId), tokenNumber(tokenNumber), customer(customer),
      status(OrderStatus::PLACED), subtotal(0.0), userDiscountAmount(0.0),
      couponDiscountAmount(0.0), couponCode(""), taxAmount(0.0), grandTotal(0.0),
      estimatedPrepMinutes(10) {
    timestamp = CommonUtils::getCurrentTimestamp();
}

void Order::addItem(std::shared_ptr<MenuItem> item, int quantity, const std::string& instructions) {
    if (!item || quantity <= 0) return;

    for (auto& orderItem : items) {
        if (orderItem.getItem()->getId() == item->getId()) {
            orderItem.addQuantity(quantity);
            if (!instructions.empty()) {
                orderItem.setInstructions(instructions);
            }
            calculateBill();
            return;
        }
    }
    items.emplace_back(item, quantity, instructions);
    calculateBill();
}

bool Order::removeItem(int itemId) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->getItem()->getId() == itemId) {
            items.erase(it);
            calculateBill();
            return true;
        }
    }
    return false;
}

bool Order::updateItemQuantity(int itemId, int newQuantity) {
    if (newQuantity <= 0) {
        return removeItem(itemId);
    }
    for (auto& itm : items) {
        if (itm.getItem()->getId() == itemId) {
            itm.setQuantity(newQuantity);
            calculateBill();
            return true;
        }
    }
    return false;
}

void Order::clearCart() {
    items.clear();
    subtotal = 0.0;
    userDiscountAmount = 0.0;
    couponDiscountAmount = 0.0;
    taxAmount = 0.0;
    grandTotal = 0.0;
}

void Order::calculateBill() {
    subtotal = 0.0;
    int totalItemCount = 0;

    for (const auto& itm : items) {
        subtotal += itm.getSubtotal();
        totalItemCount += itm.getQuantity();
    }

    // Role-based discount
    double rate = customer ? customer->getDiscountRate() : 0.0;
    userDiscountAmount = subtotal * rate;

    // Apply coupon if any
    double discountedSubtotal = subtotal - userDiscountAmount;
    if (couponCode == "CAMPUS50") {
        couponDiscountAmount = (discountedSubtotal >= 200.0) ? 50.0 : 0.0;
    } else if (couponCode == "WELCOME10") {
        couponDiscountAmount = discountedSubtotal * 0.10;
    } else if (couponCode == "SNACK5") {
        couponDiscountAmount = (discountedSubtotal >= 50.0) ? 5.0 : 0.0;
    } else {
        couponDiscountAmount = 0.0;
    }

    if (couponDiscountAmount > discountedSubtotal) {
        couponDiscountAmount = discountedSubtotal;
    }

    double taxableAmount = discountedSubtotal - couponDiscountAmount;
    if (taxableAmount < 0.0) taxableAmount = 0.0;

    taxAmount = taxableAmount * 0.05; // 5% GST
    grandTotal = taxableAmount + taxAmount;

    // Estimated preparation time calculation
    estimatedPrepMinutes = 5 + (totalItemCount * 2);
    if (estimatedPrepMinutes > 35) estimatedPrepMinutes = 35;
}

bool Order::applyCoupon(const std::string& code) {
    if (code == "CAMPUS50" || code == "WELCOME10" || code == "SNACK5") {
        couponCode = code;
        calculateBill();
        return (couponDiscountAmount > 0.0);
    }
    return false;
}

void Order::displayCart() const {
    if (items.empty()) {
        std::cout << "\n[!] Your order cart is empty." << std::endl;
        return;
    }

    std::cout << "\n+-----+------------------------------+-------+------------+------------+" << std::endl;
    std::cout << "| " << UI::BOLD << "#   "
              << "| " << std::left << std::setw(28) << "Item Description"
              << "| " << std::right << std::setw(5) << "Qty "
              << "| " << std::right << std::setw(10) << "Price   "
              << "| " << std::right << std::setw(10) << "Total   "
              << UI::RESET << " |" << std::endl;
    std::cout << "+-----+------------------------------+-------+------------+------------+" << std::endl;

    int idx = 1;
    for (const auto& itm : items) {
        itm.displayRow(idx++);
    }

    std::cout << "+-----+------------------------------+-------+------------+------------+" << std::endl;
    std::cout << "  Subtotal:            " << std::right << std::setw(12) << CommonUtils::formatCurrency(subtotal) << std::endl;
    if (userDiscountAmount > 0.0) {
        std::cout << "  Role Privilege (" << (customer ? customer->getRoleName() : "User") << "): -" 
                  << std::right << std::setw(12) << CommonUtils::formatCurrency(userDiscountAmount) << std::endl;
    }
    if (couponDiscountAmount > 0.0) {
        std::cout << "  Coupon Discount [" << couponCode << "]: -" 
                  << std::right << std::setw(12) << CommonUtils::formatCurrency(couponDiscountAmount) << std::endl;
    }
    std::cout << "  Canteen GST (5%):    " << std::right << std::setw(12) << CommonUtils::formatCurrency(taxAmount) << std::endl;
    std::cout << "  " << UI::BOLD << UI::GREEN << "Estimated Total:     " << std::right << std::setw(12) << CommonUtils::formatCurrency(grandTotal) << UI::RESET << std::endl;
    std::cout << "  Est. Prep Time:      " << estimatedPrepMinutes << " minutes" << std::endl;
}

std::string Order::generateReceiptString() const {
    std::ostringstream oss;
    oss << "=================================================================\n";
    oss << "              SMARTSERVE COLLEGE CANTEEN SYSTEM                 \n";
    oss << "                   TAX INVOICE & ORDER TOKEN                     \n";
    oss << "=================================================================\n";
    oss << " Order ID:       #" << orderId << "\n";
    oss << " Token Number:   >>> [ TK-" << tokenNumber << " ] <<<\n";
    oss << " Order Status:   " << CommonUtils::orderStatusToString(status) << "\n";
    oss << " Date & Time:    " << timestamp << "\n";
    oss << " Customer:       " << (customer ? customer->getName() : "Guest") 
        << " (" << (customer ? customer->getRoleName() : "Student") << ")\n";
    oss << " Contact No:     " << (customer ? customer->getContactNumber() : "N/A") << "\n";
    oss << "-----------------------------------------------------------------\n";
    oss << " Qty | Description                     | Unit Price | Total Price\n";
    oss << "-----------------------------------------------------------------\n";

    for (const auto& itm : items) {
        std::string name = itm.getItem() ? itm.getItem()->getName() : "Item";
        oss << " " << std::right << std::setw(3) << itm.getQuantity() << " | "
            << std::left << std::setw(31) << name.substr(0, 31) << " | "
            << std::right << std::setw(10) << std::fixed << std::setprecision(2) << (itm.getItem() ? itm.getItem()->getPrice() : 0.0) << " | "
            << std::right << std::setw(10) << std::fixed << std::setprecision(2) << itm.getSubtotal() << "\n";
        if (!itm.getInstructions().empty()) {
            oss << "     Note: \"" << itm.getInstructions() << "\"\n";
        }
    }

    oss << "-----------------------------------------------------------------\n";
    oss << " Subtotal:                                      " << std::right << std::setw(16) << CommonUtils::formatCurrency(subtotal) << "\n";
    if (userDiscountAmount > 0.0) {
        oss << " Privilege Discount (" << (customer ? customer->getRoleName() : "User") << "):       -" << std::right << std::setw(16) << CommonUtils::formatCurrency(userDiscountAmount) << "\n";
    }
    if (couponDiscountAmount > 0.0) {
        oss << " Promo Code (" << couponCode << "):                  -" << std::right << std::setw(16) << CommonUtils::formatCurrency(couponDiscountAmount) << "\n";
    }
    oss << " Canteen GST (5%):                              " << std::right << std::setw(16) << CommonUtils::formatCurrency(taxAmount) << "\n";
    oss << "-----------------------------------------------------------------\n";
    oss << " FINAL AMOUNT PAYABLE:                          " << std::right << std::setw(16) << CommonUtils::formatCurrency(grandTotal) << "\n";
    oss << "=================================================================\n";

    if (payment) {
        oss << " Payment Method: " << payment->getMethodName() << "\n";
        oss << " Transaction ID: " << payment->getTransactionReference() << "\n";
        oss << " Payment Status: " << (payment->isSuccessful() ? "PAID & VERIFIED" : "PENDING") << "\n";
    }

    oss << " Est. Pickup Time: ~" << estimatedPrepMinutes << " minutes from order placement\n";
    oss << " Please watch the Live Digital Token Board for 'TK-" << tokenNumber << "'!\n";
    oss << "=================================================================\n";
    oss << "        Thank you for choosing SmartServe Canteen System!        \n";
    oss << "=================================================================\n";

    return oss.str();
}

void Order::displayReceipt() const {
    std::cout << generateReceiptString() << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << order.generateReceiptString();
    return os;
}
