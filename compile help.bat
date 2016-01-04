@echo off
set path=D:\mingw\mingw32\bin\;%PATH%
hhc "src\osd\winui\help\mameuifx.hhp"
copy "src\osd\winui\help\help.chm" .
pause

