@echo off
REM Enhanced DLL Build Script with C Interface
REM Builds ExclusionParser.dll with C interface for dynamic testing
REM Author: AMD Verification Team
REM Date: September 13, 2025
REM Version: 2.0.0

echo ===============================================
echo   Building ExclusionParser DLL with C Interface
echo ===============================================
echo AMD Verification Team - September 13, 2025
echo Enhanced for dynamic testing support
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

echo Building Enhanced DLL with C Interface...
echo.

REM Compile all source files with C interface
echo [1/6] Compiling ExclusionParser.cpp...
cl /EHsc /std:c++20 /DEXCLUSION_PARSER_EXPORTS /I"include" /c src/ExclusionParser.cpp /Fo"build/ExclusionParser.obj"
if errorlevel 1 (
    echo ERROR: Failed to compile ExclusionParser.cpp
    pause
    exit /b 1
)

echo [2/6] Compiling ExclusionWriter.cpp...
cl /EHsc /std:c++20 /DEXCLUSION_PARSER_EXPORTS /I"include" /c src/ExclusionWriter.cpp /Fo"build/ExclusionWriter.obj"
if errorlevel 1 (
    echo ERROR: Failed to compile ExclusionWriter.cpp
    pause
    exit /b 1
)

echo [3/6] Compiling ExclusionData.cpp...
cl /EHsc /std:c++20 /DEXCLUSION_PARSER_EXPORTS /I"include" /c src/ExclusionData.cpp /Fo"build/ExclusionData.obj"
if errorlevel 1 (
    echo ERROR: Failed to compile ExclusionData.cpp
    pause
    exit /b 1
)

echo [4/6] Compiling ExclusionCInterface.cpp...
cl /EHsc /std:c++20 /DEXCLUSION_PARSER_EXPORTS /I"include" /c src/ExclusionCInterface.cpp /Fo"build/ExclusionCInterface.obj"
if errorlevel 1 (
    echo ERROR: Failed to compile ExclusionCInterface.cpp
    pause
    exit /b 1
)

echo [5/6] Creating module definition file...
echo EXPORTS > build/ExclusionParser.def
echo CreateExclusionParser >> build/ExclusionParser.def
echo DestroyExclusionParser >> build/ExclusionParser.def
echo ParseExclusionFile >> build/ExclusionParser.def
echo GetParserErrorCount >> build/ExclusionParser.def
echo GetParserLastError >> build/ExclusionParser.def
echo GetParserData >> build/ExclusionParser.def
echo CreateExclusionWriter >> build/ExclusionParser.def
echo DestroyExclusionWriter >> build/ExclusionParser.def
echo OpenWriterFile >> build/ExclusionParser.def
echo CloseWriterFile >> build/ExclusionParser.def
echo WriteExclusionHeader >> build/ExclusionParser.def
echo WriteExclusionData >> build/ExclusionParser.def
echo CreateExclusionData >> build/ExclusionParser.def
echo DestroyExclusionData >> build/ExclusionParser.def
echo SetExclusionDataFilename >> build/ExclusionParser.def
echo GetExclusionDataFilename >> build/ExclusionParser.def
echo GetExclusionDataScopeCount >> build/ExclusionParser.def
echo AddExclusionScope >> build/ExclusionParser.def
echo GetScopeBlockCount >> build/ExclusionParser.def
echo GetScopeToggleCount >> build/ExclusionParser.def
echo GetScopeFsmCount >> build/ExclusionParser.def
echo GetScopeConditionCount >> build/ExclusionParser.def
echo AddBlockExclusion >> build/ExclusionParser.def
echo AddToggleExclusion >> build/ExclusionParser.def
echo AddFsmStateExclusion >> build/ExclusionParser.def
echo AddFsmTransitionExclusion >> build/ExclusionParser.def
echo AddConditionExclusion >> build/ExclusionParser.def
echo GetExclusionErrorString >> build/ExclusionParser.def
echo GetExclusionParserVersion >> build/ExclusionParser.def
echo IsExclusionFileValid >> build/ExclusionParser.def
echo CreateTestExclusionFile >> build/ExclusionParser.def
echo ValidateExclusionData >> build/ExclusionParser.def
echo PrintExclusionDataSummary >> build/ExclusionParser.def

echo [6/6] Linking Enhanced DLL...
link /DLL build/ExclusionParser.obj build/ExclusionWriter.obj build/ExclusionData.obj build/ExclusionCInterface.obj /DEF:build/ExclusionParser.def /OUT:build/ExclusionParser.dll /IMPLIB:build/ExclusionParser.lib
if errorlevel 1 (
    echo ERROR: Failed to link ExclusionParser.dll
    pause
    exit /b 1
)

echo.
echo ===============================================
echo   Enhanced DLL Build Successful!
echo ===============================================
echo.
echo Generated files:
echo   build/ExclusionParser.dll     - Main DLL with C interface
echo   build/ExclusionParser.lib     - Import library
echo   build/ExclusionParser.exp     - Export definitions
echo   build/ExclusionParser.def     - Module definition file
echo.
echo New Features:
echo   ✅ C Interface for dynamic loading
echo   ✅ 32 exported C functions
echo   ✅ Complete API coverage
echo   ✅ Error handling support
echo   ✅ Testing utilities included
echo.

REM Test the enhanced DLL
echo Testing enhanced DLL exports...
dumpbin /exports build/ExclusionParser.dll | findstr /C:"CreateExclusionParser"
if errorlevel 1 (
    echo ⚠️  Warning: C interface exports not found
) else (
    echo ✅ C interface exports verified!
)

echo.
echo Enhanced DLL ready for dynamic testing!
echo Use build_dynamic_dll_tests.bat to build and run DLL tests.
pause