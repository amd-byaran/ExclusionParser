@echo off
echo Building Comprehensive ExclusionParser Validation Test...

REM Load Visual Studio environment if not already loaded
if not defined DevEnvDir (
    echo Loading Visual Studio environment...
    call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
)

echo Compiling comprehensive validation test...

REM Compile comprehensive test with basic runtime linking
cl.exe /std:c++20 /EHsc /I"../include" ^
    /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS ^
    /Fe:build\comprehensive_validation_test.exe ^
    test\comprehensive_validation_test.cpp

if %ERRORLEVEL% neq 0 (
    echo Failed to compile comprehensive_validation_test.cpp
    exit /b 1
)

echo.
echo Running comprehensive validation test...
echo.

build\comprehensive_validation_test.exe

if %ERRORLEVEL% neq 0 (
    echo Comprehensive validation test execution failed
    exit /b 1
)

echo.
echo Comprehensive validation test completed successfully!