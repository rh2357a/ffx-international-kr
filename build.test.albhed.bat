@echo off

setlocal enabledelayedexpansion

set files=build\files

set input_iso=base.iso
set target_iso=ffx_international_kr.iso
set target_patch=ffx_international_kr.xdelta

call scripts\build_init.bat

echo compile 'texts\event\file_01426.ev1.txt', 'texts\event\file_01426.ev1.en.txt'
tools\ffxcx -d !files!\file_01426.ev.lz1 !files!\file_01426.ev
del !files!\file_01426.ev.lz1
tools\ffxev -i1 -t tables\ko.tbs !files!\file_01426.ev texts\event\file_01426.ev1.txt
tools\ffxev -i4 -t tables\en.tbs !files!\file_01426.ev texts\event\file_01426.ev1.en.txt
tools\ffxcx -c1 !files!\file_01426.ev !files!\file_01426.ev.lz1
del !files!\file_01426.ev

call scripts\build_iso.bat

pause
