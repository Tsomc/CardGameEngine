# add_namespace.ps1 — 给引擎源码添加 namespace engine {}
# 使用 UTF8 编码安全地读写文件

$EngineDir = "D:\projects\MyFirstAiGameEngine\app\engine"
$utf8 = [System.Text.UTF8Encoding]::new($false)  # UTF-8 无 BOM

$subDirs = @(
    "foundation", "core", "platform", "input", "render",
    "audio", "resource", "script", "animation", "card",
    "scene", "ui", "debug"
)

Write-Host "Processing headers..."

$hCount = 0
foreach ($sd in $subDirs) {
    $dir = "$EngineDir/$sd"
    if (-not (Test-Path $dir)) { continue }
    Get-ChildItem $dir -Filter "*.h" | ForEach-Object {
        $text = [System.IO.File]::ReadAllText($_.FullName, $utf8)
        if ($text -match 'namespace engine \{') { return }

        $lines = [System.Collections.Generic.List[string]]::new()
        $afterGuardDef = $false
        $nsInserted = $false

        foreach ($line in ($text -split "`r`n|`n")) {
            if ($line -match '^#define\s+\w+') {
                $afterGuardDef = $true
                $lines.Add($line)
                continue
            }

            if ($line -match '^#endif') {
                if ($nsInserted) {
                    $lines.Add('')
                    $lines.Add('} /* namespace engine */')
                }
                $lines.Add($line)
                continue
            }

            if ($afterGuardDef -and -not $nsInserted -and -not ($line -match '^#include|^\s*$|^/\*|^//|^#pragma')) {
                $lines.Add('')
                $lines.Add('namespace engine {')
                $lines.Add('')
                $nsInserted = $true
            }

            $lines.Add($line)
        }

        if ($nsInserted) {
            [System.IO.File]::WriteAllText($_.FullName, ($lines -join "`r`n"), $utf8)
            $hCount++
            Write-Host "  [H] $($_.Name)"
        }
    }
}
Write-Host "Processed $hCount header files"

Write-Host "Processing sources..."

$cppCount = 0
foreach ($sd in $subDirs) {
    $dir = "$EngineDir/$sd"
    if (-not (Test-Path $dir)) { continue }
    Get-ChildItem $dir -Filter "*.cpp" | ForEach-Object {
        $text = [System.IO.File]::ReadAllText($_.FullName, $utf8)
        if ($text -match 'using namespace engine') { return }

        $lines = [System.Collections.Generic.List[string]]::new()
        $inIncludes = $false
        $inserted = $false

        foreach ($line in ($text -split "`r`n|`n")) {
            if ($line -match '^#include') {
                $inIncludes = $true
                $lines.Add($line)
                continue
            }

            if ($inIncludes -and -not $inserted) {
                if ($line -match '^\s*$') {
                    $lines.Add('using namespace engine;')
                    $inserted = $true
                    continue
                } else {
                    $lines.Add('using namespace engine;')
                    $lines.Add('')
                    $lines.Add($line)
                    $inserted = $true
                    continue
                }
            }

            $lines.Add($line)
        }

        if ($inserted) {
            [System.IO.File]::WriteAllText($_.FullName, ($lines -join "`r`n"), $utf8)
            $cppCount++
            Write-Host "  [CPP] $($_.Name)"
        }
    }
}
Write-Host "Processed $cppCount cpp files"
Write-Host "Done!"
