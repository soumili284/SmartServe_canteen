@echo off
set "PATH=C:\msys64\usr\bin;%PATH%"
echo =========================================================
echo    Pushing SmartServe to GitHub
echo    Repo: https://github.com/soumili284/SmartServe_canteen
echo =========================================================
echo.
echo If prompted:
echo   Username: soumili284
echo   Password: [Enter your GitHub Personal Access Token]
echo.
echo To generate a token: https://github.com/settings/tokens
echo Check the 'repo' scope checkbox.
echo =========================================================
echo.

git push -u origin main

if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] Pushed successfully to https://github.com/soumili284/SmartServe_canteen!
) else (
    echo.
    echo [NOTICE] If authentication failed, use a GitHub Personal Access Token as your password.
)
echo.
pause
