@echo off

setlocal enabledelayedexpansion

REM call build.bat
REM call build.multilang.bat

set korean_iso=ffx_international_kr.iso
set multilang_iso=ffx_international_kr.multilang.iso
set source_dir=tools\source\src\ffx_international_kr\assets

echo build xdelta patch
tools\ffxppkg base.iso !korean_iso! !multilang_iso! !source_dir!

pause
