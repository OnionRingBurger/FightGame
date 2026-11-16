# Find garbled Japanese comments. Project sources are mostly CP932.
# Signature focused on classic corruption: middle-dot + halfwidth katakana SU (U+30FB U+FF7D),
# halfwidth SO/SU runs, and U+FFFD.
param(
    [string]$ProjectRoot = ''
)

$ErrorActionPreference = 'Stop'
if (-not $ProjectRoot) {
    $ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
}
Set-Location $ProjectRoot

$exclude = '\\(assimp|DirectXTex|EffekseerFiles|nlohmann|imGui|\.cursor|x64|Debug|Release)\\'
$exts = @('*.h', '*.hpp', '*.cpp', '*.c', '*.cc', '*.cxx')

# U+30FB NAKAGURO + U+FF7D HALFWIDTH KATAKANA LETTER SU  => classic "・ｽ"
$sigMojibake = [string]([char]0x30FB) + [char]0xFF7D
# U+FF7F HALFWIDTH SO + U+FF7D SU => "ｿｽ" often paired
$sigSosu = [string]([char]0xFF7F) + [char]0xFF7D
$sigFffd = [char]0xFFFD

function Test-MojibakeLine([string]$line) {
    if ($line -notmatch '//' -and $line -notmatch '/\*' -and $line -notmatch '^\s*\*') { return $false }
    if ($line.Contains($sigFffd)) { return $true }
    if ($line.Contains($sigMojibake)) { return $true }
    if ($line.Contains($sigSosu)) { return $true }
    # dense halfwidth katakana noise in a comment (corruption residue)
    $hw = [regex]::Matches($line, '[' + [char]0xFF61 + '-' + [char]0xFF9F + ']')
    if ($hw.Count -ge 4 -and ($line.Contains('//') -or $line.Contains('/*') -or $line.TrimStart().StartsWith('*'))) {
        # require also nakaguro or question-mark fragment to reduce false positives on intentional halfwidth
        if ($line.Contains([char]0x30FB) -or $line.Contains('?')) { return $true }
    }
    return $false
}

$results = New-Object System.Collections.Generic.List[object]

Get-ChildItem -Recurse -File -Include $exts |
    Where-Object { $_.FullName -notmatch $exclude } |
    ForEach-Object {
        $path = $_.FullName
        $bytes = [IO.File]::ReadAllBytes($path)
        $hasBom = ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF)
        $encUtf8Strict = New-Object Text.UTF8Encoding $false, $true
        $enc932 = [Text.Encoding]::GetEncoding(932)

        $text = $null
        $used = ''
        if ($hasBom) {
            $text = [Text.Encoding]::UTF8.GetString($bytes)
            if ($text.Length -gt 0 -and [int][char]$text[0] -eq 0xFEFF) { $text = $text.Substring(1) }
            $used = 'utf8-bom'
        } else {
            try {
                $text = $encUtf8Strict.GetString($bytes)
                $used = 'utf8'
            } catch {
                $text = $enc932.GetString($bytes)
                $used = 'cp932'
            }
        }

        $lines = $text -split "`r?`n"
        for ($i = 0; $i -lt $lines.Count; $i++) {
            $line = $lines[$i]
            if (Test-MojibakeLine $line) {
                $rel = $path.Substring($ProjectRoot.Length).TrimStart('\', '/')
                $results.Add([pscustomobject]@{
                    File = $rel
                    Line = ($i + 1)
                    EncodingGuess = $used
                    Text = $line.TrimEnd()
                })
            }
        }
    }

Write-Output ('FOUND=' + $results.Count)
Write-Output ('FILES=' + @($results | Group-Object File).Count)
$results | Group-Object File | Sort-Object Name | ForEach-Object {
    Write-Output ('FILECOUNT=' + $_.Name + '=' + $_.Count)
}
$results | ForEach-Object {
    Write-Output ('---')
    Write-Output ('FILE=' + $_.File)
    Write-Output ('LINE=' + $_.Line)
    Write-Output ('ENC=' + $_.EncodingGuess)
    Write-Output ('TEXT=' + $_.Text.Trim())
}
