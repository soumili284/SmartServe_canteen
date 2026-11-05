#include "UIHelper.h"
#include <iostream>
#include <sstream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

namespace UI {

void initConsole() {
#if defined(_WIN32) || defined(_WIN64)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    // Avoid jarring clears if running in non-standard terminals, but provide standard cls
    std::cout << "\033[2J\033[H";
#else
    std::cout << "\033[2J\033[H";
#endif
}

void printBanner() {
    std::cout << CYAN << BOLD;
    std::cout << R"(
  ███████╗███╗   ███╗ █████╗ ██████╗ ████████╗███████╗███████╗██████╗ ██╗   ██╗███████╗
  ██╔════╝████╗ ████║██╔══██╗██╔══██╗╚══██╔══╝██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝
  ███████╗██╔████╔██║███████║██████╔╝   ██║   ███████╗█████╗  ██████╔╝██║   ██║█████╗  
  ╚════██║██║╚██╔╝██║██╔══██║██╔══██╗   ██║   ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  
  ███████║██║ ╚═╝ ██║██║  ██║██║  ██║   ██║   ███████║███████╗██║  ██║ ╚████╔╝ ███████╗
  ╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝
    )" << RESET << std::endl;
    std::cout << YELLOW << BOLD << "       >>> Smart College Canteen Ordering & Digital Token System <<<" << RESET << std::endl;
    std::cout << WHITE << "            Fast * Contactless * Organized * Simple Student Dining\n" << RESET << std::endl;
}

void printHeader(const std::string& title) {
    int width = 76;
    int pad = (width - 4 - static_cast<int>(title.length())) / 2;
    if (pad < 0) pad = 0;

    std::cout << "\n" << BLUE << std::string(width, '=') << RESET << std::endl;
    std::cout << BLUE << "| " << BOLD << WHITE << std::string(pad, ' ') << title 
              << std::string(width - 4 - pad - static_cast<int>(title.length()), ' ') << BLUE << " |" << RESET << std::endl;
    std::cout << BLUE << std::string(width, '=') << RESET << std::endl;
}

void printDivider(char ch, int length) {
    std::cout << CYAN << std::string(length, ch) << RESET << std::endl;
}

void printSubDivider(char ch, int length) {
    std::cout << std::string(length, ch) << std::endl;
}

void printSuccess(const std::string& msg) {
    std::cout << GREEN << BOLD << "[SUCCESS] " << RESET << GREEN << msg << RESET << std::endl;
}

void printError(const std::string& msg) {
    std::cout << RED << BOLD << "[ERROR] " << RESET << RED << msg << RESET << std::endl;
}

void printWarning(const std::string& msg) {
    std::cout << YELLOW << BOLD << "[ATTENTION] " << RESET << YELLOW << msg << RESET << std::endl;
}

void printInfo(const std::string& msg) {
    std::cout << CYAN << "[i] " << msg << RESET << std::endl;
}

int getIntInput(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value >= minVal && value <= maxVal) {
                // Clear any leftover newline in stream
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            } else {
                std::cout << RED << "Please enter a value between " << minVal << " and " << maxVal << "." << RESET << std::endl;
            }
        } else {
            if (std::cin.eof()) {
                return 0; // Safe fallback on stream EOF
            }
            std::cout << RED << "Invalid input. Please enter a valid number." << RESET << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

double getDoubleInput(const std::string& prompt, double minVal, double maxVal) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value >= minVal && value <= maxVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            } else {
                std::cout << RED << "Please enter a value between " << minVal << " and " << maxVal << "." << RESET << std::endl;
            }
        } else {
            if (std::cin.eof()) {
                return minVal;
            }
            std::cout << RED << "Invalid numeric input. Please try again." << RESET << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string getStringInput(const std::string& prompt, bool allowEmpty) {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) {
            if (std::cin.eof()) {
                return "";
            }
        }
        // Trim leading and trailing spaces
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            line = "";
        } else {
            size_t last = line.find_last_not_of(" \t\r\n");
            line = line.substr(first, (last - first + 1));
        }

        if (allowEmpty || !line.empty() || std::cin.eof()) {
            return line;
        }
        std::cout << RED << "Input cannot be empty. Please enter a value." << RESET << std::endl;
    }
}

std::string getPasswordInput(const std::string& prompt) {
    std::cout << prompt;
    std::string pass;
    if (!std::getline(std::cin, pass)) {
        return "";
    }
    return pass;
}

void pause(const std::string& prompt) {
    if (std::cin.eof()) return;
    std::cout << "\n" << CYAN << prompt << RESET;
    std::string temp;
    std::getline(std::cin, temp);
}

} // namespace UI
