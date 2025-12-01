echo mt1 files...
for %%i in (texts\menu\*.mt1.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.mt1=!"
  set "mt_name=!files!\!filename!.mt1"
  set "lz_name=!files!\!filename!.mt.lz1"

  echo.  %%i
  tools\ffxcx -d !lz_name! !mt_name!
  tools\ffxmt -i -t tables\ko.tbs !mt_name! %%i

  del !lz_name!
  tools\ffxcx -c1 !mt_name! !lz_name!
  del !mt_name!
)
