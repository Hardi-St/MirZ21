@echo off
REM Create temp directory
mkdir \tmp 2> NUL

REM Create the font
REM
REM The fonts are manualy generated with Fony.exe by Michael and Jürgen
REM
bdfconv -v -f 1 -m "0-255"  Fkt_Icons.bdf  -o ..\font_Fkt_Icons.bdf.h  -n font_Fkt_Icons
bdfconv -v -f 1 -m "65-85"  Stummilok.bdf  -o ..\font_Stummilok.bdf.h  -n font_Stummilok
bdfconv -v -f 1 -m "65-92"  Stummilok2.bdf -o ..\font_Stummilok2.bdf.h -n font_Stummilok2
bdfconv -v -f 1 -m "65-103" Stummilok3.bdf -o ..\font_Stummilok3.bdf.h -n font_Stummilok3
bdfconv -v -f 1 -m "65-85"  Weichen.bdf    -o ..\font_Weichen.bdf.h    -n font_Weichen
bdfconv -v -f 1 -m "65-92"  Haus.bdf       -o ..\font_Haus.bdf.h       -n font_Haus
bdfconv -v -f 1 -m "65-124" Haus1.bdf      -o ..\font_Haus1.bdf.h      -n font_Haus1
bdfconv -v -f 1 -m "65-122" Haus2.bdf      -o ..\font_Haus2.bdf.h      -n font_Haus2

Pause




