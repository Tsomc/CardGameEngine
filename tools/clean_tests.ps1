$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = Split-Path -Parent $ScriptDir
$Paths = @(
    (Join-Path $ProjectDir "build_test"),
    (Join-Path $ProjectDir "output")
)

foreach ($p in $Paths) {
    if (Test-Path $p) {
        Remove-Item -Recurse -Force $p
        Write-Host "Deleted: $p"
    } else {
        Write-Host "Not found: $p"
    }
}
