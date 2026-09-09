param(
    [Parameter(Mandatory=$true)][string]$FilesPath,
    [string]$ManifestPath = ''
)
# Validate timing values and relocation plans against original event scripts.
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
if (-not $ManifestPath) { $ManifestPath = Join-Path $projectRoot 'texts/event_script' }
$filesRoot = (Resolve-Path -LiteralPath $FilesPath).Path
$plans = @(Get-ChildItem -LiteralPath $ManifestPath -Filter 'file_*.json' | Sort-Object Name)
if (-not $plans.Count) { throw 'No event timing manifests found' }
$cx = Join-Path $projectRoot 'tools/ffxcx.exe'
$ev = Join-Path $projectRoot 'tools/ffxev.exe'
$workDir = Join-Path ([IO.Path]::GetTempPath()) ('ffx-event-plans-' + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $workDir | Out-Null
function Run-Tool([string]$Tool, [string[]]$ToolArgs) {
    & $Tool @ToolArgs | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "Tool failed: $Tool $ToolArgs" }
}
function Hex-Bytes([string]$Hex) {
    if ($Hex -notmatch '^(?:[0-9a-fA-F]{2})*$') { throw 'Invalid hex byte string' }
    $result = New-Object byte[] ($Hex.Length / 2)
    for ($i=0; $i -lt $result.Length; $i++) { $result[$i] = [Convert]::ToByte($Hex.Substring($i*2,2),16) }
    return ,$result
}
function Apply-Operations([byte[]]$Original, $Operations) {
    $stream = New-Object IO.MemoryStream
    try {
        $cursor = 0
        foreach ($op in $Operations) {
            $offset = [int]$op.offset
            [byte[]]$old = Hex-Bytes $op.before
            [byte[]]$new = Hex-Bytes $op.after
            if ($offset -lt $cursor -or $offset+$old.Length -gt $Original.Length) { throw 'Overlapping/out-of-bounds edit' }
            for ($i=0; $i -lt $old.Length; $i++) {
                if ($Original[$offset+$i] -ne $old[$i]) { throw "Original bytes differ at $offset" }
            }
            $stream.Write($Original,$cursor,$offset-$cursor)
            $stream.Write($new,0,$new.Length)
            $cursor = $offset+$old.Length
        }
        $stream.Write($Original,$cursor,$Original.Length-$cursor)
        return ,$stream.ToArray()
    } finally { $stream.Dispose() }
}
function Change-Operations([byte[]]$Original, $Plan) {
    $timingKinds = @('wait_value','remove_int_wait','insert_jp_wait','fmv_progress_value')
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
try {
    foreach ($planFile in $plans) {
        $plan = Get-Content -LiteralPath $planFile.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
        if ($plan.schema -ne 2 -or $plan.file -notmatch '^file_[0-9]{5}\.ev\.lz[12]$') { throw "Invalid manifest: $($planFile.Name)" }
        $unpacked = Join-Path $workDir 'event.ev'
        $section = Join-Path $workDir 'script.bin'
        Run-Tool $cx @('-d',(Join-Path $filesRoot $plan.file),$unpacked)
        Run-Tool $ev @('-e0',$unpacked,$section)
        $original = [IO.File]::ReadAllBytes($section)
        $operations = @(Change-Operations $original $plan)
        [void](Apply-Operations $original $operations)
    }
    Write-Host "Event timing manifests: $($plans.Count) verified."
} finally {
    Get-ChildItem -LiteralPath $workDir -File | Remove-Item -Force
    Remove-Item -LiteralPath $workDir
}
