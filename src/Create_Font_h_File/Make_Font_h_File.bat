@echo off
REM Create temp directory
mkdir \tmp 2> NUL

REM Create the font
REM
REM The fonts are manualy generated with Fony.exe by Michael and Jürgen
REM
bdfconv -v -f 1 -m "0-255"  Fkt_Icons.bdf -o ..\font_Fkt_Icons.bdf.h  -n font_Fkt_Icons
bdfconv -v -f 1 -m "65-85"  Stummilok.bdf -o ..\font_Stummilok.bdf.h  -n font_Stummilok
bdfconv -v -f 1 -m "65-85"  Weichen.bdf   -o ..\font_Weichen.bdf.h    -n font_Weichen
bdfconv -v -f 1 -m "65-92"  Haus.bdf      -o ..\font_Haus.bdf.h       -n font_Haus


Pause




