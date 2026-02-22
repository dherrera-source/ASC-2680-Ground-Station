@echo off
set PORT1=9090
set PORT2=8080
set ADB="C:\School\SCRCPY\scrcpy-win64-v2.4\adb.exe"

echo Checking ADB connection...
adb devices | findstr "device" >nul
if %errorlevel% neq 0 (
    echo no authorized device found. Please unlock your phone and enabled usb debugging
    pause
    exit /b
)

echo Checking existing port forwards ...
adb forward --list | findstr "tcp:%PORT1%" >nul
if %errorlevel% neq 0 (
    echo Forward not found. Creating forward on port %PORT1%...
    adb forward tcp:%PORT1% tcp:%PORT2%
) else (
    echo Forward already active on port %PORT1%
)

echo Launching ground station...
python send_controls.py

pause