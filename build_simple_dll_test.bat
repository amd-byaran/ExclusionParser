@echo off
echo Building and Testing Simple DLL Functionality...

REM Load Visual Studio environment if not already loaded
if not defined DevEnvDir (
    echo Loading Visual Studio environment...
    call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
)

echo Compiling simple DLL functionality test...

REM Compile the test against the DLL
cl.exe /std:c++20 /EHsc /I"include" ^
    /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS ^
    /Fe:build\simple_dll_test.exe ^
    test\simple_dll_test.cpp ^
    build\ExclusionParser.lib

if %ERRORLEVEL% neq 0 (
    echo Failed to compile simple_dll_test.cpp
    exit /b 1
)

echo.
echo Running simple DLL functionality test...
echo.

build\simple_dll_test.exe

if %ERRORLEVEL% neq 0 (
    echo Simple DLL functionality test execution failed
    exit /b 1
)

echo.
echo Simple DLL functionality test completed successfully!