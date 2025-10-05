echo etc files...

rem ============================================================

echo.  - 'texts\etc\file_00363.txt'
tools\ffxcx -d !files!\file_00363.bin.lz1 !files!\file_00363.temp.bin
tools\shsplit 4 -j -df tables\delim.txt -t tables\ko_etc_text.tbs -hb 0xbb110 -em 16 texts\etc\file_00363.txt !files!\file_00363.bin
tools\fcopy -Im !files!\file_00363.bin !files!\file_00363.temp.bin 0x0 0xbb110
tools\fcopy -idx !files!\file_00363.bin 0x10 "10b10b00"
tools\ffxcx -c1 !files!\file_00363.bin !files!\file_00363.bin.lz1
del !files!\file_00363.bin >nul
del !files!\file_00363.temp.bin >nul

echo.  - 'texts\etc\file_00366.txt'
tools\ffxcx -d !files!\file_00366.bin.lz1 !files!\file_00366.temp.bin
tools\shsplit 4 -j -df tables\delim.txt -t tables\ko_etc_text.tbs -hb 0xc88f0 -em 16 texts\etc\file_00366.txt !files!\file_00366.bin
tools\fcopy -Im !files!\file_00366.bin !files!\file_00366.temp.bin 0x0 0xc88f0
tools\fcopy -idx !files!\file_00366.bin 0x10 "f0880c00"
tools\ffxcx -c1 !files!\file_00366.bin !files!\file_00366.bin.lz1
del !files!\file_00366.bin >nul
del !files!\file_00366.temp.bin >nul

echo.  - 'texts\etc\file_00369.txt'
tools\ffxcx -d !files!\file_00369.bin.lz1 !files!\file_00369.temp.bin
tools\shsplit 4 -j -df tables\delim.txt -t tables\ko_etc_text.tbs -hb 0x1d840 -em 16 texts\etc\file_00369.txt !files!\file_00369.bin
tools\fcopy -Im !files!\file_00369.bin !files!\file_00369.temp.bin 0x0 0x1d840
tools\fcopy -idx !files!\file_00369.bin 0x10 "40d80100"
tools\ffxcx -c1 !files!\file_00369.bin !files!\file_00369.bin.lz1
del !files!\file_00369.bin >nul
del !files!\file_00369.temp.bin >nul

rem ============================================================

tools\ffxname -e file_00459 build\files

for %%i in (texts\etc\*.part.txt) do (
  set "filename=%%~ni"
  set "filename=!filename:.part=!"
  set "target_filename=!files!\file_00459.bin"
  set "part_filename=!files!\!filename!.bin"

  echo.  - '%%i'
  tools\ffxdlg2 -i -t tables\ko.tbs !part_filename! %%i
)

tools\ffxname -i file_00459 build\files
tools\ffxname -c file_00459 build\files

rem ============================================================

echo.  - 'texts\etc\file_00468.txt'
tools\ffxdlg -i -t tables\ko.tbs !files!\file_00468.bin texts\etc\file_00468.txt
