#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

// Base Exception for SmartServe Canteen System
class CanteenException : public std::runtime_error {
public:
    explicit CanteenException(const std::string& message) 
        : std::runtime_error(message) {}
};

class ItemNotFoundException : public CanteenException {
public:
    explicit ItemNotFoundException(int itemId)
        : CanteenException("Item with ID " + std::to_string(itemId) + " was not found in the menu.") {}
    explicit ItemNotFoundException(const std::string& name)
        : CanteenException("Item '" + name + "' was not found in the menu.") {}
};

class OutOfStockException : public CanteenException {
public:
    explicit OutOfStockException(const std::string& itemName, int requested, int available)
        : CanteenException("Insufficient stock for '" + itemName + "'. Requested: " + 
                           std::to_string(requested) + ", Available: " + std::to_string(available)) {}
};

class InvalidQuantityException : public CanteenException {
public:
    explicit InvalidQuantityException(int qty)
        : CanteenException("Invalid quantity specified: " + std::to_string(qty) + ". Quantity must be greater than 0.") {}
};

class PaymentFailedException : public CanteenException {
public:
    explicit PaymentFailedException(const std::string& reason)
        : CanteenException("Payment transaction failed: " + reason) {}
};

class InvalidInputException : public CanteenException {
public:
    explicit InvalidInputException(const std::string& field)
        : CanteenException("Invalid input provided for: " + field) {}
};

#endif // EXCEPTIONS_H
