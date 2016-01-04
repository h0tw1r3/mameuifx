@echo off
set PATH=D:\mingw\mingw32\bin;%PATH%
set MINGW32=D:\mingw\mingw32
make -j5 OSD=winui PTR64=0
echo Job Done!
pause
