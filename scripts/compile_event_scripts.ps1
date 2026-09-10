param(
    [Parameter(Mandatory=$true)][string]$FilesPath,
    [string]$ManifestPath = ''
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
if (-not $ManifestPath) { $ManifestPath = Join-Path $projectRoot 'texts/event_script' }
$cx = Join-Path $projectRoot 'tools/ffxcx.exe'
$ev = Join-Path $projectRoot 'tools/ffxev.exe'
$filesRoot = (Resolve-Path -LiteralPath $FilesPath).Path
$manifestRoot = (Resolve-Path -LiteralPath $ManifestPath).Path
$workDir = Join-Path ([IO.Path]::GetTempPath()) ('ffx-event-sync-' + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $workDir | Out-Null
function Run-Tool([string]$Tool, [string[]]$ToolArgs) {
    & $Tool @ToolArgs | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "Tool failed: $Tool $ToolArgs" }
}
function Hex-Bytes([string]$Hex) {
    if ($Hex.Length % 2) { throw 'Invalid hex byte string' }
    $result = New-Object byte[] ($Hex.Length / 2)
    for ($i=0; $i -lt $result.Length; $i++) { $result[$i] = [Convert]::ToByte($Hex.Substring($i*2,2),16) }
    return ,$result
}
function Bytes-Equal([byte[]]$Left, [byte[]]$Right) {
    if ($Left.Length -ne $Right.Length) { return $false }
    for ($i=0; $i -lt $Left.Length; $i++) { if ($Left[$i] -ne $Right[$i]) { return $false } }
    return $true
}
function Apply-Operations([byte[]]$Original, $Operations) {
    $stream = New-Object IO.MemoryStream
    try {
        $cursor=0
        foreach ($op in $Operations) {
            $offset=[int]$op.offset
            [byte[]]$old=Hex-Bytes $op.before
            [byte[]]$new=Hex-Bytes $op.after
            if ($offset -lt $cursor -or $offset+$old.Length -gt $Original.Length) { throw 'Overlapping/out-of-bounds edit' }
            for ($i=0;$i -lt $old.Length;$i++) {
                if ($Original[$offset+$i] -ne $old[$i]) { throw "Original bytes differ at $offset" }
            }
            $stream.Write($Original,$cursor,$offset-$cursor)
            $stream.Write($new,0,$new.Length)
            $cursor=$offset+$old.Length
        }
        $stream.Write($Original,$cursor,$Original.Length-$cursor)
        return ,$stream.ToArray()
    } finally { $stream.Dispose() }
}
function Changes-Are-Applied([byte[]]$Bytes, $Operations) {
    $shift=0
    foreach ($op in $Operations) {
        [byte[]]$old=Hex-Bytes $op.before
        [byte[]]$new=Hex-Bytes $op.after
        $offset=[int]$op.offset+$shift
        if ($offset -lt 0 -or $offset+$new.Length -gt $Bytes.Length) { return $false }
        for ($i=0; $i -lt $new.Length; $i++) { if ($Bytes[$offset+$i] -ne $new[$i]) { return $false } }
        $shift += $new.Length-$old.Length
    }
    return $true
}
function Change-Operations([byte[]]$Original, $Plan) {
    $timingKinds = @('wait_value','remove_int_wait','insert_jp_wait','fmv_progress_value','silent_voice_wait')
    $result = @()
    foreach ($change in @($Plan.changes)) {
        $offset = [int]$change.offset
        if ($change.kind -in $timingKinds) {
            if (($change.value -isnot [int] -and $change.value -isnot [long]) -or $change.value -lt 0 -or $change.value -gt 32767) {
                throw 'Timing value must be an integer in 0..32767'
            }
            $valueHex = ([BitConverter]::ToString([BitConverter]::GetBytes([uint16]$change.value))).Replace('-','').ToLowerInvariant()
            switch ($change.kind) {
                'wait_value' { $oldLength=3; $after='ae'+$valueHex }
                'fmv_progress_value' { $oldLength=3; $after='ae'+$valueHex }
                'remove_int_wait' { if ($change.value -ne 0) { throw 'Removed waits must remain zero' }; $oldLength=6; $after='000000000000' }
                'insert_jp_wait' { $oldLength=0; $after='ae'+$valueHex+'d80000' }
                'silent_voice_wait' {
                    $oldLength = [int]$change.length
                    if ($oldLength -lt 6) { throw 'Silent voice wait region must be at least 6 bytes' }
                    $after = 'ae'+$valueHex+'d80000'+('00' * ($oldLength-6))
                }
            }
            if ($offset -lt 0 -or $offset+$oldLength -gt $Original.Length) { throw 'Out-of-bounds timing change' }
            $before = if ($oldLength) { ([BitConverter]::ToString($Original[$offset..($offset+$oldLength-1)])).Replace('-','').ToLowerInvariant() } else { '' }
            $result += [pscustomobject]@{ offset=$offset; before=$before; after=$after; reason=$change.kind }
        } else {
            if ($null -eq $change.before -or $null -eq $change.after) { throw "Generated change lacks bytes at $offset" }
            $result += [pscustomobject]@{ offset=$offset; before=[string]$change.before; after=[string]$change.after; reason=$change.kind }
        }
    }
    return $result
}
function Other-Sections([byte[]]$Bytes) {
    $ptrs = @()
    for ($i=4; $i -lt 64; $i+=4) {
        $value = [BitConverter]::ToUInt32($Bytes,$i)
        if ($value -eq [uint32]::MaxValue) { break }
        $ptrs += $value
    }
    $result = @{}
    for ($i=1; $i -lt $ptrs.Count; $i++) {
        $start = $ptrs[$i]
        if ($start -eq 0 -or $start -ge $Bytes.Length) { continue }
        $end = $Bytes.Length
        foreach ($p in $ptrs) { if ($p -gt $start -and $p -lt $end) { $end=$p } }
        $part = New-Object byte[] ($end-$start)
        [Array]::Copy($Bytes,$start,$part,0,$part.Length)
        $result[$i] = $part
    }
    return $result
}
try {
    $ready = @()
    $plans = @(Get-ChildItem -LiteralPath $manifestRoot -Filter 'file_*.json' | Sort-Object Name)
    if (-not $plans.Count) { throw 'No event timing manifests found' }
    foreach ($planFile in $plans) {
        $plan = Get-Content -LiteralPath $planFile.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
        if ($plan.schema -ne 2 -or $plan.file -notmatch '^file_[0-9]{5}\.ev\.lz[12]$') { throw "Invalid manifest: $($planFile.Name)" }
        $target = Join-Path $filesRoot $plan.file
        $stem = $plan.file -replace '\.ev\.lz[12]$',''
        $unpacked = Join-Path $workDir ($stem+'.ev')
        $section = Join-Path $workDir ($stem+'.script')
        $packed = Join-Path $workDir $plan.file
        Run-Tool $cx @('-d',$target,$unpacked)
        Run-Tool $ev @('-e0',$unpacked,$section)
        $original = [IO.File]::ReadAllBytes($section)
        $operations = @(Change-Operations $original $plan)
        if (Changes-Are-Applied $original $operations) { Write-Host "  $($plan.file): already synchronized"; continue }
        $beforeEV = [IO.File]::ReadAllBytes($unpacked)
        $beforeSections = Other-Sections $beforeEV
        try {
            [byte[]]$patched=Apply-Operations $original $operations
        } catch { throw "$($plan.file): $($_.Exception.Message). No files have been replaced." }
        [IO.File]::WriteAllBytes($section,$patched)
        Run-Tool $ev @('-i0',$unpacked,$section)
        Run-Tool $ev @('-e0',$unpacked,$section)
        if (-not (Bytes-Equal ([IO.File]::ReadAllBytes($section)) $patched)) { throw 'Reimported script differs' }
        $afterEV=[IO.File]::ReadAllBytes($unpacked)
        $afterSections=Other-Sections $afterEV
        if ($beforeSections.Count -ne $afterSections.Count) { throw 'Unrelated section count changed' }
        foreach ($key in $beforeSections.Keys) {
            if (-not (Bytes-Equal $beforeSections[$key] $afterSections[$key])) { throw "Unrelated section $key changed" }
        }
        $compression='-c'+$plan.file.Substring($plan.file.Length-1)
        Run-Tool $cx @($compression,$unpacked,$packed)
        $roundtrip=Join-Path $workDir ($stem+'.roundtrip')
        Run-Tool $cx @('-d',$packed,$roundtrip)
        if (-not (Bytes-Equal ([IO.File]::ReadAllBytes($roundtrip)) $afterEV)) { throw 'Compression round trip failed' }
        $ready += [pscustomobject]@{ Source=$packed; Target=$target }
        $timingCount = @($plan.changes | Where-Object { $_.kind -in @('wait_value','remove_int_wait','insert_jp_wait','fmv_progress_value','silent_voice_wait') }).Count
        Write-Host "  $($plan.file) [$($plan.map)]: $timingCount timing changes verified"
    }
    # Commit only after every input version, section and compression check succeeds.
    foreach ($item in $ready) { Copy-Item -LiteralPath $item.Source -Destination $item.Target -Force }
    Write-Host "Japanese event timing: $($ready.Count) files updated."
} finally {
    # Only remove this run's direct temporary files; never recurse into caller paths.
    Get-ChildItem -LiteralPath $workDir -File | Remove-Item -Force
    Remove-Item -LiteralPath $workDir
}


