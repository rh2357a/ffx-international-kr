echo repack '%target_iso%'
if exist %target_iso% del %target_iso% >nul
tools\ffxiso -i !build_target_dir! %target_iso%

echo create '%target_patch%'
if exist %target_patch% del %target_patch% >nul
tools\xdelta3 -e -f -A=. -n -S none -s !input_iso! !target_iso! !target_patch!
