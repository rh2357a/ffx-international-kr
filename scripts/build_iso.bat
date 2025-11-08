echo repack '%target_iso%'
if exist %target_iso% del %target_iso% >nul
tools\ffxiso -i !build_target_dir! %target_iso%
