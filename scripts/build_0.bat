rem              0x10000
set inject_length=65536

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
tools\ffxftcx graphics\font\font_kr.bmp !inject_length! !files!\file_00455.ftcx

echo assemble 'asm\ffx_international.asm'...
tools\armips -equ INJECT_LENGTH !inject_length! asm\ffx_international.asm
