@echo off
echo Building Header-Only ExclusionParser Validation Test...

REM Load Visual Studio environment if not already loaded
if not defined DevEnvDir (
    echo Loading Visual Studio environment...
    call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
)

echo Compiling header-only validation test...

REM Compile with minimal linking - just basic runtime
cl.exe /std:c++20 /EHsc /I"../include" ^
    /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS ^
    /Fe:build\header_only_test.exe ^
    test\header_only_test.cpp

if %ERRORLEVEL% neq 0 (
    echo Failed to compile header_only_test.cpp
    exit /b 1
)

echo.
echo Running header-only validation test...
echo.

build\header_only_test.exe

if %ERRORLEVEL% neq 0 (
    echo Header-only test execution failed
    exit /b 1
)

echo.
echo Header-only validation test completed successfully!