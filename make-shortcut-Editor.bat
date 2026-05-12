@echo off
setlocal EnableExtensions

set ROOT=%~dp0

echo.
echo ========================================
echo MAKE Editor SHORTCUTS
echo ========================================

if exist "%ROOT%Deployment\Editor.exe" (
    powershell -NoProfile -ExecutionPolicy Bypass -Command "$s=(New-Object -ComObject WScript.Shell).CreateShortcut('%ROOT%Editor.lnk'); $s.TargetPath='%ROOT%Deployment\Editor.exe'; $s.WorkingDirectory='%ROOT%Deployment'; $s.Save()"
    echo Created Editor.lnk
) else (
    echo Skipped Editor.lnk, Editor.exe not found.
)

echo.
echo Editor Shortcut update complete.

exit /b %errorlevel%