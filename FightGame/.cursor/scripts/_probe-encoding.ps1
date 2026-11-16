$ErrorActionPreference = 'Stop'
Set-Location (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path

$p = 'include\ECS\Component\Components.h'
$b = [IO.File]::ReadAllBytes($p)
$bom = ($b.Length -ge 3 -and $b[0] -eq 0xEF -and $b[1] -eq 0xBB -and $b[2] -eq 0xBF)
Write-Output ("size=" + $b.Length + " bom=" + $bom)

$cp932 = [Text.Encoding]::GetEncoding(932)
$t932 = $cp932.GetString($b)
$t8 = [Text.Encoding]::UTF8.GetString($b)
$l932 = ($t932 -split "`r?`n")[187]
$l8 = ($t8 -split "`r?`n")[187]
Write-Output 'AS_CP932:'
Write-Output $l932
Write-Output 'AS_UTF8:'
Write-Output $l8

# Count signature in whole file under each decode
$sig = [string]([char]0x30FB) + [char]0xFF7D
Write-Output ('sig_count_cp932=' + ([regex]::Matches($t932, [regex]::Escape($sig)).Count))
Write-Output ('sig_count_utf8=' + ([regex]::Matches($t8, [regex]::Escape($sig)).Count))

# Also sample a known-good comment area if any
Write-Output 'Sample lines with // under CP932 that contain nakaguro+su:'
$lines = $t932 -split "`r?`n"
for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i].Contains($sig) -and ($lines[$i] -match '//|/\*')) {
        Write-Output (("  {0}: {1}" -f ($i + 1), $lines[$i].Trim()))
        if ($i -gt 20) { break }
    }
}
