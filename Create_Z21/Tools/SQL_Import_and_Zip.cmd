@ECHO OFF
REM Import the vehicles.csv and functions.cvf file to the database
REM
REM Links:
REM https://sqlite.org/cli.html

REM If changed the program "..\Create_Z21.xlsm" must also be adapted
SET ZipProg="C:\Program Files\7-Zip\7z.exe"


IF NOT Exist Temp (
  MKDIR Temp
  MKDIR Temp\export
  MKDIR Temp\export\Data
  )

if exist Temp\export\Data\Loco.sqlite DEL Temp\export\Data\Loco.sqlite

SET ICONV=Tools\iconv.exe
%ICONV% -f CP1252 -t UTF-8 Temp\vehicles.csv  > Temp\vehicles_UTF8.csv
%ICONV% -f CP1252 -t UTF-8 Temp\functions.csv > Temp\functions_UTF8.csv

REM Usefull commands:
REM ~~~~~~~~~~~~~~~~~
REM Print a table
REM  select * from vehicles;



ECHO # define the separator                                                    >Temp\Import.sql
ECHO .separator ","                                                           >>Temp\Import.sql

ECHO # Delete the tabelle                                                     >>Temp\Import.sql
ECHO delete from vehicles;                                                    >>Temp\Import.sql

ECHO # Import csv file                                                        >>Temp\Import.sql
ECHO .import --csv --skip 1 -v  Temp/vehicles_UTF8.csv vehicles               >>Temp\Import.sql


ECHO # Delete the tabelle                                                     >>Temp\Import.sql
ECHO delete from functions;                                                   >>Temp\Import.sql

ECHO # Import csv file                                                        >>Temp\Import.sql
ECHO .import --csv --skip 1 -v  Temp/functions_UTF8.csv functions             >>Temp\Import.sql


ECHO .save Temp/export/Data/Loco.sqlite                                       >>Temp\Import.sql
ECHO .quit                                                                    >>Temp\Import.sql

ECHO.
copy Vorlagen\Loco.sqlite Temp\Loco.sqlite
Tools\sqlite3.exe Temp\Loco.sqlite < Temp\Import.sql

if NOT exist Temp\export\Data\Loco.sqlite (
   ECHO ERROR: Loco.sqlite not generated
   return 1
   )


REM *** Zip all files in the Src_Dir ***

SET Src_Dir=Temp\


if Exist MirZ21.z21 DEL MirZ21.z21
if Exist MirZ21.zip DEL MirZ21.zip

pushd %Src_Dir%
%ZipProg% a -r ..\MirZ21.zip export\*.*
popd

REN MirZ21.zip MirZ21.z21

REM MEmu "Tablet" Downloads directory
if Exist "C:\Users\Hardi\Downloads\MEmu Download\" Copy MirZ21.z21 "C:\Users\Hardi\Downloads\MEmu Download\"



