@echo off
setlocal EnableExtensions

set ROOT=%~dp0

echo.
echo ========================================
echo MAKE SHORTCUTS
echo ========================================

call "%~dp0make-shortcut-Editor.bat"
call "%~dp0make-shortcut-Game.bat"
call "%~dp0make-shortcut-Solution.bat"

echo.
echo Shortcuts update complete.
pause
exit /b %errorlevel%