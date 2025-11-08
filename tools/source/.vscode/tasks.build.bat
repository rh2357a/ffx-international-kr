@echo off
%MSYS2_HOME%\msys2_shell.cmd -defterm -here -no-start -mingw64 -c "make %1 DEBUG=1"
