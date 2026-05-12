@echo off
setlocal EnableExtensions

set ROOT=%~dp0
cd /d "%ROOT%"

set KMAKE=tools\KoreTools_windows_x64\KoreTools_windows_x64\kmake.exe
set SOLUTION=build\BukiSolution.sln

echo.
echo ========================================
echo GENERATE
echo ========================================
echo ROOT = "%ROOT%"
echo KMAKE = "%KMAKE%"
echo SOLUTION = "%SOLUTION%"
echo.

if not exist "%ROOT%kfile.js" (
    echo ERROR: kfile.js not found at "%ROOT%kfile.js"
    exit /b 1
)

if not exist "%ROOT%%KMAKE%" (
    echo ERROR: kmake not found at "%ROOT%%KMAKE%"
    exit /b 1
)

"%ROOT%%KMAKE%"
if errorlevel 1 (
    echo ERROR: kmake generation failed.
    exit /b 1
)

if not exist "%ROOT%%SOLUTION%" (
    echo ERROR: Solution not found at "%ROOT%%SOLUTION%"
    exit /b 1
)

echo Generation complete.
call "%~dp0make-shortcut-Solution.bat"
pause

exit /b %errorlevel%