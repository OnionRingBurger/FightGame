# Registers or removes a source file entry in FightGame.vcxproj and FightGame.vcxproj.filters.
# Designed to be encoding-safe: both files are read/written as UTF-8 explicitly,
# and the filter name is detected from the filters file (no non-ASCII literals here).
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
        # Detect the filter name from the Extensions lists in the filters file itself,
        # so this script contains no non-ASCII filter names.
        $filterName = $null
        $filterDefs = [regex]::Matches($f.Text, '<Filter Include="([^"]+)">\s*<UniqueIdentifier>[^<]*</UniqueIdentifier>\s*<Extensions>([^<]*)</Extensions>')
        foreach ($m in $filterDefs) {
            if (($m.Groups[2].Value.Split(';') | ForEach-Object { $_.Trim().ToLowerInvariant() }) -contains $ext) {
                $filterName = $m.Groups[1].Value
                break
            }
        }
        if (-not $filterName) { throw "No filter found for extension .$ext in FightGame.vcxproj.filters" }

        $entry = '    <' + $tag + ' Include="' + $RelativePath + '">' + "`r`n" +
                 '      <Filter>' + $filterName + '</Filter>' + "`r`n" +
                 '    </' + $tag + '>' + "`r`n"
        $newText = Insert-AfterLastMatch $f.Text ('^[ \t]*</' + $tag + '>') $entry 'FightGame.vcxproj.filters'
        if ($DryRun) { Write-Output "[dry-run] filters entry added under filter of .$ext" }
        Save-TextFile $filtersPath $newText $f.Bom 'FightGame.vcxproj.filters'
    }
}

Write-Output 'Done. Reload the project in Visual Studio to pick up the changes.'
