#ifndef ORDER_H
#define ORDER_H

#include "OrderItem.h"
#include "User.h"
#include "Payment.h"
#include "Common.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>

class Order {
private:
    int orderId;
    int tokenNumber;
    std::shared_ptr<User> customer;
    std::vector<OrderItem> items;
    OrderStatus status;
    std::string timestamp;
    std::shared_ptr<Payment> payment;

    // Financial breakdown
    double subtotal;
    double userDiscountAmount;
    double couponDiscountAmount;
    std::string couponCode;
    double taxAmount; // 5% GST
    double grandTotal;
    int estimatedPrepMinutes;

public:
    Order(int orderId, int tokenNumber, std::shared_ptr<User> customer);

    // Getters
    int getOrderId() const { return orderId; }
    int getTokenNumber() const { return tokenNumber; }
    std::shared_ptr<User> getCustomer() const { return customer; }
    const std::vector<OrderItem>& getItems() const { return items; }
    OrderStatus getStatus() const { return status; }
    std::string getTimestamp() const { return timestamp; }
    std::shared_ptr<Payment> getPayment() const { return payment; }

    double getSubtotal() const { return subtotal; }
    double getUserDiscount() const { return userDiscountAmount; }
    double getCouponDiscount() const { return couponDiscountAmount; }
    std::string getCouponCode() const { return couponCode; }
    double getTaxAmount() const { return taxAmount; }
    double getGrandTotal() const { return grandTotal; }
    int getEstimatedPrepMinutes() const { return estimatedPrepMinutes; }

    // Mutators
    void setStatus(OrderStatus newStatus) { status = newStatus; }
    void setPayment(std::shared_ptr<Payment> pmt) { payment = pmt; }

    // Cart operations
    void addItem(std::shared_ptr<MenuItem> item, int quantity, const std::string& instructions = "");
    bool removeItem(int itemId);
    bool updateItemQuantity(int itemId, int newQuantity);
    void clearCart();
    bool isEmpty() const { return items.empty(); }

    // Calculation & coupon
    void calculateBill();
    bool applyCoupon(const std::string& code);

    // Displays
    void displayCart() const;
    void displayReceipt() const;
    std::string generateReceiptString() const;

    // Operator Overloading
    friend std::ostream& operator<<(std::ostream& os, const Order& order);
};

#endif // ORDER_H
