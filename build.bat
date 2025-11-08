@echo off

setlocal enabledelayedexpansion

set target_name=korean

set build_dir=build
set build_target_dir=!build_dir!\!target_name!
set files=!build_target_dir!\files

set input_iso=base.iso
set target_iso=ffx_international_kr.iso

set multilang=0

call scripts\build_init.bat
call scripts\compile_event_texts.bat
call scripts\compile_menu_texts.bat
call scripts\compile_battle_texts.bat
call scripts\compile_etc_texts.bat
call scripts\compile_graphics.bat
call scripts\build_iso.bat

pause
