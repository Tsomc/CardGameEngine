# ============================================================
# MyFirstAiGameEngine - 依赖下载脚本
# 首次克隆仓库后运行此脚本，一键下载并编译所有第三方库
# 使用方法: .\setup.ps1
# ============================================================

$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$LibsDir   = Join-Path $ScriptDir "libs"

Write-Host "========================================"  -ForegroundColor Cyan
Write-Host " MyFirstAiGameEngine - 依赖安装"          -ForegroundColor Cyan
Write-Host "========================================"  -ForegroundColor Cyan
Write-Host ""

if (Test-Path $LibsDir) {
    Write-Host "[警告] libs/ 目录已存在，跳过安装" -ForegroundColor Yellow
    Write-Host "  如需重新安装，请先删除 libs/ 目录"
    exit 0
}

New-Item -ItemType Directory -Force -Path $LibsDir | Out-Null

# ============================================================
# 工具函数
# ============================================================
function Download-File($Url, $Out) {
    Write-Host "  下载中..."
    curl.exe -L -o $Out $Url --progress-bar 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "下载失败: $Url"
    }
}

function Extract-Zip-To($Zip, $DestDir) {
    Write-Host "  解压中..."
    New-Item -ItemType Directory -Force -Path $DestDir | Out-Null
    Expand-Archive -Path $Zip -DestinationPath $DestDir -Force
    Remove-Item $Zip -Force
}

# ============================================================
# 1. SDL2 2.30.10
# ============================================================
Write-Host "[1/9] SDL2 2.30.10" -ForegroundColor Green
$zip = Join-Path $LibsDir "_SDL2.zip"
$dest = Join-Path $LibsDir "SDL2"
Download-File "https://github.com/libsdl-org/SDL/releases/download/release-2.30.10/SDL2-devel-2.30.10-mingw.zip" $zip
Extract-Zip-To $zip $dest
Write-Host "  完成"

# ============================================================
# 2. SDL2_image 2.8.4
# ============================================================
Write-Host "[2/9] SDL2_image 2.8.4" -ForegroundColor Green
$zip = Join-Path $LibsDir "_SDL2_image.zip"
$dest = Join-Path $LibsDir "SDL2_image"
Download-File "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.4/SDL2_image-devel-2.8.4-mingw.zip" $zip
Extract-Zip-To $zip $dest
Write-Host "  完成"

# ============================================================
# 3. SDL2_ttf 2.22.0
# ============================================================
Write-Host "[3/9] SDL2_ttf 2.22.0" -ForegroundColor Green
$zip = Join-Path $LibsDir "_SDL2_ttf.zip"
$dest = Join-Path $LibsDir "SDL2_ttf"
Download-File "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-mingw.zip" $zip
Extract-Zip-To $zip $dest
Write-Host "  完成"

# ============================================================
# 4. SDL2_mixer 2.8.0
# ============================================================
Write-Host "[4/9] SDL2_mixer 2.8.0" -ForegroundColor Green
$zip = Join-Path $LibsDir "_SDL2_mixer.zip"
$dest = Join-Path $LibsDir "SDL2_mixer"
Download-File "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-devel-2.8.0-mingw.zip" $zip
Extract-Zip-To $zip $dest
Write-Host "  完成"

# ============================================================
# 5. glm 0.9.9.8
# ============================================================
Write-Host "[5/9] glm 0.9.9.8" -ForegroundColor Green
$zip = Join-Path $LibsDir "_glm.zip"
$dest = Join-Path $LibsDir "glm"
Download-File "https://github.com/g-truc/glm/archive/refs/tags/0.9.9.8.zip" $zip
Extract-Zip-To $zip $dest
Write-Host "  完成"

# ============================================================
# 6. nlohmann/json 3.11.3
# ============================================================
Write-Host "[6/9] nlohmann/json 3.11.3" -ForegroundColor Green
$jsonDir = Join-Path $LibsDir "nlohmann\nlohmann"
New-Item -ItemType Directory -Force -Path $jsonDir | Out-Null
Download-File "https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp" (Join-Path $jsonDir "json.hpp")
Write-Host "  完成"

# ============================================================
# 7. sol2 v3.5.0
# ============================================================
Write-Host "[7/9] sol2 v3.5.0" -ForegroundColor Green
$zip = Join-Path $LibsDir "_sol2.zip"
$dest = Join-Path $LibsDir "sol2"
Download-File "https://github.com/ThePhD/sol2/archive/refs/tags/v3.5.0.zip" $zip
Extract-Zip-To $zip $dest
Write-Host "  完成"

# ============================================================
# 8. Lua 5.4.7（源码编译）
# ============================================================
Write-Host "[8/9] Lua 5.4.7（源码编译）" -ForegroundColor Green
$luaTgz = Join-Path $LibsDir "_lua.tar.gz"
$luaDest = Join-Path $LibsDir "lua-5.4.7"
Download-File "https://www.lua.org/ftp/lua-5.4.7.tar.gz" $luaTgz
Write-Host "  解压中..."
New-Item -ItemType Directory -Force -Path $luaDest | Out-Null
tar -xzf $luaTgz -C $LibsDir
Remove-Item $luaTgz -Force

Write-Host "  编译中..."
$LuaSrc   = Join-Path $luaDest "src"
$LuaBuild = Join-Path $luaDest "build"
New-Item -ItemType Directory -Force -Path $LuaBuild | Out-Null

$srcFiles = @(
    "lapi.c","lcode.c","lctype.c","ldebug.c","ldo.c","ldump.c","lfunc.c",
    "lgc.c","llex.c","lmem.c","lobject.c","lopcodes.c","lparser.c",
    "lstate.c","lstring.c","ltable.c","ltm.c","lundump.c","lvm.c","lzio.c",
    "lauxlib.c","lbaselib.c","lcorolib.c","ldblib.c","liolib.c",
    "lmathlib.c","loadlib.c","loslib.c","lstrlib.c","ltablib.c","lutf8lib.c","linit.c"
)

$objFiles = @()
foreach ($f in $srcFiles) {
    $src = Join-Path $LuaSrc $f
    $obj = Join-Path $LuaBuild ($f -replace '\.c$', '.o')
    gcc -c -O2 -Wall -DLUA_USE_MINGW -I$LuaSrc -o $obj $src
    if ($LASTEXITCODE -ne 0) {
        throw "编译失败: $f"
    }
    $objFiles += $obj
}

$null = & ar rcs (Join-Path $LuaBuild "liblua.a") @objFiles
Write-Host "  完成 (liblua.a)"

# ============================================================
# 9. Dear ImGui v1.91.0
# ============================================================
Write-Host "[9/9] Dear ImGui v1.91.0" -ForegroundColor Green
$zip = Join-Path $LibsDir "_imgui.zip"
$dest = Join-Path $LibsDir "imgui"
Download-File "https://github.com/ocornut/imgui/archive/refs/tags/v1.91.0.zip" $zip
Extract-Zip-To $zip $dest
Write-Host "  完成"

# ============================================================
# 完成
# ============================================================
Write-Host ""
Write-Host "========================================"  -ForegroundColor Cyan
Write-Host "  所有依赖安装完成！"                         -ForegroundColor Cyan
Write-Host "========================================"  -ForegroundColor Cyan
Write-Host ""
Write-Host "下一步:" -ForegroundColor White
Write-Host "  mkdir build && cd build"
Write-Host "  cmake -G 'MinGW Makefiles' -DCMAKE_BUILD_TYPE=Debug .."
Write-Host "  cmake --build ."
Write-Host ""
