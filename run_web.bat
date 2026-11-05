@echo off
echo ========================================================
echo    Launching SmartServe Web Server on Localhost (8080)
echo ========================================================

REM Check if Python is available
where python >nul 2>nul
if %ERRORLEVEL% equ 0 (
    set PY_CMD=python
) else (
    set PY_CMD=py
)

echo Starting web server at http://localhost:8080...
echo Opening browser...
start http://localhost:8080

%PY_CMD% server.py 8080
pause
