#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>
#include <iostream>
#include <memory>
#include "User.h"

class Payment {
protected:
    double amountPaid;
    bool successful;
    std::string timestamp;

public:
    Payment();
    virtual ~Payment() = default;

    double getAmountPaid() const { return amountPaid; }
    bool isSuccessful() const { return successful; }
    std::string getTimestamp() const { return timestamp; }

    // Pure virtual methods (Polymorphism)
    virtual bool processPayment(double amount) = 0;
    virtual std::string getMethodName() const = 0;
    virtual std::string getTransactionReference() const = 0;
    virtual void printReceipt() const = 0;
};

class CashPayment : public Payment {
private:
    double amountTendered;
    double changeGiven;

public:
    CashPayment();
    void setCashDetails(double tendered);
    bool processPayment(double amount) override;
    std::string getMethodName() const override { return "Cash at Counter"; }
    std::string getTransactionReference() const override;
    void printReceipt() const override;
    double getChangeGiven() const { return changeGiven; }
};

class UPIPayment : public Payment {
private:
    std::string upiId;
    std::string txnId;

public:
    UPIPayment(const std::string& upi = "student@upi");
    bool processPayment(double amount) override;
    std::string getMethodName() const override { return "UPI / QR Payment"; }
    std::string getTransactionReference() const override { return txnId; }
    void printReceipt() const override;
    std::string getUpiId() const { return upiId; }
};

class CardPayment : public Payment {
private:
    std::string maskedCardNumber;
    std::string cardHolderName;
    std::string authCode;

public:
    CardPayment(const std::string& rawCardNum, const std::string& holder);
    bool processPayment(double amount) override;
    std::string getMethodName() const override { return "Debit/Credit Card"; }
    std::string getTransactionReference() const override { return authCode; }
    void printReceipt() const override;
};

class WalletPayment : public Payment {
private:
    std::shared_ptr<User> user;
    double prevBalance;
    double remainingBalance;
    std::string walletTxnId;

public:
    WalletPayment(std::shared_ptr<User> usr);
    bool processPayment(double amount) override;
    std::string getMethodName() const override { return "Canteen SmartCard Wallet"; }
    std::string getTransactionReference() const override { return walletTxnId; }
    void printReceipt() const override;
};

#endif // PAYMENT_H
