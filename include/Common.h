#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

enum class DietaryType {
    VEG,
    NON_VEG,
    VEGAN
};

enum class ItemCategory {
    BREAKFAST = 1,
    MAIN_COURSE,
    SNACKS,
    BEVERAGES,
    DESSERTS,
    COMBOS
};

enum class OrderStatus {
    PLACED = 1,
    PREPARING,
    READY_FOR_PICKUP,
    COMPLETED,
    CANCELLED
};

enum class UserRole {
    STUDENT = 1,
    FACULTY,
    ADMIN
};

enum class PaymentType {
    CASH = 1,
    UPI,
    CARD,
    WALLET
};

namespace CommonUtils {
    inline std::string dietaryToString(DietaryType dt) {
        switch (dt) {
            case DietaryType::VEG:     return "Veg (Green)";
            case DietaryType::NON_VEG: return "Non-Veg (Red)";
            case DietaryType::VEGAN:   return "Vegan (Leaf)";
            default:                   return "Unknown";
        }
    }

    inline std::string categoryToString(ItemCategory cat) {
        switch (cat) {
            case ItemCategory::BREAKFAST:   return "Breakfast";
            case ItemCategory::MAIN_COURSE: return "Main Course";
            case ItemCategory::SNACKS:      return "Snacks";
            case ItemCategory::BEVERAGES:   return "Beverages";
            case ItemCategory::DESSERTS:    return "Desserts";
            case ItemCategory::COMBOS:      return "Combos";
            default:                        return "General";
        }
    }

    inline std::string orderStatusToString(OrderStatus status) {
        switch (status) {
            case OrderStatus::PLACED:           return "Order Placed";
            case OrderStatus::PREPARING:        return "In Kitchen / Preparing";
            case OrderStatus::READY_FOR_PICKUP: return "Ready for Pickup";
            case OrderStatus::COMPLETED:        return "Delivered / Completed";
            case OrderStatus::CANCELLED:        return "Cancelled";
            default:                            return "Unknown";
        }
    }

    inline std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        char buf[64];
        #if defined(_MSC_VER)
            struct tm timeinfo;
            localtime_s(&timeinfo, &now_time);
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
        #else
            struct tm* timeinfo = std::localtime(&now_time);
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
        #endif
        return std::string(buf);
    }

    inline std::string formatCurrency(double amount) {
        std::ostringstream oss;
        oss << "Rs. " << std::fixed << std::setprecision(2) << amount;
        return oss.str();
    }
}

#endif // COMMON_H
