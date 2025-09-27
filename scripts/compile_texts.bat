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

for %%i in (texts\event\*.ev1.en.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.ev1.en=!"
  set "ev_name=!files!\!filename!.ev1"
  set "lz_name=!files!\!filename!.ev.lz1"

  echo.  %%i
  tools\ffxcx -d !lz_name! !ev_name!
  tools\ffxev -i4 -t tables\en.tbs !ev_name! %%i

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

for %%i in (texts\event\*.ev2.en.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.ev2.en=!"
  set "ev_name=!files!\!filename!.ev2"
  set "lz_name=!files!\!filename!.ev.lz2"

  echo.  %%i
  tools\ffxcx -d !lz_name! !ev_name!
  tools\ffxev -i4 -t tables\en.tbs !ev_name! %%i

  del !lz_name!
  tools\ffxcx -c2 !ev_name! !lz_name!
  del !ev_name!
)

rem ============================================================

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

rem ============================================================
echo etc text files...
for %%i in (texts\etc\*.txt) do (
  set "filename=%%~ni"
  set "target_filename=!files!\!filename!.bin"

  echo.  %%i
  tools\ffxdlg -i -t tables\ko.tbs !target_filename! %%i
)

rem ============================================================

echo name files...
tools\ffxname -e file_00459 build\files

for %%i in (texts\name\*.txt) do (
  set "filename=%%~ni"
  set "target_filename=!files!\file_00459.bin"
  set "part_filename=!files!\!filename!.bin"

  echo.  %%i
  tools\ffxdlg2 -i -t tables\ko.tbs !part_filename! %%i
)

tools\ffxname -i file_00459 build\files
tools\ffxname -c file_00459 build\files

rem ============================================================

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

for %%i in (texts\menu\*.mt1.en.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.mt1.en=!"
  set "mt_name=!files!\!filename!.mt1"
  set "lz_name=!files!\!filename!.mt.lz1"

  echo.  %%i
  tools\ffxcx -d !lz_name! !mt_name!
  tools\ffxmt -i -t tables\en.tbs !mt_name! %%i

  del !lz_name!
  tools\ffxcx -c1 !mt_name! !lz_name!
  del !mt_name!
)
