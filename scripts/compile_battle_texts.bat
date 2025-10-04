echo bt1 files...
for %%i in (texts\battle\*.bt1.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.bt1=!"
  set "bt_name=!files!\!filename!.bt1"
  set "lz_name=!files!\!filename!.bt.lz1"

  echo.  %%i
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! %%i

  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

for %%i in (texts\battle\*.bt1.en.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.bt1.en=!"
  set "bt_name=!files!\!filename!.bt1"
  set "lz_name=!files!\!filename!.bt.lz1"

  echo.  %%i
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\en.tbs !bt_name! %%i

  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

echo bt2 files...
for %%i in (texts\battle\*.bt2.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.bt2=!"
  set "bt_name=!files!\!filename!.bt2"
  set "lz_name=!files!\!filename!.bt.lz2"

  echo.  %%i
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! %%i

  del !lz_name!
  tools\ffxcx -c2 !bt_name! !lz_name!
  del !bt_name!
)

for %%i in (texts\battle\*.bt2.en.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.bt2.en=!"
  set "bt_name=!files!\!filename!.bt2"
  set "lz_name=!files!\!filename!.bt.lz2"

  echo.  %%i
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\en.tbs !bt_name! %%i

  del !lz_name!
  tools\ffxcx -c2 !bt_name! !lz_name!
  del !bt_name!
)

rem ============================================================

echo bts files...
for %%i in (texts\battle2\*.bts.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.bts=!"
  set "bt_name=!files!\!filename!.bts"

  echo.  %%i
  tools\ffxbts -i2 -w -t tables\ko.tbs !bt_name! %%i
)
