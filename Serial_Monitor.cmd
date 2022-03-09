@echo off

SET Baud=%1
if "%1" =="" SET Baud=115200
REM 500000

REM To log the results to a file use Powershel (Ver 7.2) and enter
REM   Serial_Monitor.cmd | TEE -Encoding ascii Messages.txt
REM
REM To show timestamps enter
REM  ctrl+t ctrl+f time

if "%Python_Path_Added%"=="1" Goto PathSet
   ECHO Adding the Pyton path
   set Python_Path_Added=1
   set path=%APPDATA%\Python\python38-32\scripts;%path%
:PathSet


REM Filter options (See https://bit.ly/pio-monitor-filters)
REM Several comma separeted filters are possible
REM  - time       Enable the timestamp
REM  - log2file   Log the outputs to a file
REM  - printable  Es kommen leider immer noch Zeichen die MultiEdit nicht mag und darum auf HEX umschaltet

REM Execute in the Log directory to store the log files there
if Not Exist Log\. Mkdir Log
cd Log

REM Start the serial monitor
pio device monitor -b %Baud% --echo --filter log2file
REM pio device monitor -b 115200 --echo --filter log2file

cd ..
