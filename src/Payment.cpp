#include "Payment.h"
#include "UIHelper.h"
#include <iomanip>
#include <cstdlib>
#include <random>

Payment::Payment() : amountPaid(0.0), successful(false) {
    timestamp = CommonUtils::getCurrentTimestamp();
}

// Cash Payment
CashPayment::CashPayment() : Payment(), amountTendered(0.0), changeGiven(0.0) {}

void CashPayment::setCashDetails(double tendered) {
    amountTendered = tendered;
}

bool CashPayment::processPayment(double amount) {
    amountPaid = amount;
    if (amountTendered >= amountPaid) {
        changeGiven = amountTendered - amountPaid;
        successful = true;
        timestamp = CommonUtils::getCurrentTimestamp();
        return true;
    }
    successful = false;
    return false;
}

std::string CashPayment::getTransactionReference() const {
    return "CASH-REG-" + std::to_string(static_cast<int>(amountPaid * 100));
}

void CashPayment::printReceipt() const {
    std::cout << "Payment Mode:   " << getMethodName() << std::endl;
    std::cout << "Amount Tendered:" << CommonUtils::formatCurrency(amountTendered) << std::endl;
    std::cout << "Change Returned:" << CommonUtils::formatCurrency(changeGiven) << std::endl;
}

// UPI Payment
UPIPayment::UPIPayment(const std::string& upi) 
    : Payment(), upiId(upi) {
    // Generate mock reference
    int randNum = 100000 + (std::rand() % 900000);
    txnId = "UPI" + std::to_string(randNum) + "REF";
}

bool UPIPayment::processPayment(double amount) {
    amountPaid = amount;
    // Simulate instant UPI transaction verification
    successful = true;
    timestamp = CommonUtils::getCurrentTimestamp();
    return true;
}

void UPIPayment::printReceipt() const {
    std::cout << "Payment Mode:   " << getMethodName() << std::endl;
    std::cout << "UPI VPA ID:     " << upiId << std::endl;
    std::cout << "Transaction ID: " << txnId << std::endl;
}

// Card Payment
CardPayment::CardPayment(const std::string& rawCardNum, const std::string& holder)
    : Payment(), cardHolderName(holder) {
    if (rawCardNum.length() >= 4) {
        maskedCardNumber = "**** **** **** " + rawCardNum.substr(rawCardNum.length() - 4);
    } else {
        maskedCardNumber = "**** **** **** 1234";
    }
    int randAuth = 1000 + (std::rand() % 9000);
    authCode = "AUTH" + std::to_string(randAuth);
}

bool CardPayment::processPayment(double amount) {
    amountPaid = amount;
    successful = true;
    timestamp = CommonUtils::getCurrentTimestamp();
    return true;
}

void CardPayment::printReceipt() const {
    std::cout << "Payment Mode:   " << getMethodName() << std::endl;
    std::cout << "Card Number:    " << maskedCardNumber << std::endl;
    std::cout << "Card Holder:    " << cardHolderName << std::endl;
    std::cout << "Auth Code:      " << authCode << std::endl;
}

// Wallet Payment
WalletPayment::WalletPayment(std::shared_ptr<User> usr) 
    : Payment(), user(usr), prevBalance(0.0), remainingBalance(0.0) {
    if (user) {
        prevBalance = user->getWalletBalance();
    }
    int randNum = 10000 + (std::rand() % 90000);
    walletTxnId = "WLT-" + std::to_string(randNum);
}

bool WalletPayment::processPayment(double amount) {
    amountPaid = amount;
    if (!user) {
        successful = false;
        return false;
    }

    if (user->deductWallet(amount)) {
        remainingBalance = user->getWalletBalance();
        successful = true;
        timestamp = CommonUtils::getCurrentTimestamp();
        return true;
    }

    successful = false;
    return false;
}

void WalletPayment::printReceipt() const {
    std::cout << "Payment Mode:   " << getMethodName() << std::endl;
    std::cout << "Wallet Txn ID:  " << walletTxnId << std::endl;
    std::cout << "Prev Balance:   " << CommonUtils::formatCurrency(prevBalance) << std::endl;
    std::cout << "Debited:        " << CommonUtils::formatCurrency(amountPaid) << std::endl;
    std::cout << "Remaining Bal:  " << CommonUtils::formatCurrency(remainingBalance) << std::endl;
}
