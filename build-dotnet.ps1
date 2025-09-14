# Build script for ExclusionParser .NET Assembly
# This script builds the managed .NET assembly from the native C++ library
# using C++/CLI wrappers for seamless .NET interoperability

param(
    [Parameter(Mandatory=$false)]
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",
    
    [Parameter(Mandatory=$false)]
    [ValidateSet("x64")]
    [string]$Platform = "x64",
    
    [Parameter(Mandatory=$false)]
    [switch]$Clean,
    
    [Parameter(Mandatory=$false)]
    [switch]$Test,
    
    [Parameter(Mandatory=$false)]
    [switch]$Package,
    
    [Parameter(Mandatory=$false)]
    [switch]$Verbose
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Get script directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$RootDir = Split-Path -Parent $ScriptDir

Write-Host "=== ExclusionParser .NET Assembly Build Script ===" -ForegroundColor Green
Write-Host "Configuration: $Configuration" -ForegroundColor Yellow
Write-Host "Platform: $Platform" -ForegroundColor Yellow
Write-Host "Root Directory: $RootDir" -ForegroundColor Yellow
Write-Host ""

# Function to execute command with error handling
function Execute-Command {
    param(
        [string]$Command,
        [string]$Description,
        [string]$WorkingDirectory = $RootDir
    )
    
    Write-Host "Executing: $Description" -ForegroundColor Cyan
    Write-Host "Command: $Command" -ForegroundColor Gray
    
    try {
        $process = Start-Process -FilePath "cmd.exe" -ArgumentList "/c", $Command -WorkingDirectory $WorkingDirectory -Wait -PassThru -NoNewWindow
        if ($process.ExitCode -ne 0) {
            throw "Command failed with exit code $($process.ExitCode)"
        }
        Write-Host "✓ $Description completed successfully" -ForegroundColor Green
        Write-Host ""
    }
    catch {
        Write-Host "✗ $Description failed: $($_.Exception.Message)" -ForegroundColor Red
        exit 1
    }
}

# Function to check prerequisites
function Check-Prerequisites {
    Write-Host "Checking Prerequisites" -ForegroundColor Cyan
    
    # Check for .NET SDK
    try {
        $dotnetVersion = dotnet --version
        Write-Host "✓ .NET SDK found: $dotnetVersion" -ForegroundColor Green
    }
    catch {
        Write-Host "✗ .NET SDK not found. Please install .NET 8.0 SDK or later." -ForegroundColor Red
        exit 1
    }
    
    # Check for MSBuild (through Visual Studio)
    $msbuildPath = Get-Command "msbuild.exe" -ErrorAction SilentlyContinue
    if (-not $msbuildPath) {
        Write-Host "✗ MSBuild not found. Please install Visual Studio 2022 or Build Tools." -ForegroundColor Red
        exit 1
    }
    Write-Host "✓ MSBuild found: $($msbuildPath.Source)" -ForegroundColor Green
    
    # Check for native library
    $nativeLib = Join-Path $RootDir "lib\ExclusionParser.lib"
    $nativeDll = Join-Path $RootDir "bin\ExclusionParser.dll"
    
    if (-not (Test-Path $nativeLib)) {
        Write-Host "✗ Native library not found: $nativeLib" -ForegroundColor Red
        Write-Host "Please build the native C++ library first." -ForegroundColor Red
        exit 1
    }
    Write-Host "✓ Native library found: $nativeLib" -ForegroundColor Green
    
    if (-not (Test-Path $nativeDll)) {
        Write-Host "✗ Native DLL not found: $nativeDll" -ForegroundColor Red
        Write-Host "Please build the native C++ library first." -ForegroundColor Red
        exit 1
    }
    Write-Host "✓ Native DLL found: $nativeDll" -ForegroundColor Green
    
    Write-Host ""
}

# Function to clean build artifacts
function Clean-Build {
    Write-Host "Cleaning Build Artifacts" -ForegroundColor Cyan
    
    $cleanDirs = @(
        "bin",
        "obj", 
        "dist",
        "tests\bin",
        "tests\obj",
        "tests\TestResults"
    )
    
    foreach ($dir in $cleanDirs) {
        $fullPath = Join-Path $RootDir $dir
        if (Test-Path $fullPath) {
            Write-Host "Removing: $fullPath" -ForegroundColor Gray
            Remove-Item $fullPath -Recurse -Force -ErrorAction SilentlyContinue
        }
    }
    
    Write-Host "✓ Clean completed" -ForegroundColor Green
    Write-Host ""
}

# Function to build the .NET assembly
function Build-Assembly {
    Write-Host "Building .NET Assembly" -ForegroundColor Cyan
    
    $projectFile = Join-Path $RootDir "ExclusionParser.NET.csproj"
    if (-not (Test-Path $projectFile)) {
        Write-Host "✗ Project file not found: $projectFile" -ForegroundColor Red
        exit 1
    }
    
    $verbosityFlag = if ($Verbose) { "--verbosity detailed" } else { "--verbosity minimal" }
    $buildCommand = "dotnet build `"$projectFile`" --configuration $Configuration --platform $Platform $verbosityFlag --no-restore"
    
    # Restore packages first
    Execute-Command "dotnet restore `"$projectFile`"" "Restoring NuGet Packages"
    
    # Build the assembly
    Execute-Command $buildCommand "Building .NET Assembly"
}

# Function to run tests
function Run-Tests {
    Write-Host "Running Unit Tests" -ForegroundColor Cyan
    
    $testProjectFile = Join-Path $RootDir "tests\ExclusionParser.NET.Tests.csproj"
    if (-not (Test-Path $testProjectFile)) {
        Write-Host "✗ Test project file not found: $testProjectFile" -ForegroundColor Red
        return
    }
    
    # Build test project
    $verbosityFlag = if ($Verbose) { "--verbosity detailed" } else { "--verbosity minimal" }
    $buildCommand = "dotnet build `"$testProjectFile`" --configuration $Configuration --platform $Platform $verbosityFlag --no-restore"
    
    Execute-Command "dotnet restore `"$testProjectFile`"" "Restoring Test Packages"
    Execute-Command $buildCommand "Building Test Project"
    
    # Run tests
    $testCommand = "dotnet test `"$testProjectFile`" --configuration $Configuration --platform $Platform --no-build --logger console --logger trx --logger html --collect:`"XPlat Code Coverage`""
    Execute-Command $testCommand "Running Unit Tests"
    
    # Display test results
    $testResultsDir = Join-Path $RootDir "tests\TestResults"
    if (Test-Path $testResultsDir) {
        $trxFiles = Get-ChildItem $testResultsDir -Filter "*.trx" -Recurse | Sort-Object LastWriteTime -Descending
        if ($trxFiles.Count -gt 0) {
            Write-Host "✓ Test results available at: $($trxFiles[0].FullName)" -ForegroundColor Green
        }
    }
}

# Function to create NuGet package
function Create-Package {
    Write-Host "Creating NuGet Package" -ForegroundColor Cyan
    
    $projectFile = Join-Path $RootDir "ExclusionParser.NET.csproj"
    $packCommand = "dotnet pack `"$projectFile`" --configuration $Configuration --platform $Platform --no-build --output `"$RootDir\dist`""
    
    Execute-Command $packCommand "Creating NuGet Package"
    
    # Display package location
    $packageFile = Get-ChildItem "$RootDir\dist" -Filter "*.nupkg" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if ($packageFile) {
        Write-Host "✓ NuGet package created: $($packageFile.FullName)" -ForegroundColor Green
    }
}

# Function to display build summary
function Show-Summary {
    Write-Host "=== Build Summary ===" -ForegroundColor Green
    
    $assemblyFile = Join-Path $RootDir "bin\$Configuration\net8.0\ExclusionParser.NET.dll"
    $distFile = Join-Path $RootDir "dist\ExclusionParser.NET.dll"
    
    if (Test-Path $assemblyFile) {
        $fileInfo = Get-Item $assemblyFile
        Write-Host "✓ Assembly built: $assemblyFile" -ForegroundColor Green
        Write-Host "  Size: $([math]::Round($fileInfo.Length / 1KB, 2)) KB" -ForegroundColor Gray
        Write-Host "  Modified: $($fileInfo.LastWriteTime)" -ForegroundColor Gray
    }
    
    if (Test-Path $distFile) {
        Write-Host "✓ Assembly available in dist: $distFile" -ForegroundColor Green
    }
    
    if ($Package) {
        $packageFile = Get-ChildItem "$RootDir\dist" -Filter "*.nupkg" -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($packageFile) {
            Write-Host "✓ NuGet package: $($packageFile.FullName)" -ForegroundColor Green
        }
    }
    
    Write-Host ""
    Write-Host "Build completed successfully!" -ForegroundColor Green
}

# Main execution
try {
    Check-Prerequisites
    
    if ($Clean) {
        Clean-Build
    }
    
    Build-Assembly
    
    if ($Test) {
        Run-Tests
    }
    
    if ($Package) {
        Create-Package
    }
    
    Show-Summary
}
catch {
    Write-Host ""
    Write-Host "Build failed: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}