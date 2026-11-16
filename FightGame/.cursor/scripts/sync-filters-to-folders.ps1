# Aligns FightGame.vcxproj.filters with Source/include folder layout (Option A).
# Encoding-safe UTF-8 (BOM preserved). Nested filters under Japanese roots:
#   ソース ファイル\<path under Source>
#   ヘッダー ファイル\<path under include>
#
# Usage (from project root):
#   powershell -ExecutionPolicy Bypass -File .cursor/scripts/sync-filters-to-folders.ps1
#   ... -DryRun

param(
    [switch]$DryRun
)

$ErrorActionPreference = 'Stop'

$projectDir = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$filtersPath = Join-Path $projectDir 'FightGame.vcxproj.filters'

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

function Get-RootFilterName([string]$text, [string[]]$exts) {
    $filterDefs = [regex]::Matches($text, '<Filter Include="([^"]+)">\s*<UniqueIdentifier>[^<]*</UniqueIdentifier>\s*<Extensions>([^<]*)</Extensions>')
    foreach ($m in $filterDefs) {
        $list = $m.Groups[2].Value.Split(';') | ForEach-Object { $_.Trim().ToLowerInvariant() }
        foreach ($e in $exts) {
            if ($list -contains $e) { return $m.Groups[1].Value }
        }
    }
    throw "Root filter not found for extensions: $($exts -join ',')"
}

function Get-RelativeFilterTail([string]$includePath) {
    $norm = $includePath -replace '/', '\'
    if ($norm -match '^(?i)Source\\(.+)$') {
        $dir = [IO.Path]::GetDirectoryName($Matches[1])
        return @{ Kind = 'Source'; Tail = $dir }
    }
    if ($norm -match '^(?i)include\\(.+)$') {
        $dir = [IO.Path]::GetDirectoryName($Matches[1])
        return @{ Kind = 'Include'; Tail = $dir }
    }
    $dir = [IO.Path]::GetDirectoryName($norm)
    return @{ Kind = 'Other'; Tail = $dir }
}

function Add-FilterChain([hashtable]$guidMap, [string]$fullName) {
    if ([string]::IsNullOrEmpty($fullName)) { return }
    $parts = $fullName -split '\\'
    $acc = ''
    foreach ($p in $parts) {
        if ($acc -eq '') { $acc = $p } else { $acc = $acc + '\' + $p }
        if (-not $guidMap.ContainsKey($acc)) {
            $guidMap[$acc] = '{' + [guid]::NewGuid().ToString().ToUpperInvariant() + '}'
        }
    }
}

$f = Read-TextFile $filtersPath
$text = $f.Text

$sourceRoot = Get-RootFilterName $text @('cpp','c','cc','cxx')
$headerRoot = Get-RootFilterName $text @('h','hh','hpp','hxx')
$resourceRoot = $null
try { $resourceRoot = Get-RootFilterName $text @('rc','ico','bmp') } catch { $resourceRoot = $null }

# Preserve existing root GUIDs / Extensions blocks
$rootBlocks = @{}
foreach ($root in @($sourceRoot, $headerRoot, $resourceRoot)) {
    if (-not $root) { continue }
    $m = [regex]::Match($text, '<Filter Include="' + [regex]::Escape($root) + '">\s*<UniqueIdentifier>([^<]*)</UniqueIdentifier>\s*<Extensions>([^<]*)</Extensions>\s*</Filter>')
    if (-not $m.Success) { throw "Could not parse root filter block: $root" }
    $rootBlocks[$root] = @{ Guid = $m.Groups[1].Value; Ext = $m.Groups[2].Value }
}

$guidMap = @{}
foreach ($root in $rootBlocks.Keys) {
    $guidMap[$root] = $rootBlocks[$root].Guid
}

$fileFilterMap = @{}  # Include path -> filter name

$clCompileMatches = [regex]::Matches($text, '<ClCompile Include="([^"]+)"')
foreach ($m in $clCompileMatches) {
    $inc = $m.Groups[1].Value
    $info = Get-RelativeFilterTail $inc
    if ($info.Kind -ne 'Source' -and $info.Kind -ne 'Other') {
        Write-Warning "ClCompile not under Source\: $inc (using $sourceRoot)"
    }
    $filterName = $sourceRoot
    if (-not [string]::IsNullOrEmpty($info.Tail)) {
        $filterName = $sourceRoot + '\' + ($info.Tail -replace '/', '\')
    }
    Add-FilterChain $guidMap $filterName
    $fileFilterMap[$inc] = $filterName
}

$clIncludeMatches = [regex]::Matches($text, '<ClInclude Include="([^"]+)"')
foreach ($m in $clIncludeMatches) {
    $inc = $m.Groups[1].Value
    $info = Get-RelativeFilterTail $inc
    if ($info.Kind -ne 'Include' -and $info.Kind -ne 'Other') {
        Write-Warning "ClInclude not under include\: $inc (using $headerRoot)"
    }
    $filterName = $headerRoot
    if (-not [string]::IsNullOrEmpty($info.Tail)) {
        $filterName = $headerRoot + '\' + ($info.Tail -replace '/', '\')
    }
    Add-FilterChain $guidMap $filterName
    $fileFilterMap[$inc] = $filterName
}

# Build Filter ItemGroup XML (roots first, then nested sorted)
$filterLines = New-Object System.Collections.Generic.List[string]
$filterLines.Add('  <ItemGroup>')

$ordered = $guidMap.Keys | Sort-Object {
    $depth = ($_ -split '\\').Count
    '{0:D4}|{1}' -f $depth, $_
}

foreach ($name in $ordered) {
    $guid = $guidMap[$name]
    $filterLines.Add('    <Filter Include="' + $name + '">')
    $filterLines.Add('      <UniqueIdentifier>' + $guid + '</UniqueIdentifier>')
    if ($rootBlocks.ContainsKey($name)) {
        $filterLines.Add('      <Extensions>' + $rootBlocks[$name].Ext + '</Extensions>')
    }
    $filterLines.Add('    </Filter>')
}
$filterLines.Add('  </ItemGroup>')
$newFilterGroup = ($filterLines -join "`r`n") + "`r`n"

# Replace first ItemGroup (filter definitions)
$newText = [regex]::Replace($text, '(?s)\s*<ItemGroup>\s*<Filter Include=.*?</ItemGroup>\s*', "`r`n" + $newFilterGroup, 1)
if ($newText -eq $text) { throw 'Failed to replace Filter ItemGroup' }

# Update each file's <Filter> child
foreach ($kv in $fileFilterMap.GetEnumerator()) {
    $inc = $kv.Key
    $fname = $kv.Value
    $esc = [regex]::Escape($inc)
    $pattern = '(<Cl(?:Compile|Include) Include="' + $esc + '">\s*<Filter>)[^<]+(</Filter>)'
    $replacement = '${1}' + $fname + '${2}'
    $updated = [regex]::Replace($newText, $pattern, $replacement, 1)
    if ($updated -eq $newText) {
        throw "Failed to update filter for: $inc"
    }
    $newText = $updated
}

# Normalize line endings to CRLF
$newText = $newText -replace '(?<!\r)\n', "`r`n"

Write-Output ("Roots: source=[$sourceRoot] header=[$headerRoot] resource=[$resourceRoot]")
Write-Output ("Filter nodes: $($guidMap.Count)")
Write-Output ("Files remapped: $($fileFilterMap.Count)")
$samples = $fileFilterMap.GetEnumerator() | Sort-Object Key | Select-Object -First 8
foreach ($s in $samples) {
    Write-Output ("  $($s.Key) -> $($s.Value)")
}

Save-TextFile $filtersPath $newText $f.Bom 'FightGame.vcxproj.filters'
Write-Output 'Done. Reload the project in Visual Studio to pick up the changes.'
