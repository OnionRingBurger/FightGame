$ErrorActionPreference = 'Stop'
Set-Location 'C:\Team\FightGame\FightGame'

$cp932 = [Text.Encoding]::GetEncoding(932)
$utf8 = New-Object Text.UTF8Encoding $false, $false
$sig = [string]([char]0x30FB) + [char]0xFF7D
$sosu = [string]([char]0xFF7F) + [char]0xFF7D
$blobPath = 'FightGame/include/ECS/Component/Components.h'
$pathWin = 'include\ECS\Component\Components.h'

function Has-GoodJp([string]$s) {
    return [regex]::IsMatch($s, '[\u3040-\u30FF\u4E00-\u9FFF]{2,}') -and (-not $s.Contains($sig))
}

function Get-BlobTextCp932([string]$commit) {
    $tmp = Join-Path $env:TEMP ('fg-blob-' + $commit.Substring(0, 8) + '.bin')
    $err = $tmp + '.err'
    $p = Start-Process -FilePath 'git' -ArgumentList @('-C', 'C:\Team\FightGame', 'show', ($commit + ':' + $blobPath)) -NoNewWindow -Wait -PassThru -RedirectStandardOutput $tmp -RedirectStandardError $err
    if ($p.ExitCode -ne 0) { return $null }
    if (-not (Test-Path $tmp)) { return $null }
    $bytes = [IO.File]::ReadAllBytes($tmp)
    if ($bytes.Length -lt 10) { return $null }
    return $cp932.GetString($bytes)
}

function Try-Algo([string]$garbled) {
    $out = @()
    try {
        $cand = $utf8.GetString($cp932.GetBytes($garbled))
        if (Has-GoodJp $cand) { $out += [pscustomobject]@{ Method = 'cp932bytes->utf8'; Text = $cand } }
    } catch {}
    try {
        $cand = $cp932.GetString($utf8.GetBytes($garbled))
        if (Has-GoodJp $cand) { $out += [pscustomobject]@{ Method = 'utf8bytes->cp932'; Text = $cand } }
    } catch {}
    return $out
}

$textNow = $cp932.GetString([IO.File]::ReadAllBytes($pathWin))
$linesNow = $textNow -split "`r?`n"
$hits = @()
for ($i = 0; $i -lt $linesNow.Count; $i++) {
    $line = $linesNow[$i]
    if (($line.Contains('//') -or $line.Contains('/*')) -and ($line.Contains($sig) -or $line.Contains($sosu))) {
        $hits += [pscustomobject]@{ Line = $i + 1; Text = $line.TrimEnd() }
    }
}
Write-Output ('HITS=' + $hits.Count)

$commits = @(git -C 'C:\Team\FightGame' log --follow --pretty=format:'%H' -- $blobPath)
Write-Output ('COMMITS=' + $commits.Count)

$commitTexts = @{}
$minSig = [int]::MaxValue
$minCommit = $null
foreach ($c in $commits) {
    $t = Get-BlobTextCp932 $c
    if (-not $t) {
        Write-Output ('COMMIT_FAIL=' + $c.Substring(0, 8))
        continue
    }
    $commitTexts[$c] = $t
    $cnt = ([regex]::Matches($t, [regex]::Escape($sig))).Count
    Write-Output ('COMMIT_SIG=' + $c.Substring(0, 8) + '=' + $cnt)
    if ($cnt -lt $minSig) { $minSig = $cnt; $minCommit = $c }
}
Write-Output ('MIN_SIG=' + $minSig + ' at ' + $(if ($minCommit) { $minCommit.Substring(0, 8) } else { 'none' }))

$sampleBytes = $cp932.GetBytes($hits[0].Text)
Write-Output ('SAMPLE_HEX=' + (($sampleBytes | ForEach-Object { $_.ToString('X2') }) -join ' '))
try { Write-Output ('SAMPLE_AS_UTF8_TRY=' + $utf8.GetString($sampleBytes)) } catch { Write-Output 'SAMPLE_AS_UTF8_TRY=FAIL' }

foreach ($h in $hits) {
    Write-Output ('---')
    Write-Output ('LINE=' + $h.Line)
    Write-Output ('NOW=' + $h.Text.Trim())

    foreach ($a in (Try-Algo $h.Text)) {
        Write-Output ('ALGO=' + $a.Method + ' => ' + $a.Text.Trim())
    }

    $token = $null
    if ($h.Text -match '\b([A-Za-z_][A-Za-z0-9_]{4,})\b') { $token = $Matches[1] }

    $gitHit = $null
    foreach ($kv in $commitTexts.GetEnumerator()) {
        $clines = $kv.Value -split "`r?`n"
        $cands = @()
        if ($h.Line -ge 1 -and $h.Line -le $clines.Count) { $cands += $clines[$h.Line - 1] }
        if ($token) {
            foreach ($cl in $clines) {
                if ($cl.Contains($token) -and $cl.Contains('//')) { $cands += $cl }
            }
        }
        foreach ($cand in ($cands | Select-Object -Unique)) {
            if (-not $cand) { continue }
            if ($cand.Contains($sig) -or $cand.Contains($sosu)) { continue }
            if (Has-GoodJp $cand) {
                $gitHit = [pscustomobject]@{ Commit = $kv.Key.Substring(0, 8); Text = $cand.Trim() }
                break
            }
        }
        if ($gitHit) { break }
    }

    if ($gitHit) {
        Write-Output ('GIT=' + $gitHit.Commit + ' => ' + $gitHit.Text)
        Write-Output 'RECOVERABLE=YES'
    } else {
        $algoOk = @(Try-Algo $h.Text).Count -gt 0
        if ($algoOk) { Write-Output 'RECOVERABLE=YES (algo only)' }
        else { Write-Output 'RECOVERABLE=NO' }
    }
}
