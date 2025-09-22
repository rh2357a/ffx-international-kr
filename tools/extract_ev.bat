@echo off

pushd ..

setlocal enabledelayedexpansion

if not exist texts mkdir texts
if not exist texts\event mkdir texts\event

echo extract ev.lz1 files...
for %%i in (build\files\*.ev.lz1) do (
  set "filename=%%~ni"
  set "filename=!filename:.ev=!"

  echo.  %%i
  tools\ffxcx -d %%i texts\event\!filename!
  tools\ffxev -e1 -t tables\ja.tbs texts\event\!filename! texts\event\!filename!.ev1.ko.txt
  tools\ffxev -e4 -t tables\en.tbs texts\event\!filename! texts\event\!filename!.ev1.en.txt
  del texts\event\!filename!
)

echo extract ev.lz2 files...
for %%i in (build\files\*.ev.lz2) do (
  set "filename=%%~ni"
  set "filename=!filename:.ev=!"

  echo.  %%i
  tools\ffxcx -d %%i texts\event\!filename!
  tools\ffxev -e1 -t tables\ja.tbs texts\event\!filename! texts\event\!filename!.ev2.ko.txt
  tools\ffxev -e4 -t tables\en.tbs texts\event\!filename! texts\event\!filename!.ev2.en.txt
  del texts\event\!filename!
)

popd

pause
