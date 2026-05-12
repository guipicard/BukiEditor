@echo off
setlocal EnableExtensions

set ROOT=%~dp0

echo.
echo ========================================
echo MAKE Game SHORTCUT
echo ========================================

if exist "%ROOT%Deployment\Game.exe" (
    powershell -NoProfile -ExecutionPolicy Bypass -Command "$s=(New-Object -ComObject WScript.Shell).CreateShortcut('%ROOT%Game.lnk'); $s.TargetPath='%ROOT%Deployment\Game.exe'; $s.WorkingDirectory='%ROOT%Deployment'; $s.Save()"
    echo Created Game.lnk
) else (
    echo Skipped Game.lnk, Game.exe not found.
)

echo.
echo Game Shortcut update complete.

exit /b %errorlevel%