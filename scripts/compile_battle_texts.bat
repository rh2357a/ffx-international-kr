echo bt1 files...
for %%i in (texts\battle\*.bt1.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.bt1=!"
  set "bt_name=!files!\!filename!.bt1"
  set "lz_name=!files!\!filename!.bt.lz1"

  echo.  - '%%i'
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

  echo.  - '%%i'
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

  echo.  - '%%i'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! %%i

  del !lz_name!
  tools\ffxcx -c2 !bt_name! !lz_name!
  del !bt_name!
)

rem ============================================================

echo bt list files...

echo.  - 'texts\battle2\file_08094.list.txt'
for /f %%a in (texts\battle2\file_08094.list.txt) do (
  set "origin_name=file_08094.bt1.txt"
  set "bt_name=!files!\%%a.bt"
  set "lz_name=!files!\%%a.bt.lz1"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

echo.  - 'texts\battle2\file_07944.list.txt'
for /f %%a in (texts\battle2\file_07944.list.txt) do (
  set "origin_name=file_07944.bt1.txt"
  set "bt_name=!files!\%%a.bt"
  set "lz_name=!files!\%%a.bt.lz1"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

echo.  - 'texts\battle2\file_08147.list.txt'
for /f %%a in (texts\battle2\file_08147.list.txt) do (
  set "origin_name=file_08147.bt1.txt"
  set "bt_name=!files!\%%a.bt"
  set "lz_name=!files!\%%a.bt.lz1"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

echo.  - 'texts\battle2\file_08065.list.txt'
for /f %%a in (texts\battle2\file_08065.list.txt) do (
  set "origin_name=file_08065.bt1.txt"
  set "bt_name=!files!\%%a.bt1"
  set "lz_name=!files!\%%a.bt.lz1"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

echo.  - 'texts\battle2\file_08312.list.txt'
for /f %%a in (texts\battle2\file_08312.list.txt) do (
  set "origin_name=file_08312.bt2.txt"
  set "bt_name=!files!\%%a.bt"
  set "lz_name=!files!\%%a.bt.lz2"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c2 !bt_name! !lz_name!
  del !bt_name!
)

echo.  - 'texts\battle2\file_08208.list.txt'
for /f %%a in (texts\battle2\file_08208.list.txt) do (
  set "origin_name=file_08208.bt1.txt"
  set "bt_name=!files!\%%a.bt"
  set "lz_name=!files!\%%a.bt.lz1"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

echo.  - 'texts\battle2\file_08392.list.txt'
for /f %%a in (texts\battle2\file_08392.list.txt) do (
  set "origin_name=file_08392.bt1.txt"
  set "bt_name=!files!\%%a.bt"
  set "lz_name=!files!\%%a.bt.lz1"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

echo.  - 'texts\battle2\file_07990.list.txt'
for /f %%a in (texts\battle2\file_07990.list.txt) do (
  set "origin_name=file_07990.bt1.txt"
  set "bt_name=!files!\%%a.bt"
  set "lz_name=!files!\%%a.bt.lz1"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

echo.  - 'texts\battle2\file_08263.list.txt'
for /f %%a in (texts\battle2\file_08263.list.txt) do (
  set "origin_name=file_08263.bt1.txt"
  set "bt_name=!files!\%%a.bt"
  set "lz_name=!files!\%%a.bt.lz1"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

echo.  - 'texts\battle2\file_08348.list.txt'
for /f %%a in (texts\battle2\file_08348.list.txt) do (
  set "origin_name=file_08348.bt1.txt"
  set "bt_name=!files!\%%a.bt"
  set "lz_name=!files!\%%a.bt.lz1"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

echo.  - 'texts\battle2\file_07956.list.txt'
for /f %%a in (texts\battle2\file_07956.list.txt) do (
  set "origin_name=file_07956.bt1.txt"
  set "bt_name=!files!\%%a.bt"
  set "lz_name=!files!\%%a.bt.lz1"
  echo.    * '%%a'
  tools\ffxcx -d !lz_name! !bt_name!
  tools\ffxbt -i4 -t tables\ko.tbs !bt_name! texts\battle2\!origin_name!
  del !lz_name!
  tools\ffxcx -c1 !bt_name! !lz_name!
  del !bt_name!
)

rem ============================================================

echo bts files...
for %%i in (texts\battle3\*.bts.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.bts=!"
  set "bt_name=!files!\!filename!.bts"

  echo.  - '%%i'
  tools\ffxbts -i2 -w -t tables\ko.tbs !bt_name! %%i
)
