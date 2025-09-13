@echo off
echo Building Minimal ExclusionParser Test...

REM Set compiler options for header-only testing
set CXX_FLAGS=/std:c++20 /EHsc /I"include" /W2 /O2

echo Compiling minimal test (header-only)...
cl %CXX_FLAGS% test\minimal_test.cpp /Fe:build\minimal_test.exe
if errorlevel 1 (
    echo Failed to compile minimal_test.cpp
    exit /b 1
)

echo Build successful! Running minimal tests...
echo.

REM Run the test
build\minimal_test.exe

echo.
echo Minimal test execution completed.