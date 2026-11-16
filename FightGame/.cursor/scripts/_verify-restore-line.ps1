$ErrorActionPreference = 'Stop'
$path = 'C:\Team\FightGame\FightGame\include\ECS\Component\Components.h'
$bytes = [IO.File]::ReadAllBytes($path)
$cp932 = [Text.Encoding]::GetEncoding(932)
$utf8 = [Text.Encoding]::UTF8
Write-Output ('size=' + $bytes.Length)
$t932 = $cp932.GetString($bytes)
$lines = $t932 -split "`r?`n"
Write-Output ('lines=' + $lines.Count)
Write-Output ('L629_CP932=' + $lines[628].Trim())
# Also check if that line's raw bytes are valid CP932 for 減衰率
$idx = [Text.Encoding]::ASCII.GetString($bytes).IndexOf('float3 force')
Write-Output ('force_idx=' + $idx)
# Find attenuation comment line bytes between force and attenuation
$textAsciiProbe = [Text.Encoding]::GetEncoding(28591).GetString($bytes)
$m = [regex]::Match($textAsciiProbe, 'float3 force;\r?\n([^\r\n]+)\r?\n\t\tfloat attenuation;')
if ($m.Success) {
    $commentLatin = $m.Groups[1].Value
    $commentBytes = [Text.Encoding]::GetEncoding(28591).GetBytes($commentLatin)
    Write-Output ('comment_hex=' + (($commentBytes | ForEach-Object { $_.ToString('X2') }) -join ' '))
    Write-Output ('comment_as_cp932=' + $cp932.GetString($commentBytes))
    try {
        $strict = New-Object Text.UTF8Encoding $false, $true
        Write-Output ('comment_as_utf8=' + $strict.GetString($commentBytes))
    } catch {
        Write-Output 'comment_as_utf8=NOT_VALID_UTF8'
    }
}
