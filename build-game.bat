@echo off
setlocal EnableExtensions

set ROOT=%~dp0
cd /d "%ROOT%"

set SOLUTION=build\BukiSolution.sln
set CONFIG=Debug
set PLATFORM=x64
set TARGET_NAME=Game
set TARGET_EXE=Game.exe
set BUILD_DIR=build\%PLATFORM%\%CONFIG%
set DEPLOY_DIR=Deployment

set VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe
set MSBUILD=

echo.
echo ========================================
echo BUILD GAME
echo ========================================

echo find solution
if not exist "%ROOT%%SOLUTION%" (
    echo ERROR: Solution not found at "%ROOT%%SOLUTION%"
    pause
    exit /b 1
)
echo find vswhere
if not exist "%VSWHERE%" (
    echo ERROR: vswhere not found at "%VSWHERE%"
    pause
    exit /b 1
)

echo setting msbuild
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
    set MSBUILD=%%i
)
echo is msbuild defined
if not defined MSBUILD (
    echo ERROR: Could not find MSBuild with vswhere.
    pause
    exit /b 1
)

echo MSBUILD = "%MSBUILD%"
echo Building %TARGET_NAME%...

"%MSBUILD%" "%ROOT%%SOLUTION%" /t:%TARGET_NAME% /p:Configuration=%CONFIG%;Platform=%PLATFORM% /v:m

echo setting msbuild
if errorlevel 1 (
    echo ERROR: Build failed for %TARGET_NAME%.
    pause
    exit /b 1
)

if not exist "%ROOT%%DEPLOY_DIR%" (
    mkdir "%ROOT%%DEPLOY_DIR%"
)
echo copying builld
if exist "%ROOT%%BUILD_DIR%\%TARGET_EXE%" (
    copy /Y "%ROOT%%BUILD_DIR%\%TARGET_EXE%" "%ROOT%%DEPLOY_DIR%\%TARGET_EXE%" >nul
    echo Copied %TARGET_EXE% to Deployment.
) else (
    echo WARNING: %TARGET_EXE% not found at "%ROOT%%BUILD_DIR%\%TARGET_EXE%"
)

echo Game build complete.

call "%~dp0make-shortcut-Game.bat"
echo Editor shortcut complete.
pause
exit /b %errorlevel%