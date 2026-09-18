# FightGame.vcxproj の ClCompile 一覧から compile_commands.json を生成する。
# 用法: powershell -ExecutionPolicy Bypass -File .vscode/generate-compile-commands.ps1
# clangd / C++ 拡張の「ヘッダ⇔ソース切替」用（include/ と Source/ が別ツリー）。

$ErrorActionPreference = "Stop"
$projectRoot = Split-Path -Parent $PSScriptRoot
$vcxproj = Join-Path $projectRoot "FightGame.vcxproj"
$outFile = Join-Path $projectRoot "compile_commands.json"

$rootUri = ($projectRoot -replace "\\", "/")

$includeFlags = @(
    "-Iinclude", "-Iinclude/assimp", "-Iinclude/DataManager", "-Iinclude/DirectXTex", "-Iinclude/Drawer",
    "-Iinclude/ECS/Core", "-Iinclude/ECS/Component", "-Iinclude/ECS/System", "-Iinclude/ECS/World",
    "-Iinclude/Game/Core", "-Iinclude/Game/Scene", "-Iinclude/imGui", "-Iinclude/nlohmann", "-Iinclude/Sound",
    "-Iinclude/Tool/Debug", "-Iinclude/Tool/Math", "-Iinclude/AI", "-Iinclude/EffekseerFiles", "-Iinclude/DirectWrite",
    "-ISource/imGui"
) -join " "

$defFlags = "-DWIN32 -D_DEBUG -D_CONSOLE -DUNICODE -D_UNICODE"
$baseFlags = "-xc++ -w -std=c++17 --target=x86_64-pc-windows-msvc"

$xml = Get-Content -LiteralPath $vcxproj -Raw -Encoding UTF8
$pattern = 'ClCompile Include="([^"]+)"'
$matches = [regex]::Matches($xml, $pattern)

$entries = @()
foreach ($m in $matches) {
    $rel = ($m.Groups[1].Value -replace "\\", "/")
    $fileUri = "$rootUri/$rel"
    $command = "clang++ $baseFlags $defFlags $includeFlags -c $fileUri"
    $entries += [ordered]@{
        directory = $rootUri
        file      = $fileUri
        command   = $command
    }
}

$json = $entries | ConvertTo-Json -Depth 4
if ($args -contains "-Stdout") {
    Write-Output $json
    exit 0
}
[System.IO.File]::WriteAllText($outFile, $json, [System.Text.UTF8Encoding]::new($false))
Write-Host "Wrote $($entries.Count) entries to $outFile"
