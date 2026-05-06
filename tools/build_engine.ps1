$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = Split-Path -Parent $ScriptDir
$BuildDir = Join-Path $ProjectDir "build"

if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

Push-Location $BuildDir
try {
    cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug $ProjectDir 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Host "CMake configure failed" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    mingw32-make -j4 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed" -ForegroundColor Red
        exit $LASTEXITCODE
    }
} finally {
    Pop-Location
}
