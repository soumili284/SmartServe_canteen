#ifndef UI_HELPER_H
#define UI_HELPER_H

#include <string>
#include <iostream>
#include <limits>

namespace UI {
    // ANSI Colors
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";

    // Backgrounds
    const std::string BG_BLUE   = "\033[44m";
    const std::string BG_GREEN  = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";

    void initConsole();
    void clearScreen();
    void printBanner();
    void printHeader(const std::string& title);
    void printDivider(char ch = '=', int length = 75);
    void printSubDivider(char ch = '-', int length = 75);
    void printSuccess(const std::string& msg);
    void printError(const std::string& msg);
    void printWarning(const std::string& msg);
    void printInfo(const std::string& msg);

    int getIntInput(const std::string& prompt, int minVal, int maxVal);
    double getDoubleInput(const std::string& prompt, double minVal, double maxVal);
    std::string getStringInput(const std::string& prompt, bool allowEmpty = false);
    std::string getPasswordInput(const std::string& prompt);
    void pause(const std::string& prompt = "Press Enter to continue...");
}

#endif // UI_HELPER_H
