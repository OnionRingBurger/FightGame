$ErrorActionPreference = 'Stop'
Set-Location 'C:\Team\FightGame\FightGame'
& powershell -ExecutionPolicy Bypass -File '.\.cursor\scripts\find-mojibake-comments.ps1' | Select-String -Pattern '^(FOUND|FILES|FILECOUNT=)'

$p = 'include\ECS\Component\Components.h'
$b = [IO.File]::ReadAllBytes($p)
$t = [Text.Encoding]::GetEncoding(932).GetString($b)
$L = $t -split "`r?`n"
Write-Output ('size=' + $b.Length + ' lines=' + $L.Count)
$nums = @(188,629,673,748,753,760,873,1164,1197,1203,1204,1205,1206,1493,1977,1979,1981,1983,2058,2059,2060,2103,2126,2143,2165,2262,2265,2267)
foreach ($n in $nums) {
    Write-Output ($n.ToString() + ':' + $L[$n - 1].Trim())
}
