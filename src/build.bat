@echo off
setlocal

set "BASE_DIR=%~dp0.."
set "BUILD_DIR=%BASE_DIR%\build"

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

echo Compiling USBImager CLI...
gcc -DUSE_PHY -o "%BUILD_DIR%\usbimager-cli.exe" iso_burner.c disks_win.c stream_minimal.c -lsetupapi -lole32 -luser32 -lkernel32

if %errorlevel% equ 0 (
    echo Build successful: %BUILD_DIR%\usbimager-cli.exe
) else (
    echo Build failed!
    exit /b %errorlevel%
)
