@echo off
REM Build script for Dynamic DLL Unit Tests
REM Compiles the new DLL-based testing framework
REM Author: AMD Verification Team
REM Date: September 13, 2025
REM Version: 2.0.0

echo ===============================================
echo   Building Dynamic DLL Unit Tests v2.0.0
echo ===============================================
echo AMD Verification Team - September 13, 2025
echo.

REM Setup environment
call "C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if errorlevel 1 (
    echo ERROR: Failed to setup Visual Studio environment
    echo Please ensure Visual Studio 2022 Preview is installed
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build

echo Building Dynamic DLL Test Framework...
echo.

REM Compile DLL Test Framework
echo [1/3] Compiling DLLTestFramework.cpp...
cl /EHsc /std:c++20 /I"include" /c test/DLLTestFramework.cpp /Fo"build/DLLTestFramework.obj"
if errorlevel 1 (
    echo ERROR: Failed to compile DLLTestFramework.cpp
    pause
    exit /b 1
)

REM Compile Dynamic DLL Unit Tests
echo [2/3] Compiling dynamic_dll_unit_tests.cpp...
cl /EHsc /std:c++20 /I"include" /I"test" /c test/dynamic_dll_unit_tests.cpp /Fo"build/dynamic_dll_unit_tests.obj"
if errorlevel 1 (
    echo ERROR: Failed to compile dynamic_dll_unit_tests.cpp
    pause
    exit /b 1
)

REM Link the executable
echo [3/3] Linking dynamic_dll_tests.exe...
link build/DLLTestFramework.obj build/dynamic_dll_unit_tests.obj /OUT:build/dynamic_dll_tests.exe
if errorlevel 1 (
    echo ERROR: Failed to link dynamic_dll_tests.exe
    pause
    exit /b 1
)

echo.
echo ===============================================
echo   Dynamic DLL Tests Build Successful!
echo ===============================================
echo.
echo Output files:
echo   build/dynamic_dll_tests.exe - Main test executable
echo   build/DLLTestFramework.obj  - Framework object
echo   build/dynamic_dll_unit_tests.obj - Test object
echo.
echo Usage:
echo   build\dynamic_dll_tests.exe --all         (Run all tests)
echo   build\dynamic_dll_tests.exe --basic       (Basic DLL tests)
echo   build\dynamic_dll_tests.exe --performance (Performance tests)
echo   build\dynamic_dll_tests.exe --integrity   (DLL validation)
echo   build\dynamic_dll_tests.exe --help        (Show help)
echo.

REM Check if DLL exists
if exist "build\ExclusionParser.dll" (
    echo ✅ ExclusionParser.dll found - Ready for testing!
    echo.
    echo Running basic integrity test...
    build\dynamic_dll_tests.exe --integrity
    if errorlevel 1 (
        echo ⚠️  DLL integrity test failed
    ) else (
        echo ✅ DLL integrity verified!
    )
) else (
    echo ⚠️  ExclusionParser.dll not found in build directory
    echo    Please run build_dll.bat first to create the DLL
)

echo.
echo Build complete! You can now run dynamic DLL tests.
pause