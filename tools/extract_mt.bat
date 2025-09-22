@echo off

pushd ..

setlocal enabledelayedexpansion

if not exist texts mkdir texts
if not exist texts\menu mkdir texts\menu

echo extract mt.lz1 files...
for %%i in (build\files\*.mt.lz1) do (
  set "filename=%%~ni"
  set "filename=!filename:.mt=!"
  set "target_dir=texts\menu"

  echo.  %%i
  tools\ffxcx -d %%i !target_dir!\!filename!
  tools\ffxmt -e -t tables\ja.tbs !target_dir!\!filename! !target_dir!\!filename!.mt1.txt
  del !target_dir!\!filename!
)

echo extract mt.lz2 files...
for %%i in (build\files\*.mt.lz2) do (
  set "filename=%%~ni"
  set "filename=!filename:.mt=!"
  set "target_dir=texts\menu"

  echo.  %%i
  tools\ffxcx -d %%i !target_dir!\!filename!
  tools\ffxmt -e -t tables\ja.tbs !target_dir!\!filename! !target_dir!\!filename!.mt2.txt
  del !target_dir!\!filename!
)

popd

pause
