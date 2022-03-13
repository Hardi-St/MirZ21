#if defined(USE_CAN)
/*
 CAN Bus connection to the Maerklin world


 IPAdresse Hardi: 192.168.178.39 oder .40
 Netzwerkeinstellungen werden ueber die Webseite (http://192.168.0.111) geändert



 To compile this project use the following settings in the Arduino IDE (1.8.16):
  *  Board: "WeMos D1 R1"        (ESP8266 Boards (2.7.4))
     Upload Speed: "921600"
     CPU Frequency: "80 MHz"
     Flash Size: "4MB (FS:2MB OTA:\1019KB)
     Debug port: "Disabled"
     Debug Level: "Keine"
     IwIP Variant: "v2 Lower Memory"
     VTables: "Flash"
     Exceptions: "Legacy (new can return nullptr)"
     Erase Flash: "Only Sketch"                         ?
     SSL Support: "All SLL ciphers (most compatible)"


 Pins for the Z21_to_Maerklin Bridge
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                         Shottky                                                              Attention:
                         1N5819                       _________________                       Jumers at the 1.3"
    Attention:                                       /  ____   _   _   \                      display have to be
    Pin order doesn't      3.3V                      |  | | |_| |_| |  |                      set correctly
    match with the          |                        |  | |         |  |                      Otherwise + and GND
    CAN module             _|_                   RST-|  ,___________,TX|-GPIO1  TXD0          are swapped!
                           /_\   .------------- ADC0-|A0|           |RX|-GPIO3  RXD0          .-------------.
           / -   CS  -------|----|------------GPIO16-|D0|           |D1|-GPIO5  SCL   --------| OLED Display|
    CAN BUS     SCLK ------ |----|------------GPIO14-|D5|  ESP8266  |D2|-GPIO4  SDA   --------'-------------'
    MCP2515     MISO -[100]-o----|------------GPIO12-|D6|           |D3|-GPIO0  FLASH -------- IR-Receiver (TSSP 4P38 oder TSOP 4836)
           \ -  MOSI ------------|------------GPIO13-|D7|___________|D4|-GPIO2  (Buildin) LED --Vcc         ~~~~~~~~~
                                 |          .-GPIO15-|D8               |-GND                                Im Steckbrett eingebaut
                                 o-[1k]-----|-- 3.3V-|                 |-5V
    The ISO CAN Module           |       \ _|+       '--, Wemos D1 Mini|                       Der IR Empfänger kann auch an D4 angeschlossen werden
    has to be powered           \   Piezo |_|     Rst ==|   ____       |                       Dann flackert die LED automatisch mit. Sie kann aber nicht
    with 5V!                     \       /  |           '---    -------'                       Anderweitig genutzt werden.
                                _|_        _|_                                                 Getestet mit: ...IRremoteESP8266\examples\IRrecvDemo

 CAN Kabel von MS2                       Attention: The Pins GPIO0, GPIO2 and GPIO15
 Märklin E146781                                    select the boot selection.
  CAN H  Orange   Sub-D 7                           - GPIO0 and GPIO2 need to be pulled up to Vcc
  CAN L  Braun    Sub-D 2                           - GPIO15 needs to be pulled to GND
  GND    Schwarz  Sub-D 3, 6                        See: https://www.instructables.com/ESP8266-Using-GPIO0-GPIO2-as-inputs/
  +18V   Rot      Sub-D 9

 Interrupts:
 - GPIO16 Kann keine Interrupts (https://microcontrollerslab.com/esp8266-interrupts-timers-arduino-ide-nodemcu/)
 - GPIO15 Bootet nicht wenn PullUp verwendet wird, Geht, mit Signal nach +, hat eingbauten PullDown
 => Der IR-Receiver muss an Pin D3 angeschlossen werden
    Wenn man CAN Interrupts verwenden will, dann könnte man die über einen PNP Transistor
    an GPIO15 anschließen.

 Es gibt (mindestens) zwei CAN Bibliotheken welche Interrupts unterstützen:
 - MCP2515-nb
 - PWFusion_Mcp2515
                                                    ^   ^ 3.3V
                                    TSSP 4P38 .___. |   |
       ^                              oder    |( )| | (LED)
      ,|,     _________________     TSOP 4836 |___| |  ,|,
      |1|    /    _   _   __   \               |||__|  | | 100
      |K|   /    | |_| |_|      \              ||      |_|    3.3V       .-------------.  Attention:
       |    |    |________      |              |'-GND   |      ^   GND---|    OLED     |  Jumers at the 1.3"
       o--- |A0  ,---------,  D0| GPIO16 CS  --|--------|--.   o---------|    1.3 "    |  display have to be
       |    |rev |         |  D1| GPIO5  SCL --|--------|--|---|---------|     or      |  set correctly
       |    |rev | ESP8266 |  D2| GPIO4  SDA --|--------|--|---|---------|    0.96"    |  Otherwise + and GND
       o    |rev |         |  D3| GPIO0  IR  --'        |  |   |         '-------------'  are swapped!
      /     |rev |         |  D4| GPIO2  LED -----------'  |  _|_         Shottky
     /      |rev |_________| 3V3|                          |  /_\         1N5819
       |    |rev             GND|                          |   |         .-----------.
      _|_   |GND              D5| GPIO14 SCLK -------------|---|---------|  OptoIso  |    The ISO CAN Module
            |3V3              D6| GPIO12 MISO -------------|---o--[100]--|  MCP5215  |    has to be powered
            |GND  NODEMCU     D7| GPIO13 MOSI -------------|-------------|    CAN    |    with 5V! The standard
            |3V3  DEVKIT 0.9  D8| GPIO15 Piezo--.          '-------------|   Modul   |    module is connected
            |EN               RX| GPIO3        +|_./                     '-----------'    to 3.3V.
            |RST              TX| GPIO1         |_|                      Attention:
            |GND             GND|               |  \                     Pin order doesn't
            |5V              3V3|              _|_                       match with the
            |    # ,_____, #    |                                        CAN module
            '------| USB |------'
                   '-----'


 Man könnte auch mehrere Widerstandskodierte Taster anschließen





 Docu:
 ~~~~~
 - cs2CAN-Protokoll-2_0.pdf
 - z21-lan-protokoll-v1-11.pdf    (part of the Z21 library)
 - Doku: GPIO15 deffiniert Boot source und muss auf Low liegen beim start
         https://www.instructables.com/ESP8266-Using-GPIO0-GPIO2-as-inputs/
 - Lokbilder für Z21 App 500x200 png mit transparentem Hintergrund      Verhältnis 2.5/1
   Märklin:  Breite Max 128, Höhe Max 48:
             C:\Dat\Märklin\Arduino\MS2_Bridge\Doc\Funktion_Icons\Lok_Icons_Mä
 - Prozessor in der MS2 HW 2.0: LM3S2412 (ARM)
 - Freie SQL Datenbank: https://de.wikipedia.org/wiki/MariaDB

 Teile:
 ~~~~~~
 - CS2 Ersatzkabel Kabel: Maerklin 146781 (15Eu)
   https://www.lokmuseum.de/shopartikel.php?SWg1=Spur%20H0%20M%E4rklin&SWg2=Ersatzteile&SArt=10020587&SBez=E146781
 - Flachstecker für MS2 Kabel:  JST B4B ZR

 Links:
 ~~~~~~
 - CAN Verteiler von Gerhard Bertelsman: http://lnxpps.de/maerklin/verteiler/
 - SQLight für den ESP32: https://github.com/siara-cc/esp32_arduino_sqlite3_lib
 - SID einlesen bei Tams: http://www.punktkontakt.de/_sgg/m3m6_1.htm
 - mfxuid: https://www.stummiforum.de/t147582f7-Wo-finde-ich-die-UID-eines-Lokdecoders.html
 - SQLight browser: https://sqlitebrowser.org/dl/
 - Anleitung: https://www.stummiforum.de/t156885f7-Z-App-Datenbank-mit-dem-PC-bearbeiten.html

 Befehle:
 ~~~~~~~~
 Flash löschen: %userprofile%\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\2.6.1/esptool.exe --chip esp32   --port COM8 --baud 921600 erase_flash
                %userprofile%\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\2.6.1/esptool.exe --chip esp8266 --port COM6 --baud 921600 erase_flash

 Boot partitions aufspielen: "%userprofile%\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\2.6.1/esptool.exe" --chip esp32 --port COM8 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 "%userprofile%\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4/tools/partitions/boot_app0.bin" 0x1000 "%userprofile%\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4/tools/sdk/bin/bootloader_qio_80m.bin"


 Revision History:
 ~~~~~~~~~~~~~~~~~
 17.01.22:  - Started
 02.02.22:  - EEPROM is working for ESP8266 and ESP32
 14.02.22:  - Extended CAN Filter beim ESP korrigiert. Jetzt funktioniert das lesen der Lokdaten von der CS2.
              Vorher hat es nur mit der MS2 Funktioniert. Hier waren die unteren zwei Bytes im CAN Filter nicht genutzt.
 15.02.22:  - ESP32 is working
              - Beep
              - CAN corrected
              - Using the binary mode of preferences library to store the loko data
 19.02.22:  - Loco data are stored in a packed form
              => it's possible to store twice as much locos
            - If the number of locos is still to high not all data are stored to the EEPROM
              - Kunctions types => F0-F31 is shown
              - Loco names => "Lok nr" is shown
            - Corrected the display scrolling for 3" LCD Display
 21.02.22:  - Enable/Disable the power by the IR controls
            - Saving selected loco from the Märklin IR remote control
            - Corrected direction display on the OLED
            - IR function keys support momentary function
            - Display changes from CAN or IR directly on the Z21 app

 Adressbereiche
 ~~~~~~~~~~~~~~
 - Aus MS2:
   - MM2 Dip  1-80
   - MM2 Prog 1-255
   - DCC      1- 10239


 Aufzeichenen der Meldungen:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 - PlatformIO:
   In der Platformio.ini Datei die Zeile "monitor_filters = log2file" einfügen.
     pio device monitor -b 500000 --echo

 - Powershell und TEE:
   Ausgaben ganz normal sichtbar. Man benötigt aber Powershell 7.2. Mit der Version 5.x funktioniert der -Encoding Schalter nicht
     Serial_Monitor.cmd | TEE -Encoding ascii Messages.txt

 - MiniTerm:
   Dabei sieht man die Ausgaben nicht ;-(
     C:\Users\Hardi\AppData\Local\Programs\Python\Python37-32\Scripts\pyserial-miniterm.exe com4 115200 --eol CR > Read_Lokinfo.txt
   Abbruch mit Ctrl+Pause

 Falsche Icons?
 ~~~~~~~~~~~~~~       MS2         CS2                              ESU
 Cora F3:  Symbol 11 Pfeife       Trillerpfeife                           Wenn 11 von MS2 kommt => 12
 Cora F8:  Symbol 5  Lautsprecher Lautsprecher
 Rolf F9:  Symbol 8  Schildkröte  Schildkröte                      Trillerpfeife
 Die Trillerpfeife von Esu wird bei MS2 und CS2 als Schildkröte angezeigt

 Die MS2 kennt keine Trillerpfeife => Hier wird der Lautsprecher gezeigt

 Es gibt zwei verschiedene Z21 Aps
  - Z21 App    vom 7.9.2021 Ver. 1.4.0
  - Z21 mobile vom 2.8.2017 Ver. 2.6.9  Alt, trotz größerer Versionsnummer

 Z21 Bilder:
  Das Bild beograd64055_wagon1.png stammt von hier:
   https://www.modellbahn-fokus.de/product/H0/Roco/Sneltreinwagon/4-1-0-001003-80840-0-nl-3-3-2-0-0-1-0-0/index.html

 ToDo:
 ~~~~~
 - Erkennen wen eine neue MFX Lok auf die Schinen gestellt wird

 - Ablauf USB Port Erkennung:
   - DCCSend()
   - SendDCCAccessoryCommand()
   - Check_USB_Port_with_Dialog()
   - USB_Port_Dialog()
   - Show_USB_Port_Dialog()
   - Select_Arduino_w_Blinking_LEDs_Dialog
   - Select_COM_Port_UserForm.ShowDialog
     - EnumComPorts
     - DetectArduino Wenn eine Arduino/ESP erkannt wurde
       - Transact
   - Get_Arduino_Baudrate

 - 11.03.22:  MS2 von 3.121 auf 3.148 aktualisiert

 - Die Lokomotiven fahren nicht los, wenn sie nicht an der entsprechenden MS2 hängen.
   Evtl. muss man da was an der MS2 Bridge ändern

 - Wie viele Apps können sich verbinden?
   Anzahl auf 20 erhöhen

 - Aus Irgend einem Grund Funktioniert das entzippen der ZLib Dateien nicht mehr mit
   der aktuellen Version (23.02.22). Auch dann nicht wenn man MAX_LOK_DATA auf 65 setzt.
   Am 26.1.22 wurde die Datei "Read loks von CS2.txt" erzeugt. Dann ging es noch.
   Die HEX Ausgabe des Programms lässt sich noch mit "Unzip_Hex_to_txt.cmd" konvertieren
   => Die Daten werden richtig vom CAN gelesen.

 - Die Beep Funktionen sollen den Rechner nicht Blockieren
   tone() Funktion ohne Dauer verwenden. Das ist wichtig damit sofort ein Ton gespielt
   wird wenn die Stop Taste gedrückt wird und gleichzeitig der Befehl an die
   Lokomotiven geschickt wird.
   - https://github.com/end2endzone/NonBlockingRTTTL
   - https://forum.arduino.cc/t/tone-function-multitasking/397163

 - Befehl Einbauen mit dem die Belegung des EEPROMS angezeigt wird
   - Anzahl Locos
   - Frei Locos   (Full, No Function, No Name)
   - Einschänkungen


 - Anzeige wenn keine CAN Daten. Dann reagiert die Z21 App auch nicht auf eine Änderung der Geschwimdigkeit

 - Anzeige der IP Adresse wenn Stop per Taster gedrückt wurde

 - Das starten nach dem Löschen des EEPROMS mit Erase Flash_ESP*.cmd funktioniert nicht.
   Danach kann man den ESP nicht mehr richtig mit dem WLAN verbinden
   - Das AP Password wurde nicht neu initialisiert (Hoffentlich geht das jetzt)             => Nein
     => Man kommt gar nicht mehr auf den AP
   - Nach dem löschen des Flash und neu einspielen des Programms bootet der ESP nicht mehr
     Es kommen nur noch die ????
   - Wenn man sich dann verbunden hat, dann bricht die Verbindung der Z21 App nach ein paar Sekunden ab
     Das hatte ich schon mal, weiß aber nciht mehr warum es dann doch wieder ging
   - Wenn das EEPROM Gelöscht ist, dann wird zunächst die Lokliste gelesen. Wärend dieser
     Zeit kann man sich vermutlich nicht am AP anmelden.
   - Beim Versuch sich am AP Anzumelden kommt ein mal pro Sekunde:
       dhcps: send_nak>>udp_sendto result 0
   - Nach erfolgreiche Anmeldung kommt
       [E][WiFiClient.cpp:395] write(): fail on fd 58, errno: 5, "I/O error"
       dhcps: send_nak>>udp_sendto result 0
   - Wenn man den Strom für eine Weile abschaltet, dann geht es ganz normal?!?

   Nach dem Löschen des Flash kommen folgende Meldungen:
     EEPROM initialized Size:767
     Check EEPROM...OK
     Starting ESPSetup
     AP Name: Z21_to_Maerklin
     Pw: 12345678
     IP: 192.168.0.111
     Connect fail to
     ESPSetup O.K.
     Z21PG v4.950 - ESP32.AREF_1V1
     Build: Feb 16 2022 08:35:45
     Server Begin...OK
     Start UDP listener...OK
     Init OTA...OK
     RAM: 245796
     Pref Check_Verstr_EEPROM
     [E][Preferences.cpp:49] begin(): nvs_open failed: NOT_FOUND
     Error opening the NVS-Namespace
     Pref EEPROM_Loko_Write()
     [E][Preferences.cpp:90] remove(): nvs_erase_key fail: Locos NOT_FOUND
     Error removing 'Locos' from nvr

   Zweiter Versuch
     Check EEPROM...OK
     Starting ESPSetup
     [E][WiFiAP.cpp:109] softAP(): passphrase too short!                           <= ?
     AP Name: MIRz21_ESP32
     Pw: 123456
     IP: 192.168.0.111
     Connect fail to ''
     ESPSetup O.K.
     Z21PG v4.950 - ESP32.AREF_1V1
     Build: Feb 16 2022 18:59:07
     Server Begin...OK
     Start UDP listener...OK
     Init OTA...OK
     RAM: 246880
     Pref Check_Verstr_EEPROM
     [E][Preferences.cpp:49] begin(): nvs_open failed: NOT_FOUND
     Error opening the NVS-Namespace
     Pref EEPROM_Loko_Write()
     [E][Preferences.cpp:90] remove(): nvs_erase_key fail: Locos NOT_FOUND
     Error removing 'Locos' from nvr
     CAN Init OK!
     Lok_Cnt:0
     Starting Read_Lok_Config_from_CAN

 - Vorher (18.02.22) waren ein paar Zeilen des LCD Displays verschoben (Unteren 3 Zeilen des Loknamens)
   Nach einem Neustart war es wieder gut?

 - Beim ESP32 eine Notstop Schaltung vorsehen

 - Neuer Name: "MIRZ21"    'M' und 'Z' in einer Größeren Schrift. 'M' Rot, 'Z' Blau
   Gesprochen MIR Z21


 - Menü
   - Auswahlliste als char[], Trennung mit '\0', Ende '\0\0'
   - Callback Funktion zum eínlesen der Tasten (Hoch/Runter/Enter/ESC/Pos1/End)

 - Lokbild

 - Möglichkeit mit der man die Funktionsicons per serielem Kommando ändern kann
   So mann man die MS2 Icons Upgraden zu CS2 Icons

 - Anderer Ton wenn nicht vorhandene FktTaste gedrückt wird

 - Anzeige in eigenes Modul

 - Zwei Displays:
   Die I2C Adresse kann mit setI2CAddress() angegeben werden



 - Weichen

 - Status abfragen (Strom/Temp/..)
 - Menu
   - Lok Liste auslesen

 - Einer unbekannte DCC oder MM Lok soll auch so zu fahren sein (Adresse +4000 oder +9000)
 - Behandlung von unbekannten Lokomotiven welche von der Z21 App empfangen wurden
   - Bei der ersten unbekannten Lok wird die Lokliste neu gelesen
     Wenn die Lok nicht vorhanden ist, dann wird sie angelegt (Name: New DCC Nr)
     Vielleicht kann man herausfinden ob die Lok da ist und mit welchem Protokoll
     sie fährt.
     Wenn das Protokoll über den Offset bekannt ist, dann kann man die Lok vermutlich
     einfach so steuern.
   - Wenn MFX Lok erkannt wird, dann Lokliste neu lesen
   - Prüfen ob zwei MFX Lokos die gleiche Adresse haben. Wenn ja, dann stimmt die Datenbasis nicht mehr und muss neu gelesen werden
 - Neueinlesen der Lokliste
   - Wenn eine unbekannte Lok abgerufen wird
   - Auf Tastendruck
   - Es soll einen schnellen modus geben bei dem nur die neuen Lokomotiven angefragt werden

 - Widerstands kodierte Taster

 - Testen ob das F() Makro Speicher spart

 - SQLight untersuchen: https://github.com/siara-cc/esp_arduino_sqlite3_lib
                        https://github.com/siara-cc/esp32_arduino_sqlite3_lib


 - Manchmal hängt sich das 1.3Zoll OLED auf. Zusätzliche Pull Ups bringen nichts

 - Verbesserung in MS2_Projekt übernemen
   - Read_Config_Line_from_CAN()
     Da war ein Denkfehler in der Timeout erkennung
     Es kann auch passieren, dass eine CAN Message mit '0x0A' beginnt
     Jetzt kann TIMEOUT_CONF_LINE 500 gesetzt werden ohne dass es timeouts gibt

 - Betrieb nur mit Gleisbox?
   - Es kommt keine Antwort auf einen Ping ;-(
*/


#include <mcp_can_nd.h>
#include <SPI.h>

#include "Dprintf.h"
#include "MS2_EEPROM_Adr.h"

#if defined (ESP32)
  #include <CAN_ESP32.h>
  #define CAN_CLASS  CAN_ESP32
  CAN_CLASS  can;
#else
  #define CAN_CLASS  MCP_CAN
  CAN_CLASS  can(CAN_CS_PIN);
#endif

bool     CAN_is_ok = 0;
uint16_t Lok_Cnt = 0;

#define CENTR_UNKNON  0
#define CENTR_MS2     1
#define CENTR_CSx     2

#define BOOST_UNKNOW  0
#define BOOST_GB      1
#define BOOST_CS2     2
#define BOOST_CS3     3

uint8_t       Central_Typ   = CENTR_UNKNON;
uint8_t       Booster_Typ   = BOOST_UNKNOW;
unsigned long Central_CanId = 0;
uint16_t      Central_Ver   = 0;

//uint8_t       Loco_Speeds[MAX_LOK_DATA];

typedef struct  // Variable lok data which are stored in the RAM and EEPROM
  {
  uint16_t  Speed:10;
  uint16_t  Direction:2;
  uint16_t  Level:2;
  uint16_t  Unused:2;  // Two unused bits
  uint32_t  FuncKeys;
  } Var_Lok_Data_t;    // 6 Bytes

Var_Lok_Data_t   Var_Lok_Data[MAX_LOK_DATA];

#define Hash   0x0302  // 04.01.22:
/*
 https://www.stummiforum.de/t77840f7-CAN-Protokoll-Aufschl-sselung.html#msg1225471
 - Hash hat die Aufgabe, anzuzeigen, dass es sich um gueltige CS2-Befehle handelt.
    Dazu sind die Bits 7, 8 und 9 reserviert. Sie muessen das Bitmuster "110" enthalten.
 - Jeder Absender soll seinen eigenen Hash verwenden. Wie er den generiert empfiehlt zwar
   Maerklin, die Praxis zeigt aber, dass externe Software den universalen Hash „0x0300“
   verwendet (so auch SDL und C2XH (welche auch ein CAN-Protokoll anzeigen koennen), evtl. auch TC und WDP).
 - Beim Hashcode 0x0300 liegt entweder die UID 0x0000 oder 0xFFFF zugrunde, was also mit real existierender
   CS2/CAN-Hardware (MS1, CS1, ECoS, CS2, Detectoren) nicht kollidieren sollte.
 In Deinem Beispiel kommt man vom gesendeten Hash 0xDB5E wie folgt zur UID:
 1. UID wird ge-XOR-ed = 0x4347 XOR 0x9A99 = 0xD9DE
 2. Es werden keine Bits verschoben (Faulheit), sondern einfach die Bits 7, 8 und 9 auf “110” gesetzt:
    (0xD9DE OR 0x0300) AND 0xFF7F = 0xDB5E = 0b1101 1011 0101 1110. Und das ist genau der Hash des
    Ping-Reply aus Deinem obigen Beispiel.
 Auf das Einsparen der Bitverschiebung muss man erstmal kommen. Es bleibt festzuhalten: man kann vom Hashcode
 nicht auf die UID schliessen, da nicht bekannt sein kann, wie der Teilnehmer seinen Hash erzeugt hatte und auch
 nicht bekannt ist, wie die ueberschriebenen bzw. verschobenen Bits vorher ausgesehen haben. Ausserdem ist BIG Endian einzuhalten.


 Wenn das Gleis angeschaltet ist, dann kommen jede Sekunde einige Messages mit der CAN ID 00030300
 Das ist die antwort auf das "Lok Discovery" Kommando
 => Es ist besser wenn man nicht die 0x0300 im Programm verwendet

*/
#define IdReq  (0x00400000 + Hash) // 7.1 Befehl: Anfordern Config Data: 0x40
#define IdAnsw (0x00420000)                                                                                     // 05.01.22: Removed Hash
#define IdPing (0x00300000 + Hash)


#include "CAN_Read_Lok_Config.h"
#include "Mae_Config_Files.h"

#ifdef PRINT_CAN_FUNC_DESC
//-----------------------------------------------------------
void PrintFuncDesc(unsigned long rxId, byte len, byte *rxBuf)
//-----------------------------------------------------------
// MsgID: 80 10 4F 70
//        P  ID Hash    The last bit of the ID is the response bit => Mask with 0xFE
//
{
  unsigned char Id = (rxId >> 16) & 0xFF; // CAN_ID id used in cs2CAN-Protokoll-2_0.pdf in Red letters

  static byte first = 1;
  if (first) Dprintf("Disabled messages: Ping\n"); // Debug
  first = 0;
  switch (Id & 0xFE) // Disabled messages
    {
    case 0x30: return; // Ping
    }

  Dprintf("rx CAN% %08lX ", rxId);
  byte b;
  switch (Id & 0xFE)
    {
    case 0x00: Dprintf("System");                  break;
    case 0x02: Dprintf("LocDiscover");             break;
    case 0x03: Dprintf("Answ. Lok Discovery");     break;
    case 0x04: Dprintf("MFX Bind");                break;
    case 0x05: Dprintf("Answ. MFX Bind");          break;
    case 0x06: Dprintf("MFX Verify");              break;
    case 0x07: Dprintf("Answ. MFX Verify");        break;
    case 0x08: Dprintf("Speed");                   break;
    case 0x0A: Dprintf("Direction");               break;
    case 0x0C: Dprintf("LocFunk");                 break;
    case 0x0E: Dprintf("ReadConfig");              break;
    case 0x10: Dprintf("WriteConfig");             break;
    case 0x16: Dprintf("Zubehoer");                break;
    case 0x30: Dprintf("Ping");                    break;
    case 0x36: Dprintf("Bootloader CAN");          break;
    case 0x37: Dprintf("Bootloader Schienen");     break;
    case 0x3A: Dprintf("StatConfig");              break;
    case 0x40: Dprintf("Data Query");              break;
    case 0x42: Dprintf("Config Data Stream");      break;
    case 0x44: Dprintf("Connect 6021");            break;
    default:   Dprintf("0x%02X",Id & 0xFE);
    }
  Dprintf(" %s.",Id & 1 ? "resp":"reqw");

  if (len > 4)
      {
      switch (Id & 0xFE)
        {
        case 0x00: // "System"
                   switch (rxBuf[4])
                     {
                     case 0x00: Dprintf(" System Stopp"      ); break;
                     case 0x01: Dprintf(" System Go"         ); break;
                     case 0x02: Dprintf(" System Halt"       ); break;
                     case 0x03: Dprintf(" Lok Nothalt"       ); break;
                     case 0x04: Dprintf(" Lok Zyklus Stopp"  ); break;
                     case 0x05: Dprintf(" Lok Datenprotokoll"); break;
                     case 0x06: Dprintf(" Schaltzeit"        ); break;
                     case 0x07: Dprintf(" Fast Read"         ); break;
                     case 0x08: Dprintf(" Gleis Protokoll"   ); break;
                     case 0x09: Dprintf(" Neuanmeldezaehler" ); break;
                     case 0x0A: Dprintf(" Ueberlast"         ); break;
                     case 0x0B: Dprintf(" Status"            ); break;
                     case 0x0C: Dprintf(" Kennung"           ); break;
                     case 0x80: Dprintf(" Reset"             ); break;
                     }
                   break;
        }
      }
  Dprintf(" ");
  for (b = 0; b < 4;   b++) Dprintf( "%02X",rxBuf[b]);  // No Space in between for the MFX-UID/Loc-ID
  for (b = 4; b < len; b++) Dprintf(" %02X",rxBuf[b]);
  Dprintf("\n");
}
#else
  #define PrintFuncDesc(rxId, len, rxBuf)
#endif

void Display_Ext_Loco_if_possible(uint16_t Lok_Nr, uint8_t Z21_or_CAN); // Forward definition
void Beep(uint16_t ms);

/*                                                                                                            // 21.02.22:
 Änderungen der Geschwindigkeit, Richtung und Funktionstasten über CAN oder IR Fernbedienung sollen sofort
 auf der Z21 App angezeigt werden. Bisher kam die Reaktion mit 1-2 Sekunden Verzögerung.
 Normalerweise läuft alle Kommunikation bei der Z21 über das LAN. Ich habe keine richtige Möglichkeit
 gefunden wie man Änderungen von "Außen" auf das LAN Schickt.
 Als Abhilfe nutze ich die
   returnLocoStateFull(byte client, uint16_t Adr, bool bc)
 Funktion. Diese kann aber nicht direkt aufgerufen werden. Darum wird der Umweg über
   Z21.receive(uint8_t client, uint8_t *packet)
 verwendet bei dem eine empfangene Message simmuliert wird. Dummerweise sendet
   returnLocoStateFull()
 sendet aber nicht an alle clients. Es gibt zwei Möglichkeiten. Wenn es eine Statusanfrage einer
 A21 App ist, dann wird die Antwort nur an diese App geschickt (LAN_X_GET_LOCO_INFO).
 Wenn es eine Antwort auf eine änderung dieser App ist, dann bekommt sie selber keine Meldung davon
 sondern nur die anderen Apps.
 Da ich keinen eigenen Client für diese Rückmeldungen anlegen will benutzte ich bereits existierende
 Clients. Darum wird die Message einfach von zwei verschiedenen Clienst geschickt. So ist sichergestellt,
 dass alle Z21 App die Nachricht bekommen. Das ist zwar nicht ganz sauber, ...

   Analyse der Daten aus Z21LANreceive()
     Change Speed:
       returnLocoStateFull(1, 42, 1)
       1 Z21 RX: 7 0 40 0 21 24 5
       1 Z21 RX: A 0 40 0 E4 13 0 2A 96 4B
                          |  |  ~~~~ |   \__ XOR
                          |  |   |    \_____ Richtung und Geschwindigkeit  22
                          |  |    \_________ Adresse 29
                          |   \_____________ ** 128 Fahrstufen **
                           \________________ LAN_X_SET_LOCO
       LocoSpeed(Adr:42, Speed: 22, Dir:1)
       Adr:  42 uid:4008 Cora

     Set Function Key F0
       1 Z21 RX: A 0 40 0 E4 F8 0 2A 40 76
                          |  |  ~~~~ |   \__ XOR
                          |  |   |    \_____ F0  einschalten
                          |  |    \_________ Adresse 29
                          |   \_____________ ** Funktionen **
                           \________________ LAN_X_SET_LOCO
       notifyz21_Lan_LocoFkt( 42, 1,  0)
*/


//-------------------------------------------------------------------------------
void Add_XOR_to_Buffer_and_Send_to_Clients(byte *packetBuffer, uint8_t BufferLen)
//-------------------------------------------------------------------------------
{
  byte Xor = 0;
  for (uint8_t i = 4; i < BufferLen-1; i++) Xor = Xor ^ packetBuffer[i];
  packetBuffer[BufferLen-1] = Xor;

  z21.receive(1, packetBuffer); // Send to all clients except 1
  z21.receive(2, packetBuffer); // Send to all clients except 2 Unfortunately the clients 3, 4, ... get the message twice
}

//--------------------------------------------------------------------------
void Send_Speed_to_LAN(uint8_t Index, uint16_t Speed1000, uint8_t Direction)                                  // 21.02.22:
//--------------------------------------------------------------------------
// Speed1000 0..1000 (1024)  0xFFFF Don't change
// Direction 0 Don't change, 1 Forward, 2 Backward
//
{
  //Dprintf("Send_Speed_to_LAN %i, %i, %i\n", Index, Speed1000, Direction);
                          //  0      1     2     3      4     5        6      7      8      9
  byte packetBuffer[10] = { 0x0A,  0x00, 0x40, 0x00,  0xE4, 0x13 };// 0x00,  0x2A,  0x96,  0x4B

  uint16_t Adr = Read_Lok_Adr_from_EEPROM(Index);
  packetBuffer[6] = (Adr >>8) & 0x3F;
  packetBuffer[7] =  Adr      & 0xFF;
  if (Adr >= 128) packetBuffer[6] |= 0xC0;

  uint8_t OldDir = dcc.getLocoDir(Adr) ? 0x80 : 0;
  uint8_t Speed128;
  if (Speed1000 == 0xFFFF) // Don't change the speed
       Speed128 = dcc.getLocoSpeed(Adr);
  else {
       Speed128 = (((uint32_t)Speed1000*127)+500)/1000;  // +500 for rounding
       if (Speed128 > 127) Speed128 = 127;
       }
  if (Direction == 0)
       {
       Speed128 |= OldDir;
       Direction = Var_Lok_Data[Index].Direction; // Don't change the direction                               // 20.02.22:
       }
  else if (Direction == 1) Speed128 |= 0x80;

  if ((dcc.getLocoSpeed(Adr) | OldDir) == Speed128) return ; // Nothing to do => return. That's important
                                                             // because otherwise the Speed1000 steps get
                                                             // overwritten by the feedback received over LAN
  packetBuffer[8] = Speed128;
  Add_XOR_to_Buffer_and_Send_to_Clients(packetBuffer, sizeof(packetBuffer));
}
//------------------------------------------------------------------
void Send_FktKey_to_LAN(uint8_t Index, uint8_t FktNr, uint8_t OnOff)
//------------------------------------------------------------------
{
  //Dprintf("Send_FktKey_to_LAN %i, %i, %i\n", Index, FktNr, OnOff);
                          //  0      1     2     3      4     5        6      7      8      9
  byte packetBuffer[10] = { 0x0A,  0x00, 0x40, 0x00,  0xE4, 0xF8 };// 0x00,  0x2A,  0x96,  0x4B

  uint16_t Adr = Read_Lok_Adr_from_EEPROM(Index);
  packetBuffer[6] = (Adr >>8) & 0x3F;
  packetBuffer[7] =  Adr      & 0xFF;
  if (Adr >= 128) packetBuffer[6] |= 0xC0;

  packetBuffer[8] = (OnOff<<6) | (FktNr & 0x3F);
  Add_XOR_to_Buffer_and_Send_to_Clients(packetBuffer, sizeof(packetBuffer));
}

//---------------------------------------------------------------------------------------
void SetDCCSpeed(uint8_t Index, uint16_t Speed1000, uint8_t Direction, uint8_t SendtoCAN)
//---------------------------------------------------------------------------------------
// Set the Speed for the DCC world
// Speed1000 0..1000 (1024)
// Direction 0 Don't change, 1 Forward, 2 Backward
//
// This function is called if a speed or direction
// message is received from the CAN bus.
{
  if (!SendtoCAN) Send_Speed_to_LAN(Index, Speed1000, Direction); // Received from CAN?

  uint16_t Adr = Read_Lok_Adr_from_EEPROM(Index);
  uint8_t Speed128 = (((uint32_t)Speed1000*127)+500)/1000;  // +500 for rounding
  if (Speed128 > 127) Speed128 = 127;
  if (Direction == 0)
       {
       uint8_t OldDir = dcc.getLocoDir(Adr) ? 0x80 : 0;
       //Dprintf("OldDir %i ", OldDir); // Debug
       Speed128 |= OldDir;
       Direction = Var_Lok_Data[Index].Direction; // Don't change the direction                               // 20.02.22:
       }
  else if (Direction == 1) Speed128 |= 0x80;

  dcc.setSpeed128(Adr, Speed128);
  Var_Lok_Data[Index].Speed = ((uint32_t)(0x7F & Speed128) * 1000) / 127;
  Var_Lok_Data[Index].Direction = Direction;

  Dprintf("DCC_Speed %i Dir %i Adr %i\n", Speed128 & 0x7F, (Speed128 & 0x80)>0, Adr);  // Debug
  Display_Ext_Loco_if_possible(Index, 0);
}

//---------------------------------------------------------------------------------
void SetDCC_LokoFkt(uint8_t Index, uint8_t FktNr, uint8_t OnOff, uint8_t SendtoCAN)
//---------------------------------------------------------------------------------
// This function is called if a a function key
// message is received from the CAN bus.
{
  if (!SendtoCAN) Send_FktKey_to_LAN(Index, FktNr, OnOff);

  dcc.setLocoFunc(Read_Lok_Adr_from_EEPROM(Index), OnOff, FktNr);

  uint32_t Mask = 1;
  Mask = Mask << FktNr;
  uint32_t FuncKeys = Var_Lok_Data[Index].FuncKeys;
  if (OnOff)
       { FuncKeys |=  Mask; }
  else { FuncKeys &= ~Mask; }
  Var_Lok_Data[Index].FuncKeys = FuncKeys;
  Dprintf("SetDCC_LokoFkt(%i, %i, %i)\n", Index, FktNr, OnOff);  // Debug
  Display_Ext_Loco_if_possible(Index, 0);
}

//-------------------------
uint32_t Get_UID(byte *Buf)
//-------------------------
{
  uint32_t ret = 0;
  for (byte *p = Buf; p < Buf+4; p++)
      {
      ret = ret<<8;
      ret |= *p;
      }
   return ret;
}

#define GET_1_BYTE(BufP) BufP[4]
#define GET_2_BYTE(BufP) ((BufP[4]<<8) + BufP[5])

//-------------------------------------------------------------------
void Store_CAN_Data(byte CAN_ID, byte *rxBuf, byte len, uint8_t Send)
//-------------------------------------------------------------------
// Store Lok speed, direction and functions from CAN
{
  //Dprintf("Store_CAN_Data %02X len:%i, uid:%04lX  ix:%i\n", CAN_ID, len, Get_UID(rxBuf), Find_Index_from_ID(Get_UID(rxBuf)));  // Debug
  if (len <= 4) return;

  // Store the actual states
  uint8_t Index = Find_Index_from_ID(Get_UID(rxBuf));
  if (Index != 0xFFFF)
     {
     switch (CAN_ID)
        {
        case 0x08: // Speed
                   //Dprintf("Store_CAN_Data Speed %i\n", GET_2_BYTE(rxBuf)); // Debug
                   SetDCCSpeed(Index, GET_2_BYTE(rxBuf), 0, Send);
                   break;
        case 0x0A: // Direction
                   //Dprintf("Store_CAN_Data Dir %i\n", GET_1_BYTE(rxBuf));  // Debug
                   SetDCCSpeed(Index, 0, GET_1_BYTE(rxBuf), Send); // Speed = 0
                   break;
        case 0x0C: // Lok functions
                   if (rxBuf[4] < 32 && len == 6) // len == 6" to skip the request data message
                      SetDCC_LokoFkt(Index, rxBuf[4], rxBuf[5], Send);
                   break;
        }
     }
}

//--------------------------------------------
void ProcPing(unsigned long rxId, byte *rxBuf)
//--------------------------------------------
// Update the global variable Central_Typ with
//  CENTR_UNKNON
//  CENTR_MS2
//  CENTR_CSx
// and Central_CanId with the corrospondin MsgId
//
//  Test: Nur CS2
//  00301B35 -  0                              Anfrage
//  00311B34 -  8    43 54 20 60 03 51 00 00   Booster in CS2
//  00310303 -  8    43 54 20 61 04 02 FF FF   CS2  (Sw 4.2.13)  Wird gesendet wenn ein anderes Gerät eine Ping Anfrage schickt
//                   ----------- ----- -----
//                   UID Sender  SWVer Gerät
//
//  Aus Lokabfrage per Progamm an CS3 (loknamen 0 10).trc       Version vom CAN
//  0030EF2A -  0
//  0031DB7F -  8    63 73 7E D8 0C 71 00 50   Booster in CS3   SW 12.113 ?
//  00310000 -  8    63 73 7E D9 01 01 FF EE   CS3?             SW 1.1    ?
//                   ----------- ----- -----
//                   UID Sender  SWVer Gerät

//  Test: MS2 + Gleisbox
//  00300303 -  0                              Anfrage von Prog.
//  0031BB4A -  8    4D 54 FA 1E 03 79 00 33   MS2 als Master   SW 3.121
//  0031D329 -  8    47 42 EE 90 01 2F 00 11   Gleisbox         SW 1.47

//  Test: MS2 von Gerd mit alte SW 2.7 + Gleisbox
//  0030BB4B -  0
//  0031033A -  8    47 45 FD 47 01 2F 00 11   Gleisbox         SW 1.47
//  0031E72C -  8    4D 53 31 FF 02 07 00 32   MS2 als Master   SW 2.7

//  Test: CS2 & MS2
//  00300303 -  0                              Anfrage von Prog.
//  00311B34 -  8    43 54 20 60 03 51 00 00   Booster in CS2   SW 3.81
//  0031131C -  8    4D 53 CF 4F 03 79 00 32   MS2 Slave        SW 3.121
//  00310303 -  8    43 54 20 61 04 02 FF FF   CS2  (Sw 4.2.13) SW 4.2
//
//  Aus der Datei: Lokabfrage per Progamm an CS3 (loknamen 0 10).xlsx
//  0000DB7F -   6   63 73 7E D8 30 01         System Go    wird von CS3 gesendet
//  0000DB7F -   6   63 73 7E D8 30 00         System Stopp  "
//  0031DB7F -   8   63 73 7E D8 0C 71 00 50   CS3, da von dieser ID auch die Befehle oben kommen
//  00310000 -   8   63 73 7E D9 01 01 FF EE   Booster in CS3 ?



//  Test: Lokabfrage graue MS2 HW2.0 mit schwarzer GB keine gesp. Loko
//  00300302 -  0
//  0031033A -  8    47 45 FD 47 01 2F 00 11   Gleisbox         SW 1.47
//  0031B733 -  8    4D 53 BB E0 03 79 00 32   MS2 als Slave ?  SW 3.121  Im Menü sagt die MS2, dass sie der Master ist
//  => Die ID 00 32 kann slave oder Master sein
//  ==> Die Erkennung muss anders gemacht werden. Das Problem dabei ist, dass die
//      CS2 so lange braucht bis sie gebootet hat. Eine an die CS2 angeschlossene MS2 antwortet
//      dann schon viel früher
//
// Ping Antworten beim Booten von CS2 mit MS2 (Zeiten in ms)
//      20995.761 2  Rx    0031131C -  8    4D 53 CF 4F 03 79 00 32   MS2
//     100781.842 2  Rx    00310302 -  8    43 54 20 61 04 02 FF FF   CS2
//     102234.458 2  Rx    00311B34 -  8    43 54 20 60 03 51 00 00   Booster in CS2
// => Man kann erst dann bestimmen ob eine MS2 mit GB oder eine CS2 vorhanden ist wenn
//    sich der Booster oder die GB gemeldet hat
//
//  Die Software Version ist Dezimal zu interpretieren
//  03 79 = 3.121 = Software Version der MS2
//  01 2F = 1.47 = Software Version der Gleisbox
//
//  Gerätekennung    (Aus CAN_Doku_V101.pdf)
//  0x00 0x00  Gleis Format Prozessor in CS2 60213,60214 / Booster 60173, 60174
//  0x00 0x10  Gleisbox 60112 und 60113
//  0x00 0x20  Connect 6021 Art-Nr.60128  (wird an die CS2 angeschlossen zur verbindung mit den alten Märklin Digital Systems)
//  0x00 0x30  MS 2 60653, Txxxxx
//  0xFF 0xE0  Wireless Devices (Was ist das?)
//  0xFF 0xFF  CS2-GUI (Master)  (gibt es eine andere ID wenn eine zweite CS2 als Slave angeschlossen ist?)
//
//  Beobachtet:
//  0x00 0x50  CS3-GUI (Master), Es meldet sich kein separater Booster
//  0xFF 0xEE  ? gesehen die MS3 angeschlossen war.
//  0x00 0x32  MS2 HW2.0
//  0x00 0x33  MS2 HW2.1
//  0x00 0x11  Gleisbox 60116
//  0x00 0x40  S88 Link
{
  // Der Booster bestimmt den Typ der Zentrale
  // - GB             => MS2 ist die Zentrale
  // - Booster in CS2 => CSx ist die Zentrale
  //
  // Wenn meherer MS2 im System sind, dann ist die MS2 der Master welche die niedrigere
  // Seriennummer hat. Oder ist es anderst rum? In Meinem Beispiel passt es.
  // Aber woher bekommt man die Seriennummer bei der MS2
  // Ist es immer die mit der niedrigeren Message Nummer? Nein bei mir ist es genau anders rum.
  //    0x4F70 = 20336d  HW2.1   Serial: 111268
  //    0xB733 = 46899d  HW2.0   Serial:  35296 Master
  // => Ich weiß nicht auf welcher Message ID geantwortet wird.
  //    Die unten bestimmte "Central_CanId" bringt nichts
  //    Sie bleibt erst mal drinnen. Evtl. kann ich später noch was damit machen.
  //

  if (Central_Typ != CENTR_UNKNON) return ; // Wenn die Zemtrale gereits gekannt ist, dann wird sie nicht mehr verändert

  uint16_t DeviceTyp = ReadUIntN(rxBuf+6,2);
  switch (DeviceTyp) // Detect the Booster / Gleisbox
    {
    case 0x0000: // Gleis Format Prozessor in CS2 60213,60214 / Booster 60173, 60174
    case 0xFFEE: // Gleis Format Prozessor in CS3 ?
                 Booster_Typ = BOOST_CS2;
                 break;
    case 0x0010: // Gleisbox 60112 und 60113 = meine grauen
    case 0x0011: // Gleisbox 60116 = meine schwarze
                 Booster_Typ = BOOST_GB;
                 break;
    case 0x0050: Booster_Typ = BOOST_CS3;
                 break;
    }

  if (Booster_Typ != BOOST_UNKNOW)  // Die Zentrale kann erst dann bestimmt werden wenn der Booster bekannt ist
     {                              // Wenn eine MS2 an eine CS2 angeschlossen ist, dann ist die MS2 sofort da
     switch (DeviceTyp)             // es muss aber noch auf die CS2 gewartet werden. Der Booster in der CS2 kommt spy
        {
        case 0x0030: // MS 2 60653, Txxxxx
        case 0x0031: // Noch nicht beobachtet
        case 0x0032: // MS2 HW2.0 = zwei meiner grauen MS2
        case 0x0033: // MS2 HW2.1 = zwei meiner grauen MS2 und die schwarze
                     if (Booster_Typ == BOOST_GB)
                        Central_Typ = CENTR_MS2;
                     break;
        case 0xFFFF: // CS2-GUI (Master)
        case 0x0050: // CS3-GUI (Master) ?:
                     Central_Typ = CENTR_CSx;
                     break;
        }
     if (Central_Typ != CENTR_UNKNON)
        {
        Central_CanId = rxId & 0x0000FFFF;
        Central_Ver   = ReadUIntN(rxBuf+4, 2);
        }
     }
}

void CAN_Server(void); // Forward definition

//-------------------------------
const char *Get_Central_Typ_Str()
//-------------------------------
{
  switch (Central_Typ)
    {                       //1234567890123  Medium font
    case CENTR_UNKNON: return "Nothing";
    case CENTR_MS2:    return "MS2";
    case CENTR_CSx:    return "CSx";
    default:           return "Unknown";
    }
}

uint8_t Read_Z21Button(); // Forward definition

//---------------------------------------------
uint8_t Check_Abort_Button(uint8_t &Old_Button)
//---------------------------------------------
// Return 1 if the abort button was pressed
{
  uint8_t Button = Read_Z21Button();
  if (Button == 0 && Old_Button != 0) // button pressed now
     {
     uint32_t TimeOut = millis() + 3000;
     while(Read_Z21Button() == 0 || millis() > TimeOut) // wait until the button is released
       WDT_RESET();
     return 1;
     }
  Old_Button = Button;
  return 0;
}

//------------------------------------------------------------
uint8_t Check_Abort_Button_or_SerialChar(uint8_t &Old_Button)
//------------------------------------------------------------
{
  uint8_t Res = Check_Abort_Button(Old_Button);
  if (Res == 0 && Serial.available() > 0) Res = 2;
  return Res;
}

//---------------------------------
uint8_t Get_Central(CAN_CLASS *can)
//---------------------------------
// Die Funktion schickt alle 500 ms einen Ping ab.
// Sie wartet x Sekunden auf Antworten.
// Im Test hat es bei einer MS2/GB 1.6 ms gedauert bis beide geantwortet haben.
// Die lange Wartezeit ist dazu da, dass die MS2 gebootet hat
{
  if (Central_Typ != CENTR_UNKNON) return Central_Typ;
  uint8_t WaitSeconds = 3*60;  // The boot time of the CS2 is <2 Minutes  (100 sec)
  uint32_t NextSend = 0;
  int16_t  CountDown = WaitSeconds * 2;
  uint8_t  WaitingWasShown = 0;
  uint8_t Old_Button = 0; // Don't abort if the button is always pressed (Not/wrong connected, or stuck)
  while (Central_Typ == CENTR_UNKNON && CountDown > 0)
    {
    if (millis() >= NextSend)
       {
       NextSend = millis() + 500;
       byte Buf[8];
       can->sendMsgBuf(IdPing, 1, 0, Buf);
       CountDown--;
       if (CountDown % 2 == 0)          // First update after 500 ms because CountDown is always even.
          {         //1234567890123     // If the Ping is received before no display is shown to prevent flashing up two displays which can't be read
          Write_OLED("\14\n\23"         // Clear screen and medium size font
                     " Waiting for\n"
                     " CAN master \n"
                     "     %i\21", CountDown / 2); // \21 = Smal font
          WaitingWasShown = 1;
          }
       }
    CAN_Server();
    WDT_RESET();
    if (Check_Abort_Button_or_SerialChar(Old_Button)) break; // Abort by pressing the button
    }
  if (WaitingWasShown)
      {         //1234567890123
      Write_OLED("\23\14" // Clear screen and Medium size font
             "\n"
             "  Detected\n"
             "  %s\21", Get_Central_Typ_Str());    // \21 = Small font
      }
  return Central_Typ;
}


//-------------------
void CAN_Server(void)
//-------------------
{
  unsigned long rxId;
  byte len;
  byte rxBuf[8];

  if (CAN_is_ok && (can.checkReceive() == CAN_MSGAVAIL))
     {
     if (can.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK)
        {
        // if (rxId & 0x80000000) Dprintf("Msg %08X %i:", rxId & 0x7FFFFFFF, len); else Dprintf("Msg     %04X %i:", rxId); for (uint8_t i = 0; i < len; i++) Dprintf(" %02X", rxBuf[i]); Dprintf("\n"); // Debug
        unsigned char CAN_ID = (rxId >> 16) & 0xFF; // CAN_ID id used in CAN_Doku_V101.pdf in Red letters
        PrintFuncDesc(rxId, len, rxBuf); // Debug

        switch (CAN_ID)
           {
           case 0x08: // Speed
           case 0x0A: // Direction
           case 0x0C: // Lok functions
           case 0x16: // Accessory command
                      Store_CAN_Data(CAN_ID, rxBuf, len, 0);
                      break;
           case 0x31: if (len == 8) ProcPing(rxId, rxBuf);
                      break;
#if SPY_CAN_CONFIG_FILE
           case 0x40: // Config data request
           case 0x42:
           case 0x84: // Debug messages
                      Start_Spy_Config_File(rxId, len, rxBuf); break;
#endif

/*
           case 0x00: // System command (Sub Cmd in byte 4)
                      switch (rxBuf[4])
                         {
                         case 0x00: // Stop
                                    #ifdef DEBUG_CAN_SEND
                                      Dprintf("Send Stop to %i\n",j);
                                    #endif
                                    break;
                         }
           case 0x0C: // Lok function request                                                              // 09.01.22:
                      // Unfortunately the answer to the Lok functions F16-F31 from the GB is wrong
                      // => We correct them
                      // Ich habe das Problem hier beschrieben: "Problem mit Funktionstasten 16-31.docx"
                      // und an Maerklin gemeldet.
                      // Als Workaround wird mit dem folgenden Programmteil geprueft ob eine Anfrage Message
                      // der Funktion F23 oder F31 an die Gleisbox gesendet wurde und entsprechend korrigierte
                      // Rueckmeldungen gesendet. Das Funktioniert mit der GB Software 1.47 zuverlaessig.
                      // Das Individuelle korrigieren jeder einzelnen Abfrage war nicht moeglich. Dabei sind
                      // Anfragen verloren gegangen und nicht korrigiert worden.
                      // Diese Korrektur wird nur dann afgerufen wenn das erste mal mit der SHIFT Taste der
                      // MS2 auf eine neue Seute gewechselt wird. Anschliessend merkt sich die MS2 die Werte.
                      if (len == 5 && (rxBuf[4] == 0x17 || rxBuf[4] == 0x1F))
                         {
                         uint8_t Index = Find_Index_from_ID(Get_UID(rxBuf));
                         if (Index != 0xFFFF)
                            {
                            uint32_t Mask = 1;
                            rxBuf[4] -= 7;
                            Mask = Mask << (rxBuf[4]);
                            for (uint8_t i = 0; i < 8; i++) // Send 8 Messages
                                {
                                rxBuf[5] = Var_Lok_Data[Index].FuncKeys & Mask ? 1 : 0;
                                can.sendMsgBuf(0x000D0300, 1, 6, rxBuf);
                                Mask = Mask << 1;
                                rxBuf[4]++;
                                }
                            }
                         }
                      break;
 */
           case 0x00: // System command request (Sub Cmd in byte 4)
                      switch (rxBuf[4])
                         {
                         case 0x00: globalPower(csTrackVoltageOff);
                                    break;
                         case 0x01: // Go
                                    globalPower(csNormal);
                                    break;
                         case 0x02: globalPower(csEmergencyStop);
                                    break;
                         }
                       break;
/*
           case 0x01: // System command response (Sub Cmd in byte 4)
                      switch (rxBuf[4])
                         {
                         case 0x0A: // Ueberlast
                                    Clear_GB_Power_Enable(i);
                                    Dprintf("ueberlast\n");
                                    break;
                         case 0x0B: if (len == 8) // Response to Command "System Status"
                                       {
                                       uint16_t Val = (rxBuf[6] << 8) + rxBuf[7];
                                       switch (rxBuf[5])
                                          {
                                          case 1: Measurement[i].Current = Val; break;
                                          case 3: Measurement[i].Voltage = Val; break;
                                          case 4: Measurement[i].Temp    = Val; break;
                                          }
                                       }
                                    break;
                          }
                      break;
           case 0x31: // Ping response
                      if (len == 8 && rxBuf[6] == 0
                         && ((rxBuf[7] == 0x10) || // Response from Gleisbox 60112 und 60113
                             (rxBuf[7] == 0x11)))  // Response from new Gleisbox software version 1.47 ?  05.01.22:
                         {
                         GleisBoxUId[i] = Get_UID(rxBuf);
                         //Dprintf("Ping Resp. GleisBoxUId[%i]=%08lX\n",i, GleisBoxUId[i]);
                         }
                      break;
           case 0x16: // Accessory command
                      if (len == 6) Set_Accessory_State(rxBuf);
                      break;
*/
           }
        }
     }
}

//----------------------------------------
void UId_to_Data(uint32_t uid, byte *Data)
//----------------------------------------
{
  for (int8_t i = 3; i >= 0; i--, uid >>= 8)
      Data[i] = uid & 0xFF;
}

//------------------------------------------------------------------------
void Send_Cmd_to_Lok(uint8_t Cmd, uint32_t uid, uint8_t len, uint16_t Par)
//------------------------------------------------------------------------
// Send one command to the CAN
{
  byte Data[8];
  UId_to_Data(uid, Data);

  if (len == 5)
       { Data[4] = Par & 0xFF; }
  else { Data[4] = (Par>>8); Data[5] = Par & 0xFF; }
  uint32_t MsgID = Hash + ((uint32_t)Cmd << 16);

  //Dprintf("uid %lX, Par %i, MsgId:%08lX",uid, Par, MsgID); for (uint8_t i = 0; i < len; i++) Dprintf(" %02X",Data[i]); Dprintf("\n");

  Store_CAN_Data(Cmd, Data, len, 1);
  if (CAN_is_ok)
     {
     can.sendMsgBuf(MsgID, 1, len, Data);
     }
}

//----------------------------------------------------------------------------
void Send_Cmd_to_Lok_by_Ix(uint8_t Cmd, uint8_t Ix, uint8_t len, uint16_t Par)
//----------------------------------------------------------------------------
{
  uint32_t uid = Read_Lok_uid_from_EEPROM(Ix);
  Send_Cmd_to_Lok(Cmd, uid, len, Par);
}

//------------------------------------------------------------------------------------
void Send_Cmd_From_IR_to_Lok_by_Ix(uint8_t Cmd, uint8_t Ix, uint8_t len, uint16_t Par)
//------------------------------------------------------------------------------------
{
  // Forward the commands to the LAN before calling Send_Cmd_to_Lok_by_Ix()                                   // 21.02.22:
  switch (Cmd)
    {
    case 0x08: // Speed
               Dprintf("Send_Cmd_From_IR_to_Lok_by_Ix Speed:%i\n", Par);
               Send_Speed_to_LAN(Ix, Par, 0);
               break;
    case 0x0A: // Direction
               Dprintf("Send_Cmd_From_IR_to_Lok_by_Ix Dir:%i\n", Par);
               Send_Speed_to_LAN(Ix, 0xFFFF, Par);
               break;
    case 0x0C: // Function keys
               Dprintf("Send_Cmd_From_IR_to_Lok_by_Ix FucKey:%i\n", Par);
               Send_FktKey_to_LAN(Ix, Par>>8, Par & 0xFF);
               break;
    }

  Send_Cmd_to_Lok_by_Ix(Cmd, Ix, len, Par);
}

//--------------------------------------
void Send_System_Cmd_to_CAN(uint8_t Cmd)
//--------------------------------------
{
  if (!CAN_is_ok) return ;

  byte Buf[5] = { 0, 0, 0, 0 }; // Send Go to all  Loks
  Buf[4] = Cmd;
  uint32_t Id = Hash; // CAN_ID = 0
  can.sendMsgBuf(Id, 1, 5, Buf);

  #if 1
    const char *Msg = "?";
    switch (Cmd)
      {
      case 0x00: Msg = "Stopp";   // Gleis Format Prozessor stoppt den Betrieb auf Haupt- und Programmiergleis. Es wird keine elektrische
                       break;     // Energie mehr geliefert. Alle Fahrstufen/Funktionswerte und Einstellungen werden behalten.
      case 0x01: Msg = "Go";      // Der Gleis Format Prozessor aktiviert den Betrieb und liefert elektrische Energie. Es werden alle evtl.
                       break;     // noch vorhandenen bzw. gespeicherten Geschwindigkeitsstufen/Funktionen wieder gesendet.
      case 0x02: Msg = "Halt";    // Allen Lokomotiven wird befohlen, inklusive ABV, anzuhalten (Fahrstufe 0). Digitalsignal weiterhin auf
                       break;     // Gleis, danach werden keine weiteren Kommandos auf das Gleis gesendet. Elektrische Energie steht weiterhin zur Verfügung.
      }
    Dprintf("Send_System_Cmd_to_CAN: %s\n", Msg);
  #endif
}

//--------------------------
void Send_Halt_to_all_Loco()
//--------------------------
{
  #if SET_SPEED_CONTROLLERS_TO_0_AT_STOP
      for (uint16_t i = 0; i < Lok_Cnt; i++)
        {
        if (Var_Lok_Data[i].Speed > 0)
           Send_Cmd_to_Lok_by_Ix(0x08, i, 6, 0);  // Set the speed display to 0
        }
  #endif
  Send_System_Cmd_to_CAN(0x02);  // Send Halt to all lokos
}

//------------------------------------
void Send_Emergency_Stop_to_all_Loco()
//------------------------------------
{
  for (uint16_t i = 0; i < Lok_Cnt; i++)
      {
      if (Var_Lok_Data[i].Speed > 0)
         Send_Cmd_to_Lok_by_Ix(0x00, i, 5, 0x03); // Befehl: Lok Nothalt
      }
  Send_System_Cmd_to_CAN(0x02);  // Send Halt to all lokos
}

//------------------
void Disable_Power()
//------------------
{
  Send_Emergency_Stop_to_all_Loco();

  uint32_t End = millis() + 1000;
  while (millis() < End)
      {
      CAN_Server();
      WDT_RESET();
      }
  Send_System_Cmd_to_CAN(0x00); // Turn of the power
}


extern "C" {
//------------------------------------------------------------------
void notifyz21_Lan_LocoFkt(uint16_t Adr, uint8_t state, uint8_t fkt)
//------------------------------------------------------------------
{
  uint8_t F0_4 = dcc.getFunktion0to4(Adr) << 1;               // gibt Funktionszustand - F0 F4 F3 F2 F1 zurueck
  uint32_t FuncKeys = (F0_4 | ((F0_4 &  0x20) ? 1:0)) & 0x1F; // Sortet correct F4 - F0
  FuncKeys |= dcc.getFunktion5to8(  Adr)  << 5;               // gibt Funktionszustand - F8 F7 F6 F5    zurueck
  FuncKeys |= dcc.getFunktion9to12( Adr)  << 9;               // gibt Funktionszustand - F12 F11 F10 F9 zurueck
  FuncKeys |= dcc.getFunktion13to20(Adr)  << 13;              // gibt Funktionszustand - F20 - F13      zurueck
  FuncKeys |= dcc.getFunktion21to28(Adr)  << 21;              // gibt Funktionszustand - F28 - F21      zurueck

  //Dprintf("notifyz21_Lan_LocoFkt(%3i, %i, %2i) ", Adr, state, fkt); Serial.println(0x80000000 | FuncKeys, BIN); // 0x80000000 to fill all digits  // Debug

  uint16_t Lok_Nr = Find_Index_from_Adr(Adr);
  if (Lok_Nr == 0xFFFF) return;
  uint32_t Mask = 1;
  Mask = Mask << fkt;
  uint16_t Par = fkt*256;
  if (state)
       { FuncKeys |=  Mask; Par++; }
  else { FuncKeys &= ~Mask; }
  Var_Lok_Data[Lok_Nr].FuncKeys = FuncKeys;
  Send_Cmd_to_Lok_by_Ix(0x0C, Lok_Nr, 6, Par);
  Display_Ext_Loco_if_possible(Lok_Nr, 1);
}

//-------------------------------------------------------
void notifyz21_Lan_LocoSpeed(uint16_t Adr, uint8_t speed)
//-------------------------------------------------------
// Speed Bit  0x80 = Direction forward
//       Bits 0x7F = Speed (0-127)
// Is called if the Z21 app sends new speed and direction data
{
  Dprintf("LocoSpeed(Adr:%i, Speed:%3i, Dir:%i)\n", Adr, speed&0x7F, (speed&0x80)>0); // Debug

  if (Railpower == csEmergencyStop) globalPower(csNormal); // Disable the emergency stop is the speed is changed

  uint16_t Lok_Nr = Find_Index_from_Adr(Adr);
  if (Lok_Nr == 0xFFFF) return;
  uint16_t Speed1000 = 0;

  uint8_t Dir = (speed & 0x80) > 0 ? 1 : 2;
  Send_Cmd_to_Lok_by_Ix(0x0A, Lok_Nr, 5, Dir);
  Speed1000 = ((uint32_t)(0x7F & speed) * 1000) / 127;
//  Var_Lok_Data[Lok_Nr].Direction = Dir;
//  Var_Lok_Data[Lok_Nr].Speed = Speed1000;
  Send_Cmd_to_Lok_by_Ix(0x08, Lok_Nr, 6, Speed1000);
  Display_Ext_Loco_if_possible(Lok_Nr, 1);
}

//-------------------------------------
void notifyz21_RailPower(uint8_t State)
//-------------------------------------
{
  switch (State)
    {
    case csNormal:           // Normal Operation Resumed ist eingeschaltet
                             Send_System_Cmd_to_CAN(1);
                             break;
    case csEmergencyStop:    // Der Nothalt ist eingeschaltet
                             Send_Halt_to_all_Loco();
                             break;
    case csTrackVoltageOff:  // Die Gleisspannung ist abgeschaltet
                             Disable_Power();
                             break;
//  case csShortCircuit:     // Kurzschluss
//  case csServiceMode:      // Der Programmiermodus ist aktiv - Service Mode
    }
}

} // extern "C"




//--------------
void Print_RAM()
//--------------
{
  #if defined(ESP32)
     // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system.html#_CPPv222esp_get_free_heap_sizev
      Dprintf("freeRAM:%i  MinRAM %i\n",esp_get_free_internal_heap_size(), esp_get_minimum_free_heap_size());
  #elif defined(ESP8266)
     // https://arduino-esp8266.readthedocs.io/en/latest/libraries.html
     Dprintf("freeRAM:%i  Fragm %i%% MaxBlock %i\n", ESP.getFreeHeap(), ESP.getHeapFragmentation(), ESP.getMaxFreeBlockSize());
  #endif
}

#include "Animations.h"

//--------------
void CAN_setup()
//--------------
{
  // *** Initialize the CAN bus ***
  if (can.begin(MCP_STDEXT, CAN_250KBPS, MCP_8MHZ) == CAN_OK) // init CAN bus, baudrate: 250k@8MHz
       {
       Serial.println(F("CAN Init OK!"));
       CAN_is_ok = true;
       can.setMode(MCP_NORMAL);
       can.EnableStatusLEDs();
       }
  else {
       Serial.println(F("CAN Init Fail!"));
       Write_OLED("\23\14Error\n\22"
                  "CAN init failed\n");
       }

  Ma_Lok_Nr[1] = 150;

  Lok_Cnt = Read_Lok_Cnt_from_EEPROM();
  Dprintf("Lok_Cnt:%i\n", Lok_Cnt);   // Debug

  memset(Var_Lok_Data, 0, sizeof(Var_Lok_Data));

  if (CAN_is_ok)
     {
     if (Lok_Cnt == 0) Read_Lok_Config_from_CAN(&can);
     }
  Print_RAM(); // Debug

//  Beep(10);
//  Stummilok_Ani();

  Beep(100);
}

//-------------
void CAN_loop()
//-------------
{
  if (CAN_is_ok)
     CAN_Server();
}


#endif // USE_CAN

