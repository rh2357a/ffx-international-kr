@echo off

setlocal enabledelayedexpansion

set files=build\files

set input_iso=base.iso
set target_iso=ffx_international_kr.iso
set target_patch=ffx_international_kr.xdelta

set multilang=0

echo extract '%input_iso%'
if exist build rmdir /s /q build
tools\ffxiso -e %input_iso% build

echo apply patches...
for %%i in (patch\*.xdelta) do (
  set "filename=%%~ni"
  set "origin_filename=!files!\!filename!"
  set "patch_filename=!files!\!filename!.tmp"

  echo.  %%i
  tools\xdelta3 -d -s !origin_filename! %%i !patch_filename!
  move /Y !patch_filename! !origin_filename! >nul
)

echo assemble 'asm\ffx_international.jpn.asm'...
tools\armips asm\ffx_international.jpn.asm

call scripts\build_iso.bat

pause
