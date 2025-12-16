@echo off
:: Chuyen mau sang Xanh la Matrix
color 0A
title Mummy System Override Protocol ^& Launcher
cls

:StartSequence

REM =========================================================
REM CONFIGURATION VECTORS
REM =========================================================
set "GAME_NAME=MummyGame.exe"

cls
echo.
echo  /$$      /$$                                                             /$$$$$$ 
echo ^| $$$    /$$$                                                            /$$__  $$
echo ^| $$$$  /$$$$ /$$   /$$ /$$$$$$/$$$$  /$$$$$$/$$$$  /$$   /$$           ^| $$  \__/
echo ^| $$ $$/$$ $$^| $$  ^| $$^| $$_  $$_  $$^| $$_  $$_  $$^| $$  ^| $$ /$$$$$$ ^| $$ /$$$$
echo ^| $$  $$$ ^| $$^| $$  ^| $$^| $$ \ $$ \ $$^| $$ \ $$ \ $$^| $$  ^| $$^|______/ ^| $$^|_  $$
echo ^| $$\  $ ^| $$^| $$  ^| $$^| $$ ^| $$ ^| $$^| $$ ^| $$ ^| $$^| $$  ^| $$           ^| $$  \ $$
echo ^| $$ \/  ^| $$^|  $$$$$$/^| $$ ^| $$ ^| $$^| $$ ^| $$ ^| $$^|  $$$$$$$           ^|  $$$$$$/
echo ^|__/     \__/ \______/ ^|__/ ^|__/ ^|__/^|__/ ^|__/ ^|__/ \____  $$            \______/ 
echo                                                      /$$  ^| $$                      
echo   ^>^> SYSTEM BREACH INITIATED...                     ^|  $$$$$$/                      
echo   ^>^> TARGET: %GAME_NAME%                      \______/                       
echo ===================================================================================
echo.

REM Neu bien ForceRebuild duoc bat, bo qua buoc kiem tra file cu
if defined FORCE_REBUILD goto PreBuildCleanup

REM =========================================================
REM PHASE 1: SCANNING HOST SYSTEM
REM =========================================================
echo [^>] Scanning local directories for existing payloads...

REM Check Visual Studio Release
if exist "build\Release\%GAME_NAME%" (
    timeout /t 1 >nul
    echo [!] Payload identified in sector: build\Release {VS variant}.
    echo [^>] Bypassing build phase.
    goto LaunchSequenceVS
)

REM Check MinGW Build
if exist "build\%GAME_NAME%" (
    timeout /t 1 >nul
    echo [!] Payload identified in sector: build {MinGW variant}.
    echo [^>] Bypassing build phase.
    goto LaunchSequenceMinGW
)

echo [X] No viability executable found on host.
echo.

:PreBuildCleanup
REM =========================================================
REM PHASE 2: COMPILATION PROTOCOL INITIATED
REM =========================================================
echo [^>] Initializing compilation sequence...
timeout /t 1 >nul

:: Neu dang o che do Rebuild thi xoa sach build cu
if defined FORCE_REBUILD (
    echo [-] FORCE REBUILD ENGAGED: Purging old cache data...
    if exist "build" rmdir /s /q "build"
    set "FORCE_REBUILD="
)

if not exist "build" (
    echo [+] Creating temporary build workspace...
    mkdir build
)

:TryVisualStudio
echo.
echo [^>] Attempting injection via primary vector: Visual Studio 2022...
echo -----------------------------------------------------------------
cmake -G "Visual Studio 17 2022" -S . -B build -Wno-dev
if %errorlevel% neq 0 (
    echo [!] Primary vector failed. Rerouting...
    goto TryMinGW
)

echo [^>] Compiling Release configuration payload...
cmake --build build --config Release -- /p:WarningLevel=0
if %errorlevel% neq 0 (
    echo [!] Compilation aborted. Rerouting...
    goto TryMinGW
)

if exist "build\Release\%GAME_NAME%" (
    goto LaunchSequenceVS
)

:TryMinGW
echo.
echo [!] Visual Studio unavailable or compromised.
echo [^>] Engaging fallback protocol: MinGW Makefiles...
echo -----------------------------------------------------------------

REM Clean old cache for fresh MinGW build
echo [-] Purging corrupted cache data...
if exist "build" rmdir /s /q "build"
mkdir build

echo [^>] Configuring MinGW generator...
cmake -G "MinGW Makefiles" -S . -B build -DCMAKE_BUILD_TYPE=Release -Wno-dev
if %errorlevel% neq 0 goto FinalError

echo [^>] Executing build command...
cmake --build build
if %errorlevel% neq 0 goto FinalError

if exist "build\%GAME_NAME%" (
    goto LaunchSequenceMinGW
)

REM =========================================================
REM PHASE 3: EXECUTION / ERROR
REM =========================================================

:LaunchSequenceVS
echo.
echo [SUCCESS] Payload generated successfully.
echo [^>] Executing target...
echo =========================================================
cd build\Release
"%GAME_NAME%"
cd ..\..
goto MissionDebrief

:LaunchSequenceMinGW
echo.
echo [SUCCESS] Payload generated successfully.
echo [^>] Executing target...
echo =========================================================
cd build
"%GAME_NAME%"
cd ..
goto MissionDebrief

:FinalError
color 0C
echo.
echo =========================================================
echo [FATAL ERROR] SYSTEM CRITICAL FAILURE
echo =========================================================
echo  ^>^> Compilation protocol aborted unexpectedly.
echo.
pause
exit

REM =========================================================
REM PHASE 4: MISSION DEBRIEF (MENU LUA CHON)
REM =========================================================
:MissionDebrief
color 0A
echo.
echo ===================================================================================
echo   ^>^> TARGET DISCONNECTED.
echo   ^>^> AWAITING FURTHER INSTRUCTIONS...
echo.
echo   [P] PLAY AGAIN - Relaunch existing payload (Instant).
echo   [B] REBUILD    - Delete cache and re-compile (Use after editing code).
echo   [E] EXIT       - Terminate session.
echo.
:: Lenh Choice de nhan phim tat P, B, E
choice /c PBE /n /m ">> SELECT COMMAND NODE [P/B/E]: "

:: Check errorlevel theo thu tu nguoc lai cua chuoi PBE (E=3, B=2, P=1)
if errorlevel 3 goto RealExit
if errorlevel 2 goto TriggerRebuild
if errorlevel 1 goto StartSequence

:TriggerRebuild
set "FORCE_REBUILD=1"
goto StartSequence

:RealExit
echo.
echo [^>] Session terminated. Traces cleared.
timeout /t 1 >nul
exit