@echo off

setlocal enabledelayedexpansion

set files=build\files

set input_iso=base.iso
set target_iso=ffx_international_kr.iso
set target_patch=ffx_international_kr.xdelta

set multilang=1

call scripts\build_init.bat

rem backup file_00976.ev.lz1
copy !files!\file_00976.ev.lz1 !files!\file_00976.ev.lz1.bak >nul

call scripts\compile_event_texts.bat
call scripts\compile_menu_texts.bat
call scripts\compile_battle_texts.bat
call scripts\compile_etc_texts.bat
call scripts\compile_graphics.bat
call scripts\compile_multilang.bat
call scripts\build_iso.bat

pause
