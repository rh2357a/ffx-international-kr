@echo off

setlocal enabledelayedexpansion

set build_dir=build
set build_target_dir=!build_dir!\jp_voice
set files=!build_target_dir!\files

set input_iso=base.iso
set target_iso=ffx_international_kr.jp_voice.iso
set target_patch=ffx_international_kr.jp_voice.xdelta

set target_name=jp_voice
set multilang=0

call scripts\build_init.bat
call scripts\compile_event_texts_jp_voice.bat
if errorlevel 1 exit /b 1
call scripts\compile_menu_texts.bat
call scripts\compile_battle_texts.bat
call scripts\compile_etc_texts.bat
call scripts\compile_graphics.bat
call scripts\build_iso.bat

pause
