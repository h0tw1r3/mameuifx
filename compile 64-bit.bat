@echo off
set PATH=D:\mingw\mingw64\bin;%PATH%
set MINGW64=D:\mingw\mingw64
make -j5 OSD=winui PTR64=1
echo Job Done!
pause
