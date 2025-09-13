@echo off
echo Building and Testing DLL Functionality...

REM Load Visual Studio environment if not already loaded
if not defined DevEnvDir (
    echo Loading Visual Studio environment...
    call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
)

echo Compiling DLL functionality test...

REM Compile the test against the DLL
cl.exe /std:c++20 /EHsc /I"include" ^
    /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS ^
    /Fe:build\dll_functionality_test.exe ^
    test\dll_functionality_test.cpp ^
    build\ExclusionParser.lib

if %ERRORLEVEL% neq 0 (
    echo Failed to compile dll_functionality_test.cpp
    exit /b 1
)

echo.
echo Running DLL functionality test...
echo.

REM Ensure the DLL is in the same directory as the executable for runtime loading
copy build\ExclusionParser.dll build\ >nul 2>&1

build\dll_functionality_test.exe

if %ERRORLEVEL% neq 0 (
    echo DLL functionality test execution failed
    exit /b 1
)

echo.
echo DLL functionality test completed successfully!