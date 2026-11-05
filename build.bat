@echo off
echo =========================================================
echo    Compiling SmartServe - Canteen Ordering System (C++)
echo =========================================================

REM Detect Compiler
set GXX=g++
if exist "C:\msys64\mingw64\bin\g++.exe" (
    set GXX=C:\msys64\mingw64\bin\g++.exe
)

echo Using compiler: %GXX%

if not exist "bin" mkdir bin
if not exist "data" mkdir data
if not exist "receipts" mkdir receipts

%GXX% -std=c++17 -Wall -Wextra -Iinclude src/MenuItem.cpp src/FoodItem.cpp src/BeverageItem.cpp src/ComboMeal.cpp src/Menu.cpp src/OrderItem.cpp src/Order.cpp src/Payment.cpp src/User.cpp src/TokenManager.cpp src/FileStorage.cpp src/UIHelper.cpp src/main.cpp -o bin/SmartServe.exe

if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] SmartServe compiled successfully!
    echo Executable generated at: bin\SmartServe.exe
    echo.
    echo Run the application using: run.bat or .\bin\SmartServe.exe
) else (
    echo.
    echo [ERROR] Compilation failed! Please inspect error messages above.
)
