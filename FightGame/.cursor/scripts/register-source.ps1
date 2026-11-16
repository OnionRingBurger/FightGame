# Registers or removes a source file entry in FightGame.vcxproj and FightGame.vcxproj.filters.
# Designed to be encoding-safe: both files are read/written as UTF-8 explicitly,
# and Japanese filter root names are detected from the filters file (no non-ASCII literals here).
# New files are placed under Option A nested filters:
#   <root>\<relative dir under Source or include>
#
# Usage (run from the project root, next to FightGame.vcxproj):
#   powershell -ExecutionPolicy Bypass -File .cursor/scripts/register-source.ps1 -RelativePath "include\ECS\System\FooSystem.h"
#   ... -RelativePath "Source\ECS\System\FooSystem.cpp"
#   ... -Remove   : remove the entry instead of adding it
#   ... -DryRun   : validate and show what would change, without writing

param(
    [Parameter(Mandatory=$true)][string]$RelativePath,
    [switch]$Remove,
    [switch]$DryRun
)

$ErrorActionPreference = 'Stop'

$projectDir = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$vcxprojPath = Join-Path $projectDir 'FightGame.vcxproj'
$filtersPath = "$vcxprojPath.filters"

$RelativePath = $RelativePath -replace '/', '\'
$ext = [IO.Path]::GetExtension($RelativePath).TrimStart('.').ToLowerInvariant()

$headerExts = @('h','hh','hpp','hxx','inl','inc','ipp')
$sourceExts = @('cpp','c','cc','cxx','ixx','asm')
if ($headerExts -contains $ext) { $tag = 'ClInclude' }
elseif ($sourceExts -contains $ext) { $tag = 'ClCompile' }
else { throw "Unsupported extension: .$ext" }

function Read-TextFile([string]$path) {
    $bytes = [IO.File]::ReadAllBytes($path)
    $hasBom = $bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF
    $text = [Text.Encoding]::UTF8.GetString($bytes)
    if ($hasBom) { $text = $text.Substring(1) }
    return @{ Text = $text; Bom = $hasBom }
}

function Assert-ValidXml([string]$text, [string]$name) {
    try { $null = [xml]$text } catch { throw "XML validation failed for ${name}: $($_.Exception.Message)" }
}

function Save-TextFile([string]$path, [string]$text, [bool]$bom, [string]$name) {
    Assert-ValidXml $text $name
    if ($DryRun) { Write-Output "[dry-run] $name is valid, not written."; return }
    [IO.File]::WriteAllText($path, $text, (New-Object Text.UTF8Encoding($bom)))
    Write-Output "$name updated."
}

function Insert-AfterLastMatch([string]$text, [string]$linePattern, [string]$insertText, [string]$name) {
    $lines = [System.Collections.Generic.List[string]]($text -split '(?<=\n)')
    $lastIdx = -1
    for ($i = 0; $i -lt $lines.Count; $i++) {
        if ($lines[$i] -match $linePattern) { $lastIdx = $i }
    }
    if ($lastIdx -lt 0) { throw "No existing $tag entries found in ${name}; manual edit required." }
    $lines.Insert($lastIdx + 1, $insertText)
    return ($lines -join '')
}

function Get-RootFilterName([string]$text, [string]$fileExt) {
    $filterDefs = [regex]::Matches($text, '<Filter Include="([^"]+)">\s*<UniqueIdentifier>[^<]*</UniqueIdentifier>\s*<Extensions>([^<]*)</Extensions>')
    foreach ($m in $filterDefs) {
        $list = $m.Groups[2].Value.Split(';') | ForEach-Object { $_.Trim().ToLowerInvariant() }
        if ($list -contains $fileExt) { return $m.Groups[1].Value }
    }
    throw "No root filter found for extension .$fileExt in FightGame.vcxproj.filters"
}

function Get-NestedFilterName([string]$root, [string]$relPath) {
    $norm = $relPath -replace '/', '\'
    $under = $null
    if ($norm -match '^(?i)Source\\(.+)$') { $under = $Matches[1] }
    elseif ($norm -match '^(?i)include\\(.+)$') { $under = $Matches[1] }
    else { $under = $norm }
    $dir = [IO.Path]::GetDirectoryName($under)
    if ([string]::IsNullOrEmpty($dir)) { return $root }
    return $root + '\' + ($dir -replace '/', '\')
}

function Ensure-FilterNodes([string]$text, [string]$fullFilterName) {
    $parts = $fullFilterName -split '\\'
    $acc = ''
    $result = $text
    foreach ($p in $parts) {
        if ($acc -eq '') { $acc = $p } else { $acc = $acc + '\' + $p }
        if ($result -match ('<Filter Include="' + [regex]::Escape($acc) + '">')) { continue }

        $guid = '{' + [guid]::NewGuid().ToString().ToUpperInvariant() + '}'
        $node = '    <Filter Include="' + $acc + '">' + "`r`n" +
                '      <UniqueIdentifier>' + $guid + '</UniqueIdentifier>' + "`r`n" +
                '    </Filter>' + "`r`n"

        # Insert before </ItemGroup> of the first ItemGroup that already has a <Filter
        $m = [regex]::Match($result, '(?s)(<ItemGroup>\s*<Filter Include=.*?)(</ItemGroup>)')
        if (-not $m.Success) { throw 'Could not find Filter ItemGroup to insert into' }
        $result = $result.Substring(0, $m.Groups[2].Index) + $node + $result.Substring($m.Groups[2].Index)
        if ($DryRun) { Write-Output "[dry-run] would add Filter: $acc" }
    }
    return $result
}

$escaped = [regex]::Escape($RelativePath)

# ---- FightGame.vcxproj ----
$v = Read-TextFile $vcxprojPath
if ($Remove) {
    $pattern = '(?m)^[ \t]*<' + $tag + ' Include="' + $escaped + '"[ \t]*/>[ \t]*\r?\n'
    if ($v.Text -notmatch $pattern) {
        Write-Warning "vcxproj: entry not found: $RelativePath"
    } else {
        $newText = [regex]::Replace($v.Text, $pattern, '')
        Save-TextFile $vcxprojPath $newText $v.Bom 'FightGame.vcxproj'
    }
} else {
    if ($v.Text -match ('<' + $tag + ' Include="' + $escaped + '"')) {
        Write-Output 'vcxproj: already registered, skipping.'
    } else {
        $entry = '    <' + $tag + ' Include="' + $RelativePath + '" />' + "`r`n"
        $newText = Insert-AfterLastMatch $v.Text ('^[ \t]*<' + $tag + ' Include="[^"]*"[ \t]*/>') $entry 'FightGame.vcxproj'
        if ($DryRun) { Write-Output "[dry-run] vcxproj entry: $($entry.Trim())" }
        Save-TextFile $vcxprojPath $newText $v.Bom 'FightGame.vcxproj'
    }
}

# ---- FightGame.vcxproj.filters ----
$f = Read-TextFile $filtersPath
if ($Remove) {
    $pattern = '(?ms)^[ \t]*<' + $tag + ' Include="' + $escaped + '">.*?</' + $tag + '>[ \t]*\r?\n'
    if ($f.Text -notmatch $pattern) {
        Write-Warning "filters: entry not found: $RelativePath"
    } else {
        $newText = [regex]::Replace($f.Text, $pattern, '')
        Save-TextFile $filtersPath $newText $f.Bom 'FightGame.vcxproj.filters'
    }
} else {
    if ($f.Text -match ('<' + $tag + ' Include="' + $escaped + '"')) {
        Write-Output 'filters: already registered, skipping.'
    } else {
        $rootName = Get-RootFilterName $f.Text $ext
        $filterName = Get-NestedFilterName $rootName $RelativePath
        $withNodes = Ensure-FilterNodes $f.Text $filterName

        $entry = '    <' + $tag + ' Include="' + $RelativePath + '">' + "`r`n" +
                 '      <Filter>' + $filterName + '</Filter>' + "`r`n" +
                 '    </' + $tag + '>' + "`r`n"
        $newText = Insert-AfterLastMatch $withNodes ('^[ \t]*</' + $tag + '>') $entry 'FightGame.vcxproj.filters'
        if ($DryRun) { Write-Output "[dry-run] filters entry under: $filterName" }
        Save-TextFile $filtersPath $newText $f.Bom 'FightGame.vcxproj.filters'
    }
}

Write-Output 'Done. Reload the project in Visual Studio to pick up the changes.'
