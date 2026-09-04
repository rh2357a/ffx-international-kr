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
function Hash([byte[]]$Bytes) {
    $sha = [Security.Cryptography.SHA256]::Create()
    try { return ([BitConverter]::ToString($sha.ComputeHash($Bytes))).Replace('-','').ToLowerInvariant() }
    finally { $sha.Dispose() }
}
function Hex-Bytes([string]$Hex) {
    if ($Hex.Length % 2) { throw 'Invalid hex byte string' }
    $result = New-Object byte[] ($Hex.Length / 2)
    for ($i=0; $i -lt $result.Length; $i++) { $result[$i] = [Convert]::ToByte($Hex.Substring($i*2,2),16) }
    return ,$result
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
        $result[$i] = Hash $part
    }
    return $result
}
try {
    $ready = @()
    $plans = @(Get-ChildItem -LiteralPath $manifestRoot -Filter 'file_*.json' | Sort-Object Name)
    if (-not $plans.Count) { throw 'No event timing manifests found' }
    foreach ($planFile in $plans) {
        $plan = Get-Content -LiteralPath $planFile.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
        if ($plan.schema -ne 1 -or $plan.file -notmatch '^file_[0-9]{5}\.ev\.lz[12]$') { throw "Invalid manifest: $($planFile.Name)" }
        $target = Join-Path $filesRoot $plan.file
        $stem = $plan.file -replace '\.ev\.lz[12]$',''
        $unpacked = Join-Path $workDir ($stem+'.ev')
        $section = Join-Path $workDir ($stem+'.script')
        $packed = Join-Path $workDir $plan.file
        Run-Tool $cx @('-d',$target,$unpacked)
        Run-Tool $ev @('-e0',$unpacked,$section)
        $original = [IO.File]::ReadAllBytes($section)
        $hash = Hash $original
        if ($hash -eq $plan.patched_sha256) { Write-Host "  $($plan.file): already synchronized"; continue }
        if ($hash -ne $plan.original_sha256) { throw "Unexpected script version: $($plan.file). No files have been replaced." }
        $beforeEV = [IO.File]::ReadAllBytes($unpacked)
        $beforeSections = Other-Sections $beforeEV
        $stream = New-Object IO.MemoryStream
        try {
            $cursor=0
            foreach ($op in $plan.operations) {
                $offset=[int]$op.offset
                [byte[]]$old=Hex-Bytes $op.before
                [byte[]]$new=Hex-Bytes $op.after
                if ($offset -lt $cursor -or $offset+$old.Length -gt $original.Length) { throw 'Overlapping/out-of-bounds edit' }
                for ($i=0;$i -lt $old.Length;$i++) {
                    if ($original[$offset+$i] -ne $old[$i]) { throw "Original bytes differ: $($plan.file) at $offset" }
                }
                $stream.Write($original,$cursor,$offset-$cursor)
                $stream.Write($new,0,$new.Length)
                $cursor=$offset+$old.Length
            }
            $stream.Write($original,$cursor,$original.Length-$cursor)
            [byte[]]$patched=$stream.ToArray()
        } finally { $stream.Dispose() }
        if ((Hash $patched) -ne $plan.patched_sha256) { throw "Patched script hash mismatch: $($plan.file)" }
        [IO.File]::WriteAllBytes($section,$patched)
        Run-Tool $ev @('-i0',$unpacked,$section)
        Run-Tool $ev @('-e0',$unpacked,$section)
        if ((Hash ([IO.File]::ReadAllBytes($section))) -ne $plan.patched_sha256) { throw 'Reimported script differs' }
        $afterEV=[IO.File]::ReadAllBytes($unpacked)
        $afterSections=Other-Sections $afterEV
        if ($beforeSections.Count -ne $afterSections.Count) { throw 'Unrelated section count changed' }
        foreach ($key in $beforeSections.Keys) {
            if ($beforeSections[$key] -ne $afterSections[$key]) { throw "Unrelated section $key changed" }
        }
        $compression='-c'+$plan.file.Substring($plan.file.Length-1)
        Run-Tool $cx @($compression,$unpacked,$packed)
        $roundtrip=Join-Path $workDir ($stem+'.roundtrip')
        Run-Tool $cx @('-d',$packed,$roundtrip)
        if ((Hash ([IO.File]::ReadAllBytes($roundtrip))) -ne (Hash $afterEV)) { throw 'Compression round trip failed' }
        $ready += [pscustomobject]@{ Source=$packed; Target=$target }
        Write-Host "  $($plan.file) [$($plan.map)]: $(@($plan.timings).Count) timing changes verified"
    }
    # Commit only after every input version, section and compression check succeeds.
    foreach ($item in $ready) { Copy-Item -LiteralPath $item.Source -Destination $item.Target -Force }
    Write-Host "Japanese event timing: $($ready.Count) files updated."
} finally {
    # Only remove this run's direct temporary files; never recurse into caller paths.
    Get-ChildItem -LiteralPath $workDir -File | Remove-Item -Force
    Remove-Item -LiteralPath $workDir
}


