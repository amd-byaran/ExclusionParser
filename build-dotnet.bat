@echo off
rem Build script for ExclusionParser .NET Assembly
rem This script builds the managed .NET assembly using C++/CLI

echo Building ExclusionParser .NET Assembly...

rem Set up Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2026\Insider\VC\Auxiliary\Build\vcvars64.bat"

rem Build the .NET assembly
echo Building managed C++/CLI assembly...
msbuild ExclusionParser.NET.csproj /p:Configuration=Release /p:Platform=x64 /v:minimal

if %ERRORLEVEL% NEQ 0 (
    echo Error: .NET assembly build failed
    pause
    exit /b 1
)

echo .NET assembly build completed successfully!

rem Create dist directory if it doesn't exist
if not exist "dist" mkdir dist

rem Copy output files
if exist "bin\Release\ExclusionParser.NET.dll" (
    copy "bin\Release\ExclusionParser.NET.dll" "dist\"
    echo Copied ExclusionParser.NET.dll to dist\
)

if exist "bin\Release\ExclusionParser.NET.pdb" (
    copy "bin\Release\ExclusionParser.NET.pdb" "dist\"
    echo Copied ExclusionParser.NET.pdb to dist\
)

if exist "bin\ExclusionParser.dll" (
    copy "bin\ExclusionParser.dll" "dist\"
    echo Copied native ExclusionParser.dll to dist\
)

echo.
echo Build Summary:
if exist "dist\ExclusionParser.NET.dll" (
    echo ✓ Managed assembly: dist\ExclusionParser.NET.dll
) else (
    echo ✗ Managed assembly not found
)

if exist "dist\ExclusionParser.dll" (
    echo ✓ Native library: dist\ExclusionParser.dll
) else (
    echo ✗ Native library not found
)

echo.
echo ExclusionParser .NET Assembly build completed!
pause