echo graphics...

echo.  - ui gfx
tools\ffxgnop -c graphics\battle\file_00465.bmp !files!\file_00465.bin
tools\ffxgnop -c graphics\menu\file_00470.bmp   !files!\file_00470.bin

echo.  - monster arena menu
tools\ffxcx -d %files%\file_00367.bin.lz1 !files!\file_00367.bin1
tools\ffxgnop -i 0x80c50 !files!\file_00367.bin1 graphics\menu\file_00367_0.bmp
tools\ffxgnop -i 0x91050 !files!\file_00367.bin1 graphics\menu\file_00367_1.bmp
tools\ffxcx -c1 !files!\file_00367.bin1 %files%\file_00367.bin.lz1

echo.  - copyright
tools\ffxcx -d !files!\file_11695.map.lz1 !files!\file_11695.map1
tools\ffxmap -ip0 !files!\file_11695.map1 graphics\misc\file_11695.bmp !files!\file_11695.map1
tools\ffxmap -ip4 !files!\file_11695.map1 graphics\misc\file_11695.bmp !files!\file_11695.map1
tools\ffxcx -c1 !files!\file_11695.map1 !files!\file_11695.map.lz1
del !files!\file_11695.map1

echo.  - title
tools\ffxcx -d !files!\file_11691.map.lz1 !files!\file_11691.map1
tools\ffxmap -ip0 !files!\file_11691.map1 graphics\misc\file_11691.bmp !files!\file_11691.map1
tools\ffxcx -c1 !files!\file_11691.map1 !files!\file_11691.map.lz1
del !files!\file_11691.map1

echo.  - misc jpn logo
tools\ffxcx -d !files!\file_09832.bin.lz1 !files!\file_09832.bin1
tools\ffxgnop -i 0xb2000 !files!\file_09832.bin1 graphics\misc\file_09832.bmp
tools\ffxcx -c1 !files!\file_09832.bin1 !files!\file_09832.bin.lz1
del !files!\file_09832.bin1

echo.  - airship text gfx
set airship_cache=build\airship
if exist "!airship_cache!" rmdir /s /q "!airship_cache!"
if not exist "!airship_cache!" mkdir "!airship_cache!"
copy /Y graphics\airship\* !airship_cache!\ >nul
for /L %%i in (0,1,26) do (
    set "num=0%%i"
    set "xx=!num:~-2!"
    for /L %%j in (12,1,25) do (
        copy /Y "!airship_cache!\file_11113_11_!xx!.bmp" "!airship_cache!\file_11113_%%j_!xx!.bmp" >nul
    )
)
tools\ffxcx -d !files!\file_11113.map.lz1 !files!\file_11113.map
tools\ffxmap -i !files!\file_11113.map !airship_cache!\file_11113_ !files!\file_11113.map
tools\ffxcx -c1 !files!\file_11113.map !files!\file_11113.map.lz1
del !files!\file_11113.map
if exist "!airship_cache!" rmdir /s /q "!airship_cache!"

echo.  - help gfx
for %%i in (graphics\help\*.gwh1.bmp) do (
  set "filename=%%~ni"
  set "filename=!filename:.gwh1=!"
  tools\ffxcx -d !files!\!filename!.bin.lz1 !files!\!filename!.bin
  del !files!\!filename!.bin.lz1
  tools\ffxgwh -i !files!\!filename!.bin graphics\help\!filename!.gwh1.bmp !files!\!filename!.bin
  tools\ffxcx -c1 !files!\!filename!.bin !files!\!filename!.bin.lz1
  del !files!\!filename!.bin
)

for %%i in (graphics\help\*.gwh2.bmp) do (
  set "filename=%%~ni"
  set "filename=!filename:.gwh2=!"
  tools\ffxcx -d !files!\!filename!.bin.lz2 !files!\!filename!.bin
  del !files!\!filename!.bin.lz2
  tools\ffxgwh -i !files!\!filename!.bin graphics\help\!filename!.gwh2.bmp !files!\!filename!.bin
  tools\ffxcx -c2 !files!\!filename!.bin !files!\!filename!.bin.lz2
  del !files!\!filename!.bin
)
