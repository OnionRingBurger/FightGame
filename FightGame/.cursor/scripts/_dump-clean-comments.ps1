$ErrorActionPreference = 'Stop'
$cp932 = [Text.Encoding]::GetEncoding(932)
$sig = [string]([char]0x30FB) + [char]0xFF7D
$blobPath = 'FightGame/include/ECS/Component/Components.h'

function Get-Blob([string]$commit) {
    $tmp = Join-Path $env:TEMP ('fg-clean-' + $commit.Substring(0, 8) + '.bin')
    $err = $tmp + '.err'
    $p = Start-Process -FilePath 'git' -ArgumentList @('-C', 'C:\Team\FightGame', 'show', ($commit + ':' + $blobPath)) -NoNewWindow -Wait -PassThru -RedirectStandardOutput $tmp -RedirectStandardError $err
    if ($p.ExitCode -ne 0) { throw "fail $commit" }
    return $cp932.GetString([IO.File]::ReadAllBytes($tmp))
}

foreach ($c in @('2153274a', '12e4e554')) {
    # resolve full hash
    $full = (git -C 'C:\Team\FightGame' rev-parse $c)
    Write-Output ('==== ' + $full.Substring(0, 8) + ' ====')
    $text = Get-Blob $full
    $lines = $text -split "`r?`n"
    Write-Output ('TOTAL_LINES=' + $lines.Count)
    for ($i = 0; $i -lt $lines.Count; $i++) {
        $line = $lines[$i]
        if (-not ($line.Contains('//') -or $line.Contains('/*'))) { continue }
        # print comment lines that look Japanese OR that match current interest keywords
        $interesting = $false
        if ([regex]::IsMatch($line, '[\u3040-\u30FF\u4E00-\u9FFF]')) { $interesting = $true }
        if ($line -match 'namespace|Entity|fovy|aspect|nearCrip|farCrip|SHOTUI|Result|Json|owner|OBB|World|Camera|Attack|range|angle|radius') { $interesting = $true }
        if ($interesting -and -not $line.Contains($sig)) {
            Write-Output (('{0}:{1}' -f ($i + 1), $line.TrimEnd()))
        }
    }
}

# Current garbled lines with surrounding code context (prev non-empty code token)
Write-Output '==== CURRENT CONTEXT ===='
$now = $cp932.GetString([IO.File]::ReadAllBytes('C:\Team\FightGame\FightGame\include\ECS\Component\Components.h'))
$nlines = $now -split "`r?`n"
$hitLines = @(188,629,673,748,753,760,873,1164,1197,1203,1204,1205,1206,1493,1977,1979,1981,1983,2058,2059,2060,2101,2124,2141,2163,2260,2263,2265)
foreach ($ln in $hitLines) {
    Write-Output ('--- NOW ' + $ln + ' ---')
    $start = [Math]::Max(1, $ln - 3)
    $end = [Math]::Min($nlines.Count, $ln + 3)
    for ($i = $start; $i -le $end; $i++) {
        $mark = $(if ($i -eq $ln) { '>>>' } else { '   ' })
        Write-Output ($mark + ' ' + $i + ':' + $nlines[$i - 1])
    }
}
