echo extract '%input_iso%'
if exist !build_target_dir! rmdir /s /q !build_target_dir!
tools\ffxiso -e %input_iso% !build_target_dir!

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
tools\ffxftcx graphics\font\font_kr_v2.bmp !build_dir!\font file_00455
move /Y !build_dir!\font\file_00455.ftcx !files! >nul

echo assemble 'asm\ffx_international.asm'...
tools\armips -strequ TARGET_NAME "!target_name!" -equ MULTILANG !multilang! asm\ffx_international.asm
