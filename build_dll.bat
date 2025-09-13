@echo off
echo Building ExclusionParser DLL and Library...

REM Load Visual Studio environment if not already loaded
if not defined DevEnvDir (
    echo Loading Visual Studio environment...
    call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
)

echo Creating build directory...
if not exist build mkdir build

echo Compiling ExclusionParser DLL...

REM Define the export symbol for DLL compilation
set EXPORT_FLAGS=/DEXCLUSION_PARSER_EXPORTS

REM Compile source files into object files
echo Compiling ExclusionData.cpp...
cl.exe /std:c++20 /EHsc /I"include" %EXPORT_FLAGS% ^
    /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS ^
    /c src\ExclusionData.cpp /Fo:build\ExclusionData.obj

if %ERRORLEVEL% neq 0 (
    echo Failed to compile ExclusionData.cpp
    exit /b 1
)

echo Compiling ExclusionParser.cpp...
cl.exe /std:c++20 /EHsc /I"include" %EXPORT_FLAGS% ^
    /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS ^
    /c src\ExclusionParser.cpp /Fo:build\ExclusionParser.obj

if %ERRORLEVEL% neq 0 (
    echo Failed to compile ExclusionParser.cpp
    exit /b 1
)

echo Compiling ExclusionWriter.cpp...
cl.exe /std:c++20 /EHsc /I"include" %EXPORT_FLAGS% ^
    /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS ^
    /c src\ExclusionWriter.cpp /Fo:build\ExclusionWriter.obj

if %ERRORLEVEL% neq 0 (
    echo Failed to compile ExclusionWriter.cpp
    exit /b 1
)

echo Linking DLL...
link.exe /DLL /OUT:build\ExclusionParser.dll ^
    /IMPLIB:build\ExclusionParser.lib ^
    build\ExclusionData.obj build\ExclusionParser.obj build\ExclusionWriter.obj

if %ERRORLEVEL% neq 0 (
    echo Failed to link DLL
    exit /b 1
)

echo.
echo Successfully created:
echo   build\ExclusionParser.dll
echo   build\ExclusionParser.lib
echo   build\ExclusionParser.exp

echo.
echo DLL and library compilation completed successfully!