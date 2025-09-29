@echo off

setlocal enabledelayedexpansion

set files=build\files

set input_iso=base.iso
set target_iso=ffx_international_kr.iso
set target_patch=ffx_international_kr.xdelta

call scripts\build_0.bat
call scripts\compile_etc_texts.bat
call scripts\compile_graphics.bat
call scripts\build_1.bat

pause
