# expand_compile_commands.ps1
# Expands @file.rsp references in compile_commands.json
# Copy the result to project root for VS Code IntelliSense

$ErrorActionPreference = "Stop"

$ScriptDir   = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir  = Split-Path -Parent $ScriptDir
$BuildDir    = Join-Path $ProjectDir "build"
$InputFile   = Join-Path $BuildDir "compile_commands.json"
$OutputFile  = Join-Path $ProjectDir "compile_commands.json"

if (-not (Test-Path $InputFile)) {
    Write-Host "ERROR: $InputFile not found" -ForegroundColor Red
    Write-Host "Run cmake configure first to generate compile_commands.json"
    exit 1
}

Write-Host "Reading compile_commands.json ..."
$data = Get-Content $InputFile -Raw | ConvertFrom-Json

$expandedCount = 0
foreach ($entry in $data) {
    $command = $entry.command

    $rspPattern = '@([^\s"]+\.rsp)'
    $matches = [regex]::Matches($command, $rspPattern)

    foreach ($match in $matches) {
        $rspPath = $match.Groups[1].Value
        $fullRspPath = Join-Path $BuildDir $rspPath

        if (Test-Path $fullRspPath) {
            $rspContent = Get-Content $fullRspPath -Raw
            $rspContent = $rspContent -replace '\r?\n', ' '
            $rspContent = $rspContent.Trim()
            $command = $command.Replace("@" + $rspPath, $rspContent)
            $expandedCount++
        } else {
            Write-Host "WARNING: response file not found: $fullRspPath" -ForegroundColor Yellow
        }
    }

    $command = $command -replace '\\', '/'
    $entry.command = $command
    $entry.directory = $ProjectDir -replace '\\', '/'

    if (-not [System.IO.Path]::IsPathRooted($entry.file)) {
        $entry.file = (Join-Path $BuildDir $entry.file) -replace '\\', '/'
    }
}

Write-Host "Expanded $expandedCount response file references"
$json = $data | ConvertTo-Json -Depth 10
$json | Set-Content -Path $OutputFile -Encoding UTF8
Write-Host "Written to: $OutputFile" -ForegroundColor Green
Write-Host "Done!"
