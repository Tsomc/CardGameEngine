# expand_compile_commands.ps1
# Expands @file.rsp references and merges engine + test compile_commands.json

$ErrorActionPreference = "Stop"

$ScriptDir   = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir  = Split-Path -Parent $ScriptDir
$OutputFile  = Join-Path $ProjectDir "compile_commands.json"

# Collect from multiple build directories
$buildDirs = @(
    (Join-Path $ProjectDir "build"),       # Engine
    (Join-Path $ProjectDir "build_test")   # Tests
)

$allEntries = @()

foreach ($buildDir in $buildDirs) {
    $inputFile = Join-Path $buildDir "compile_commands.json"
    if (-not (Test-Path $inputFile)) {
        Write-Host "SKIP: $inputFile not found" -ForegroundColor Yellow
        continue
    }

    Write-Host "Reading $inputFile ..."
    $data = Get-Content $inputFile -Raw | ConvertFrom-Json

    $expandedCount = 0
    foreach ($entry in $data) {
        $command = $entry.command

        $rspPattern = '@([^\s"]+\.rsp)'
        $matches = [regex]::Matches($command, $rspPattern)

        foreach ($match in $matches) {
            $rspPath = $match.Groups[1].Value
            $fullRspPath = Join-Path $buildDir $rspPath

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
            $entry.file = (Join-Path $buildDir $entry.file) -replace '\\', '/'
        }
    }

    Write-Host "  Expanded $expandedCount response file references"
    $allEntries += $data
}

if ($allEntries.Count -eq 0) {
    Write-Host "ERROR: no compile commands found. Run cmake configure first." -ForegroundColor Red
    exit 1
}

$json = $allEntries | ConvertTo-Json -Depth 10
$json | Set-Content -Path $OutputFile -Encoding UTF8
Write-Host "Merged $($allEntries.Count) entries into: $OutputFile" -ForegroundColor Green
Write-Host "Done!"
