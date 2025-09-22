@echo off

pushd tools

set input_iso=base.iso
set target_iso=ffx_kr.iso
set target_patch=ffx_kr.xdelta
set font_path=font/font_kr.bmp
call build.bat

popd

pause
