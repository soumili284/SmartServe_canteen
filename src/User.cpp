#include "User.h"
#include "UIHelper.h"
#include <iostream>
#include <iomanip>

User::User(const std::string& id, const std::string& name, const std::string& contact,
           UserRole role, double initialWallet)
    : userId(id), name(name), contactNumber(contact), role(role), walletBalance(initialWallet) {}

bool User::deductWallet(double amount) {
    if (amount <= 0.0) return false;
    if (walletBalance >= amount) {
        walletBalance -= amount;
        return true;
    }
    return false;
}

void User::addWalletBalance(double amount) {
    if (amount > 0.0) {
        walletBalance += amount;
    }
}

// Student Implementation
Student::Student(const std::string& id, const std::string& name, const std::string& contact,
                 const std::string& roll, const std::string& dept, int sem, double wallet)
    : User(id, name, contact, UserRole::STUDENT, wallet),
      rollNumber(roll), department(dept), semester(sem) {}

void Student::displayProfile() const {
    std::cout << "\n+------------------ STUDENT PROFILE ------------------+" << std::endl;
    std::cout << "| Name:         " << std::left << std::setw(37) << name << " |" << std::endl;
    std::cout << "| Roll Number:  " << std::left << std::setw(37) << rollNumber << " |" << std::endl;
    std::cout << "| Department:   " << std::left << std::setw(37) << (department + " (Sem " + std::to_string(semester) + ")") << " |" << std::endl;
    std::cout << "| Contact:      " << std::left << std::setw(37) << contactNumber << " |" << std::endl;
    std::cout << "| Discount:     " << std::left << std::setw(37) << "5% Campus Student Privilege" << " |" << std::endl;
    std::cout << "| Wallet Bal:   " << UI::GREEN << std::left << std::setw(37) << CommonUtils::formatCurrency(walletBalance) << UI::RESET << " |" << std::endl;
    std::cout << "+-----------------------------------------------------+" << std::endl;
}

// Faculty Implementation
Faculty::Faculty(const std::string& id, const std::string& name, const std::string& contact,
                 const std::string& empId, const std::string& dept, double wallet)
    : User(id, name, contact, UserRole::FACULTY, wallet),
      employeeId(empId), department(dept) {}

void Faculty::displayProfile() const {
    std::cout << "\n+------------------ FACULTY PROFILE ------------------+" << std::endl;
    std::cout << "| Name:         " << std::left << std::setw(37) << name << " |" << std::endl;
    std::cout << "| Employee ID:  " << std::left << std::setw(37) << employeeId << " |" << std::endl;
    std::cout << "| Department:   " << std::left << std::setw(37) << department << " |" << std::endl;
    std::cout << "| Contact:      " << std::left << std::setw(37) << contactNumber << " |" << std::endl;
    std::cout << "| Discount:     " << std::left << std::setw(37) << "10% Staff Privilege" << " |" << std::endl;
    std::cout << "| Wallet Bal:   " << UI::GREEN << std::left << std::setw(37) << CommonUtils::formatCurrency(walletBalance) << UI::RESET << " |" << std::endl;
    std::cout << "+-----------------------------------------------------+" << std::endl;
}

// Admin Implementation
Admin::Admin(const std::string& id, const std::string& name, const std::string& contact,
             const std::string& desig)
    : User(id, name, contact, UserRole::ADMIN, 5000.0), designation(desig) {}

void Admin::displayProfile() const {
    std::cout << "\n+---------------- ADMIN / STAFF PORTAL ---------------+" << std::endl;
    std::cout << "| Name:         " << std::left << std::setw(37) << name << " |" << std::endl;
    std::cout << "| Designation:  " << std::left << std::setw(37) << designation << " |" << std::endl;
    std::cout << "| Access Level: " << std::left << std::setw(37) << "Full Administrative Authority" << " |" << std::endl;
    std::cout << "+-----------------------------------------------------+" << std::endl;
}
