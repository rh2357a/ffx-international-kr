param(
    [Parameter(Mandatory=$true)][string]$FilesPath,
    [string]$ManifestPath = '',
    [switch]$Check
)
# Recalculate fixed-width wait values and hashes from existing relocation plans.
# Adding/removing edits or changing offsets requires a new relocation plan.
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
function Hash([byte[]]$Bytes) {
    $sha = [Security.Cryptography.SHA256]::Create()
    try { return ([BitConverter]::ToString($sha.ComputeHash($Bytes))).Replace('-','').ToLowerInvariant() }
    finally { $sha.Dispose() }
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
try {
    $ready = @()
    foreach ($planFile in $plans) {
        $plan = Get-Content -LiteralPath $planFile.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
        if ($plan.schema -ne 1 -or $plan.file -notmatch '^file_[0-9]{5}\.ev\.lz[12]$') { throw "Invalid manifest: $($planFile.Name)" }
        $unpacked = Join-Path $workDir 'event.ev'
        $section = Join-Path $workDir 'script.bin'
        Run-Tool $cx @('-d',(Join-Path $filesRoot $plan.file),$unpacked)
        Run-Tool $ev @('-e0',$unpacked,$section)
        $original = [IO.File]::ReadAllBytes($section)
        if ((Hash $original) -ne $plan.original_sha256) { throw "Original script required: $($plan.file)" }
        # Validate the existing plan before using its fixed-width relocation data.
        if ((Hash (Apply-Operations $original $plan.operations)) -ne $plan.patched_sha256) {
            throw "Existing operations/hash mismatch: $($plan.file)"
        }
        $timingOps = @($plan.operations | Where-Object { $_.reason -in @('wait_value','remove_int_wait','insert_jp_wait') })
        if ($timingOps.Count -ne @($plan.timings).Count) { throw 'Timing layout changed; a new relocation plan is required' }
        $seen = @{}
        foreach ($edit in $plan.timings) {
            if (($edit.new -isnot [int] -and $edit.new -isnot [long]) -or $edit.new -lt 0 -or $edit.new -gt 32767) {
                throw 'Frame count must be an integer in 0..32767'
            }
            $matches = @($timingOps | Where-Object { $_.offset -eq $edit.offset -and $_.reason -eq $edit.kind })
            if ($matches.Count -ne 1 -or $seen.ContainsKey([int]$edit.offset)) { throw 'Timing layout changed; a new relocation plan is required' }
            $seen[[int]$edit.offset] = $true
            $op = $matches[0]
            if ($edit.kind -ne 'insert_jp_wait' -and ($op.before -ne $edit.before -or $op.after -ne $edit.after)) { throw 'Edit only timings[].new; other timing fields must match operations' }
            $valueHex = ([BitConverter]::ToString([BitConverter]::GetBytes([uint16]$edit.new))).Replace('-','').ToLowerInvariant()
            switch ($edit.kind) {
                'wait_value' {
                    if ($op.after -notmatch '^ae[0-9a-f]{4}$') { throw 'Invalid wait instruction' }
                    $after = 'ae'+$valueHex
                }
                'insert_jp_wait' {
                    if ($op.before -ne '' -or $op.after -notmatch '^ae[0-9a-f]{4}d80000$') { throw 'Invalid inserted wait' }
                    $after = 'ae'+$valueHex+'d80000'
                }
                'remove_int_wait' {
                    if ($edit.new -ne 0 -or $op.after -ne '000000000000') { throw 'Removed waits must remain zero' }
                    $after = $op.after
                }
                default { throw "Unsupported timing kind: $($edit.kind)" }
            }
            # Instruction widths are unchanged, so every relocated address stays valid.
            $op.after = $after
            if ($edit.kind -ne 'insert_jp_wait') { $edit.after = $after }
        }
        $newHash = Hash (Apply-Operations $original $plan.operations)
        if ($Check -and $newHash -ne $plan.patched_sha256) { throw "Pending timing changes: $($plan.file)" }
        if ($newHash -ne $plan.patched_sha256) {
            $plan.patched_sha256 = $newHash
            $ready += [pscustomobject]@{ Path=$planFile.FullName; Text=($plan | ConvertTo-Json -Depth 30)+[Environment]::NewLine }
        }
    }
    # No manifest is written until all input files and existing plans are verified.
    foreach ($item in $ready) { [IO.File]::WriteAllText($item.Path,$item.Text,(New-Object Text.UTF8Encoding $false)) }
    Write-Host "Event timing manifests: $($plans.Count) verified, $($ready.Count) updated."
} finally {
    Get-ChildItem -LiteralPath $workDir -File | Remove-Item -Force
    Remove-Item -LiteralPath $workDir
}
