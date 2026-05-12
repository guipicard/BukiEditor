@echo off
setlocal EnableExtensions

set ROOT=%~dp0

echo.
echo ========================================
echo MAKE Solution SHORTCUT
echo ========================================

if exist "%ROOT%build\BukiSolution.sln" (
    powershell -NoProfile -ExecutionPolicy Bypass -Command "$s=(New-Object -ComObject WScript.Shell).CreateShortcut('%ROOT%BukiSolution.lnk'); $s.TargetPath='%ROOT%build\BukiSolution.sln'; $s.WorkingDirectory='%ROOT%build'; $s.Save()"
    echo Created BukiSolution.lnk
) else (
    echo Skipped BukiSolution.lnk, solution not found.
)

echo.
echo Solution Shortcut update complete.

exit /b %errorlevel%