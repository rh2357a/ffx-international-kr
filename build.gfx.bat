@echo off

setlocal enabledelayedexpansion

set files=build\files

set input_iso=base.iso
set target_iso=ffx_international_kr.iso
set target_patch=ffx_international_kr.xdelta

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

echo build font...
rem                            0x1000
tools\ffxftcx graphics\font\font_kr.bmp  4096  !files!\file_00455.ftcx

echo asm files...
for %%i in (asm\*.asm) do (
  echo.  %%i
  tools\armips %%i
)

call scripts\compile_graphics.bat

echo repack '%target_iso%'
if exist %target_iso% del %target_iso% >nul
tools\ffxiso -i build %target_iso%

echo create '%target_patch%'
if exist %target_patch% del %target_patch% >nul
tools\xdelta3 -e -s %input_iso% %target_iso% %target_patch%

pause
