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
    $timingKinds = @('wait_value','remove_int_wait','insert_jp_wait','fmv_progress_value','silent_voice_wait','voice_sync_wait','voice_sync_group_start','voice_sync_group_end')
    $result = @()
    foreach ($change in @($Plan.changes)) {
        $offset = [int]$change.offset
        $extraOperations = @()
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
                    if ($oldLength -lt 9) { throw 'Silent voice wait region must be at least 9 bytes' }
                    $after = 'd8d600ae'+$valueHex+'d80000'+('00' * ($oldLength-9))
                }
                { $_ -in @('voice_sync_wait','voice_sync_group_start','voice_sync_group_end') } {
                    $blockStart = $offset-73
                    $oldLength = 79
                    if ($blockStart -lt 0 -or $blockStart+$oldLength -gt $Original.Length) { throw 'Out-of-bounds voice synchronization change' }
                    $transformed = $Original[$blockStart+64] -eq 0xd7
                    $jumpIndexOffset = if ($transformed) { $blockStart+65 } else { $blockStart+62 }
                    $jumpBack = [BitConverter]::ToUInt16($Original,$jumpIndexOffset)+1
                    [byte[]]$afterBytes = New-Object byte[] $oldLength
                    if ($transformed) {
                        [Array]::Copy($Original,$blockStart,$afterBytes,0,$oldLength)
                    } else {
                        [Array]::Copy($Original,$blockStart,$afterBytes,3,64)
                    }
                    if ($change.kind -eq 'voice_sync_group_end') {
                        $afterBytes[0]=0; $afterBytes[1]=0; $afterBytes[2]=0
                    } else {
                        $afterBytes[0]=0xd8; $afterBytes[1]=0xd6; $afterBytes[2]=0
                    }
                    $afterBytes[67]=0xae; $afterBytes[68]=[byte]($change.value -band 0xff); $afterBytes[69]=[byte](($change.value -shr 8) -band 0xff)
                    $afterBytes[70]=0xd8; $afterBytes[71]=0; $afterBytes[72]=0
                    $afterBytes[73]=0xb0; $afterBytes[74]=[byte]($jumpBack -band 0xff); $afterBytes[75]=[byte](($jumpBack -shr 8) -band 0xff)
                    if ($change.kind -eq 'voice_sync_group_start') {
                        $afterBytes[76]=0; $afterBytes[77]=0; $afterBytes[78]=0
                    } else {
                        $afterBytes[76]=0xd8; $afterBytes[77]=0xd9; $afterBytes[78]=0
                    }
                    $after = ([BitConverter]::ToString($afterBytes)).Replace('-','').ToLowerInvariant()
                    $offset = $blockStart

                    $codeOffset = [BitConverter]::ToUInt32($Original,0x30)
                    $tailAddress = [uint32]([int]$change.offset-$codeOffset)
                    $loopAddress = [uint32]($tailAddress-22)
                    $exitJump = [int]($jumpBack-1)
                    $workerCount = [BitConverter]::ToUInt16($Original,0x34)
                    $jumpTable = -1
                    for ($workerIndex=0; $workerIndex -lt $workerCount; $workerIndex++) {
                        $workerHeader = [BitConverter]::ToUInt32($Original,0x38+$workerIndex*4)
                        $jumpCount = [BitConverter]::ToUInt16($Original,$workerHeader+0x0a)
                        if ($jumpCount -le $jumpBack) { continue }
                        $candidate = [BitConverter]::ToUInt32($Original,$workerHeader+0x24)
                        $exitTarget = [BitConverter]::ToUInt32($Original,$candidate+$exitJump*4)
                        $loopTarget = [BitConverter]::ToUInt32($Original,$candidate+$jumpBack*4)
                        if (($exitTarget -eq $tailAddress -or $exitTarget -eq $tailAddress+3) -and
                            ($loopTarget -eq $loopAddress -or $loopTarget -eq $loopAddress+3)) { $jumpTable=$candidate; break }
                    }
                    if ($jumpTable -lt 0) { throw "Voice synchronization jump table not found at $($change.offset)" }
                    foreach ($jump in @(@{ Index=$exitJump; Value=$tailAddress+3 },@{ Index=$jumpBack; Value=$loopAddress+3 })) {
                        $jumpOffset = $jumpTable+$jump.Index*4
                        $jumpBefore = ([BitConverter]::ToString($Original[$jumpOffset..($jumpOffset+3)])).Replace('-','').ToLowerInvariant()
                        $jumpAfter = ([BitConverter]::ToString([BitConverter]::GetBytes([uint32]$jump.Value))).Replace('-','').ToLowerInvariant()
                        $extraOperations += [pscustomobject]@{ offset=$jumpOffset; before=$jumpBefore; after=$jumpAfter; reason='voice_sync_jump' }
                    }
                    if ($change.kind -eq 'voice_sync_group_end') {
                        $voiceOffset = $blockStart-12
                        $voiceBefore = ([BitConverter]::ToString($Original[$voiceOffset..($voiceOffset+11)])).Replace('-','').ToLowerInvariant()
                        $extraOperations += [pscustomobject]@{ offset=$voiceOffset; before=$voiceBefore; after=('00'*12); reason='voice_sync_continue' }
                    }
                }
            }
            if ($offset -lt 0 -or $offset+$oldLength -gt $Original.Length) { throw 'Out-of-bounds timing change' }
            $before = if ($oldLength) { ([BitConverter]::ToString($Original[$offset..($offset+$oldLength-1)])).Replace('-','').ToLowerInvariant() } else { '' }
            $result += [pscustomobject]@{ offset=$offset; before=$before; after=$after; reason=$change.kind }
            $result += $extraOperations
        } else {
            if ($null -eq $change.before -or $null -eq $change.after) { throw "Generated change lacks bytes at $offset" }
            $result += [pscustomobject]@{ offset=$offset; before=[string]$change.before; after=[string]$change.after; reason=$change.kind }
        }
    }
    return @($result | Sort-Object offset)
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
        $timingCount = @($plan.changes | Where-Object { $_.kind -in @('wait_value','remove_int_wait','insert_jp_wait','fmv_progress_value','silent_voice_wait','voice_sync_wait','voice_sync_group_start','voice_sync_group_end') }).Count
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


