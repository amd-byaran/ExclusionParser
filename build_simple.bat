@echo off
echo Building ExclusionParser Simple Test...

REM Set compiler options - focus on functionality, not warnings
set CXX_FLAGS=/std:c++20 /EHsc /I"include" /W2 /O2 /D"EXCLUSION_PARSER_STATIC" /wd4251 /wd4273

REM Create output directory
if not exist "build" mkdir build

echo Compiling source files...

REM Compile source files to object files
cl %CXX_FLAGS% /c src\ExclusionData.cpp /Fo:build\ExclusionData.obj
if errorlevel 1 (
    echo Failed to compile ExclusionData.cpp
    exit /b 1
)

cl %CXX_FLAGS% /c src\ExclusionParser.cpp /Fo:build\ExclusionParser.obj
if errorlevel 1 (
    echo Failed to compile ExclusionParser.cpp
    exit /b 1
)

cl %CXX_FLAGS% /c src\ExclusionWriter.cpp /Fo:build\ExclusionWriter.obj
if errorlevel 1 (
    echo Failed to compile ExclusionWriter.cpp
    exit /b 1
)

echo Compiling simple test...
cl %CXX_FLAGS% /c test\simple_test.cpp /Fo:build\simple_test.obj
if errorlevel 1 (
    echo Failed to compile simple_test.cpp
    exit /b 1
)

echo Linking executable...
cl build\ExclusionData.obj build\ExclusionParser.obj build\ExclusionWriter.obj build\simple_test.obj /Fe:build\simple_test.exe
if errorlevel 1 (
    echo Failed to link executable
    exit /b 1
)

echo Build successful! Running tests...
echo.

REM Run the test
build\simple_test.exe

echo.
echo Test execution completed.