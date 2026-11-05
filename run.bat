@echo off
if not exist "bin\SmartServe.exe" (
    echo [i] Executable not found. Compiling first...
    call build.bat
)
if exist "bin\SmartServe.exe" (
    bin\SmartServe.exe
)
