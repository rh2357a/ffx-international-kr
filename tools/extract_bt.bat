@echo off

pushd ..

setlocal enabledelayedexpansion

if not exist texts mkdir texts
if not exist texts\battle mkdir texts\battle

echo extract bt.lz1 files...
for %%i in (build\files\*.bt.lz1) do (
  set "filename=%%~ni"
  set "filename=!filename:.bt=!"
  set "target_dir=texts\battle"

  echo.  %%i
  tools\ffxcx -d %%i !target_dir!\!filename!
  tools\ffxbt -e4 -t tables\ja.tbs !target_dir!\!filename! !target_dir!\!filename!.bt1.txt
  del !target_dir!\!filename!
)

echo extract bt.lz2 files...
for %%i in (build\files\*.bt.lz2) do (
  set "filename=%%~ni"
  set "filename=!filename:.bt=!"
  set "target_dir=texts\battle"

  echo.  %%i
  tools\ffxcx -d %%i !target_dir!\!filename!
  tools\ffxbt -e4 -t tables\ja.tbs !target_dir!\!filename! !target_dir!\!filename!.bt2.txt
  del !target_dir!\!filename!
)

popd

pause
