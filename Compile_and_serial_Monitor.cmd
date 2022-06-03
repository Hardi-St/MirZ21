@echo off

REM To log the results to a file use Powershel (Ver 7.2) and enter
REM   Compile_and_serial_Monitor.cmd | TEE -Encoding ascii Messages.txt

if "%Python_Path_Added%"=="1" Goto PathSet
   ECHO Adding the Pyton path
   set Python_Path_Added=1
   Set PyPathStart=%LOCALAPPDATA%\Programs\Python
   REM Don't know why it's sometimes located in ..38 or in ..37
   if exist %PyPathStart%\Python39-32\Scripts\pio.exe Set Path=%PyPathStart%\Python39-32\Scripts;%path%
   if exist %PyPathStart%\Python38-32\Scripts\pio.exe Set Path=%PyPathStart%\Python38-32\Scripts;%path%
   if exist %PyPathStart%\Python37-32\Scripts\pio.exe Set Path=%PyPathStart%\Python37-32\Scripts;%path%
:PathSet


REM Default environment esp32dev or d1
if      "%1" == ""  SET Env=d1
if NOT  "%1" == ""  SET Env=%1

pio run -e %Env%       -t upload  %2 %3 %4 %5 %6 %7 %8 %9
IF errorlevel 1 Goto :eof


REM Filter options (See https://bit.ly/pio-monitor-filters)
REM Several comma separeted filters are possible
REM  - time       Enable the timestamp
REM  - log2file   Log the outputs to a file
REM  - printable  Es kommen leider immer noch Zeichen die MultiEdit nicht mag und darum auf HEX umschaltet

REM Execute in the Log directory to store the log files there
if Not Exist Log\. Mkdir Log
cd Log

REM Start the serial monitor (The ESP32 seames to have problems with 1000000 baud)
REM Old 500000 (The Excel serial Monitor seames to have problems with 500000)
pio device monitor -b 115200  --echo --filter log2file

cd ..
