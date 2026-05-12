@echo off
setlocal EnableExtensions

call "%~dp0generate.bat"
if errorlevel 1 exit /b 1

call "%~dp0build-editor.bat"
if errorlevel 1 exit /b 1

call "%~dp0build-game.bat"
if errorlevel 1 exit /b 1

echo.
echo ========================================
echo All steps completed successfully.
echo ========================================
pause

if errorlevel 1 exit /b 1