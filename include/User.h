#ifndef USER_H
#define USER_H

#include "Common.h"
#include <string>
#include <iostream>
#include <iomanip>

class User {
protected:
    std::string userId;
    std::string name;
    std::string contactNumber;
    UserRole role;
    double walletBalance;

public:
    User(const std::string& id, const std::string& name, const std::string& contact, 
         UserRole role, double initialWallet = 500.0);
    virtual ~User() = default;

    // Getters
    std::string getUserId() const { return userId; }
    std::string getName() const { return name; }
    std::string getContactNumber() const { return contactNumber; }
    UserRole getRole() const { return role; }
    double getWalletBalance() const { return walletBalance; }

    // Wallet operations
    bool deductWallet(double amount);
    void addWalletBalance(double amount);

    // Pure virtual methods (Polymorphism)
    virtual double getDiscountRate() const = 0;
    virtual void displayProfile() const = 0;
    virtual std::string getRoleName() const = 0;
};

class Student : public User {
private:
    std::string rollNumber;
    std::string department;
    int semester;

public:
    Student(const std::string& id, const std::string& name, const std::string& contact,
            const std::string& roll, const std::string& dept, int sem, double wallet = 350.0);

    std::string getRollNumber() const { return rollNumber; }
    std::string getDepartment() const { return department; }
    int getSemester() const { return semester; }

    double getDiscountRate() const override { return 0.05; } // 5% Student Discount
    std::string getRoleName() const override { return "Student"; }
    void displayProfile() const override;
};

class Faculty : public User {
private:
    std::string employeeId;
    std::string department;

public:
    Faculty(const std::string& id, const std::string& name, const std::string& contact,
            const std::string& empId, const std::string& dept, double wallet = 750.0);

    std::string getEmployeeId() const { return employeeId; }
    std::string getDepartment() const { return department; }

    double getDiscountRate() const override { return 0.10; } // 10% Faculty Discount
    std::string getRoleName() const override { return "Faculty / Staff"; }
    void displayProfile() const override;
};

class Admin : public User {
private:
    std::string designation;

public:
    Admin(const std::string& id, const std::string& name, const std::string& contact,
          const std::string& desig = "Canteen Head Manager");

    double getDiscountRate() const override { return 0.15; } // 15% Staff Discount
    std::string getRoleName() const override { return "Administrator"; }
    void displayProfile() const override;
};

#endif // USER_H
