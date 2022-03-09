@ECHO OFF
: SET PortNr=8
ECHO Errasing the FLASH of the ESP32
ECHO This will clear all "EEPROM" (nvr) settings
ECHO.
ECHO To delete the whole flash memory enter
ECHO   %~nx0 all
ECHO.
Pause

if not "%1" == "all" goto Erase_nvr

ECHO Erasing the whole Flash
%userprofile%\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\2.6.1/esptool.exe --chip esp32  erase_flash
Goto End

:Erase_nvr
ECHO Erase only the NVR if the standard configutation is used
REM See: "C:\Users\Hardi\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\tools\partitions\default.csv"
%userprofile%\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\2.6.1/esptool.exe erase_region 0x9000 0x5000

:End
Pause
