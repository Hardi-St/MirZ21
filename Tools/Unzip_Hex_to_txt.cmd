@echo OFF
REM Unzip a compressed HEX file from the MIRz21 program
REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
REM - Converts Intel Hex files to a bin file
REM - Uncopresses the file with zlib
REM
REM The "loks" HEX file is generated with the serial console by entering
REM  r 9
REM and copying the output to a file.


IF "%1" == "" (
  ECHO No File to uncompress given
  ECHO Start the program with
  ECHO   Unzip_Hex_to_txt  File.hex
  ECHO.
  ECHO or drop a HEX filr to the program with the explorer
  PAUSE
  GOTO :EOF
  )

SET Ext=%~x1
if NOT "%Ext%" == ".hex" (
  ECHO Error: Wrong file given. The file must hafe the extention '.hex"
  GOTO :EOF
  )

SET Name=%~n1
IF Exist %Name%.txt Del %Name%.txt
IF Exist %Name%.bin Del %Name%.bin

hex2bin.exe %Name%.hex > NULL

IF NOT EXIST %Name%.bin (
  ECHO Error: The file %Name%.bin was not created ;-(
  Goto :EOF
  )

Call unzip_ZLib_file.py %Name%.bin %Name%.txt
IF Exist %Name%.bin Del %Name%.bin

ECHO.
ECHO.
ECHO.

IF Exist %Name%.txt ECHO %Name%.txt generated
IF NOT Exist %Name%.txt ECHO !!! Error generating %Name%.txt !!!
ECHO.
ECHO.

Pause
