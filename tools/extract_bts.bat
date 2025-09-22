@echo off

pushd ..

setlocal enabledelayedexpansion

if not exist texts mkdir texts
if not exist texts\battle2 mkdir texts\battle2

echo extract bts files...
for %%i in (build\files\*.bts) do (
  set "filename=%%~ni"
  set "target_dir=texts\battle2"

  echo.  %%i
  copy %%i !target_dir!\!filename! >nul
  tools\ffxbts -e2 -w -t tables\ja.tbs !target_dir!\!filename! !target_dir!\!filename!.bts.txt
  del !target_dir!\!filename!
)

popd

pause
