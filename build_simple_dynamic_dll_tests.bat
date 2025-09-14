@echo off
REM Build script for Simple Dynamic DLL Unit Tests
REM Compiles the simplified DLL-based testing framework
REM Author: AMD Verification Team
REM Date: September 13, 2025
REM Version: 2.0.0

echo ===============================================
echo   Building Simple Dynamic DLL Tests v2.0.0
echo ===============================================
echo AMD Verification Team - September 13, 2025
echo.

REM Setup environment
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if errorlevel 1 (
    echo ERROR: Failed to setup Visual Studio environment
    echo Please ensure Visual Studio is installed
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build

echo Building Simple Dynamic DLL Tests...
echo.

REM Compile Simple Dynamic DLL Tests
echo [1/1] Compiling simple_dynamic_dll_tests.cpp...
cl /EHsc /std:c++20 /I"include" test/simple_dynamic_dll_tests.cpp /Fe"build/simple_dynamic_dll_tests.exe"
if errorlevel 1 (
    echo ERROR: Failed to compile simple_dynamic_dll_tests.cpp
    pause
    exit /b 1
)

echo.
echo ===============================================
echo   Simple Dynamic DLL Tests Build Successful!
echo ===============================================
echo.
echo Output files:
echo   build/simple_dynamic_dll_tests.exe - Simple DLL test executable
echo.
echo Usage:
echo   build\simple_dynamic_dll_tests.exe
echo.

REM Check if DLL exists and run tests
if exist "build\ExclusionParser.dll" (
    echo ✅ ExclusionParser.dll found - Running dynamic tests!
    echo.
    echo Running simple dynamic DLL tests...
    build\simple_dynamic_dll_tests.exe
    echo.
    echo Dynamic DLL testing completed!
) else (
    echo ⚠️  ExclusionParser.dll not found in build directory
    echo    Please run build_dll.bat first to create the DLL
)

echo.
echo Build complete!
pause