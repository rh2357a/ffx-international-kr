echo multilang patch...

echo.  - file_00976.ev.lz1
tools\xdelta3 -d -s !files!\file_00976.ev.lz1.bak multilang\patch\file_00976.ev.lz1.xdelta !files!\file_00976.ev.lz1.tmp
move /Y !files!\file_00976.ev.lz1.tmp !files!\file_00976.ev.lz1 >nul

rem =====================================================

echo compile multilang event texts...

echo.  - file_00976.ev1.txt, file_00976.ev1.en.txt
for %%_ in (1) do (
  set "filename=multilang\texts\event\file_00976.ev1.txt"
  set "filename2=multilang\texts\event\file_00976.ev1.en.txt"
  set "bin_name=!files!\file_00976.ev1"
  set "lz_name=!files!\file_00976.ev.lz1"

  tools\ffxcx -d !lz_name! !bin_name!
  tools\ffxev -i1 -t tables\ko.tbs !bin_name! !filename!
  tools\ffxev -i4 -t tables\en.tbs !bin_name! !filename2!

  del !lz_name!
  tools\ffxcx -c1 !bin_name! !lz_name!
  del !bin_name!
)

rem =====================================================

echo compile multilang texts...

echo.  - file_07799.mt1.en.txt
for %%_ in (1) do (
  set "filename=multilang\texts\menu\file_07799.mt1.en.txt"
  set "mt_name=!files!\file_07799.mt1"
  set "lz_name=!files!\file_07799.mt.lz1"

  tools\ffxcx -d !lz_name! !mt_name!
  tools\ffxmt -i -t tables\en.tbs !mt_name! !filename!

  del !lz_name!
  tools\ffxcx -c1 !mt_name! !lz_name!
  del !mt_name!
)

echo.  - file_07928.mt1.txt
for %%_ in (1) do (
  set "filename=multilang\texts\menu\file_07928.mt1.txt"
  set "mt_name=!files!\file_07928.mt1"
  set "lz_name=!files!\file_07928.mt.lz1"

  tools\ffxcx -d !lz_name! !mt_name!
  tools\ffxmt -i -t tables\ko.tbs !mt_name! !filename!

  del !lz_name!
  tools\ffxcx -c1 !mt_name! !lz_name!
  del !mt_name!
)

rem =====================================================

echo compile multilang file_00459 texts...

tools\ffxname -e file_00459 build\files

echo.  - file_00459.06.part.txt
tools\ffxdlg2 -i -t tables\ko.tbs !files!\file_00459.06.bin multilang\texts\etc\file_00459.06.part.txt
tools\ffxname -i file_00459 build\files

rem =====================================================

echo compile multilang graphics...

echo.  - file_00379.gwh2.bmp
for %%_ in (1) do (
  set "filename=multilang\graphics\help\file_00379.gwh2.bmp"
  set "bin_name=!files!\file_00379.bin"
  set "lz_name=!files!\file_00379.bin.lz2"

  del !lz_name!
  tools\ffxgwh -c !filename! !bin_name!
  tools\ffxcx -c2 !bin_name! !lz_name!
  del !bin_name!
)
