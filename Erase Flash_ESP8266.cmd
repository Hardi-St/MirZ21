@ECHO OFF
SET PortNr=6
ECHO Errasing the FLASH of the ESP8266 connected to port %PortNr%
ECHO This will clear all "EEPROM" settings and the program
Pause

%userprofile%\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\2.6.1/esptool.exe --chip esp8266 --port COM%PortNr% --baud 921600 erase_flash
Pause
