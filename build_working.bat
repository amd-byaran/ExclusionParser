@echo off
echo Building ExclusionParser Working Test...

REM Set compiler options - focus on functionality
set CXX_FLAGS=/std:c++20 /EHsc /I"include" /W2 /O2 /D"EXCLUSION_PARSER_STATIC" /wd4251 /wd4273

REM Create output directory
if not exist "build" mkdir build

echo Compiling source files...

REM Compile source files to object files (only if not already compiled)
if not exist "build\ExclusionData.obj" (
    cl %CXX_FLAGS% /c src\ExclusionData.cpp /Fo:build\ExclusionData.obj
    if errorlevel 1 (
        echo Failed to compile ExclusionData.cpp
        exit /b 1
    )
)

if not exist "build\ExclusionParser.obj" (
    cl %CXX_FLAGS% /c src\ExclusionParser.cpp /Fo:build\ExclusionParser.obj
    if errorlevel 1 (
        echo Failed to compile ExclusionParser.cpp
        exit /b 1
    )
)

if not exist "build\ExclusionWriter.obj" (
    cl %CXX_FLAGS% /c src\ExclusionWriter.cpp /Fo:build\ExclusionWriter.obj
    if errorlevel 1 (
        echo Failed to compile ExclusionWriter.cpp
        exit /b 1
    )
)

echo Compiling working test...
cl %CXX_FLAGS% /c test\working_test.cpp /Fo:build\working_test.obj
if errorlevel 1 (
    echo Failed to compile working_test.cpp
    exit /b 1
)

echo Linking executable...
cl build\ExclusionData.obj build\ExclusionParser.obj build\ExclusionWriter.obj build\working_test.obj /Fe:build\working_test.exe
if errorlevel 1 (
    echo Failed to link executable
    exit /b 1
)

echo Build successful! Running tests...
echo.

REM Run the test
build\working_test.exe

echo.
echo Test execution completed.