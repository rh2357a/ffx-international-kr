echo ev1 files...
for %%i in (texts\event\*.ev1.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.ev1=!"
  set "ev_name=!files!\!filename!.ev1"
  set "lz_name=!files!\!filename!.ev.lz1"

  echo.  %%i
  tools\ffxcx -d !lz_name! !ev_name!
  tools\ffxev -i1 -t tables\ko.tbs !ev_name! %%i

  del !lz_name!
  tools\ffxcx -c1 !ev_name! !lz_name!
  del !ev_name!
)

echo ev2 files...
for %%i in (texts\event\*.ev2.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.ev2=!"
  set "ev_name=!files!\!filename!.ev2"
  set "lz_name=!files!\!filename!.ev.lz2"

  echo.  %%i
  tools\ffxcx -d !lz_name! !ev_name!
  tools\ffxev -i1 -t tables\ko.tbs !ev_name! %%i

  del !lz_name!
  tools\ffxcx -c2 !ev_name! !lz_name!
  del !ev_name!
)
