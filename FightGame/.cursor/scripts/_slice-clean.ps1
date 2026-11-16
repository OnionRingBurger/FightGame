$cp932 = [Text.Encoding]::GetEncoding(932)
$tmp = Join-Path $env:TEMP 'fg-t.bin'
Start-Process git -ArgumentList @('-C', 'C:\Team\FightGame', 'show', '2153274a:FightGame/include/ECS/Component/Components.h') -NoNewWindow -Wait -RedirectStandardOutput $tmp -RedirectStandardError ($tmp + '.e') | Out-Null
$t = $cp932.GetString([IO.File]::ReadAllBytes($tmp))
$lines = $t -split "`r?`n"
660..730 | ForEach-Object { '{0}:{1}' -f $_, $lines[$_ - 1] }
Write-Output '----'
2200..2290 | ForEach-Object { '{0}:{1}' -f $_, $lines[$_ - 1] }
