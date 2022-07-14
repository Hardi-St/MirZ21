/**************************************************************
 * Z21 Ethernet DCC Command Station
 * Copyright (c) 2015-2022 by Philipp Gahtow
***************************************************************
*
* This is a simple dcc command station that receive commands via Ethernet, XpressNet or LocoNet.
* It base on the Z21 ethernet protocol of ROCO

Unterstützte Funktionen/Protokolle:
 * NMRA DCC output (POM and Railcom Cutout with Detector)
 * support functions F0 - F28
 * Z21 Ethernet over LAN and/or WLAN
 *  - support W5100 or ENC28 LAN Shield
 *  - support WLAN with ESP8266 over Serial
 * S88N feedback, with DCC
 * XpressNet with AUTO switch MASTER-SLAVE-MODE
 * LocoNet T/B with optional Slotserver
 * LocoNet LocoBuffer
 * multi Hardware Support:
 *  - support for ATmega 328p/644/1280/1284/2560 (NANO, UNO, MEGA, Sanguino)
 *  - support for ARM (DUE) (only: DCC, LAN, WLAN, S88)
 *  - support ESP8266 and ESP32
 *  --> a use of all functions/interfaces is only awayable on Arduino MEGA!!!

***************************************************************

- DCC Master Interface with Timer 2 by modifired CmdrArduino library by Philipp Gahtow
- Z21 LAN Protokoll mit W5100/ENC28 Ethernet Shield with z21.h library
- LAN HTTP Website on Port 80 to configure ethernet IP and S88 bus length
- ESP8266 WiFi Z21 LAN Untersützung with z21.h library and HTTP Webite
- fast S88N feedback
- legacy:(DCC input, to read signal from other Central Station via Interrupt 0 and Timer 4) - not supported anymore
- LocoNet at MEGA with Timer 5, normal Timer1 with Loconet.h library and Slotserver for FRED
- XpressNet (RS485) via LOOP-Function with XpressNetMaster.h library
- Relais for seperate program track
- Global Railcom Detector for MEGA on Serial3
- continus DCC for S88N and LocoNet-B
- support external LocoNet Booster over LocoNet-B Line
- use as LocoNet LocoBuffer over USB with JMRI and Rocrail

***************************************************************

Softwareversion: */
#define Z21mobileSwVer 495
/*
---------------------------------------------------------------
changes:
15.04.2015  Abschaltung S88 Interface per Define (S88N)
16.04.2015  Aktualisierung Z21 LAN Protokoll V1.05 & Firmware-Version 1.26
17.04.2015  LN OPC_INPUT_REP msg von Belegmeldern über LAN_LOCONET_DETECTOR
20.04.2015  kurze/Lange DCC Adressen (1..99 kurz, ab 100 lang)
22.04.2015  Add in DCC Lib Function support F13 - F28
            Add Power Button with Reset (press at startup)
23.04.2015  Add LocoNet set Railpower (OPC_GPOFF, OPC_GPON, OPC_IDLE)
            Add LocoNet Slot Write (OPC_WR_SL_DATA)
            New Broadcast Msg (8 Bit) Z21 Protokoll V1.05 (Include LocoNet)
            Add LocoNet OPC_RQ_SL_DATA, OPC_UHLI_FUN, OPC_SW_REQ, OPC_SW_REP, OPC_SW_ACK, OPC_SW_STATE
28.04.2015  Add DCC CV Write and Decoder Reset Packet before CV-Programming
04.07.2015  Add Support Sanguino (ATmega644p and ATmega1284p) =>MCU_config.h
10.07.2015  Change Timer for DCC Interface and S88 to support LocoNet for all MCU
            Add second Booster support (intenal/external)
21.07.2015  S88 max Module define hinzu und S88 HTTP Ausgabe angepasst
30.07.2015  Versionsnummer für Debug definiert
02.08.2015  DCC Accessory Befehl korrigiert
            PowerButton Input geändert von Pin 51 nach Pin 47
03.08.2015  DCC Decoder Funktion korrigiert
17.09.2015  S88 Timer Auswahl (MEGA = Timer3)
18.09.2015  ESP8266 WiFi Support; Z21 LAN über externe Library
23.09.2015  Überarbeitung LAN_LOCONET_DETECTOR
            Neues Kommando OPC_MULTI_SENSE
            DCC Dekoder ohne Timer4!
            Optionale Lok-Event-Informationen im LocoNet (reduzierung der Sendedaten)
03.10.2015  S88 verbessert -> Fehler in der S88 Modulanzahl korrigiert (Überlauf der Zählervariale)
            LocoNet TX/RX Packetverarbeitung verbessert
04.10.2015  ROCO EXT Booster Short mit Transistor (invertiert!)
            Optimierung S88 Timer (Rechenoperationen und Seicherbedarf)
10.10.2015  Anzeigen Reset Zentrale mittels binkenden LEDs
13.10.2015  Rückmelder über LocoNet
            Senden von DCC Weichenschaltmeldungen auch über LocoNet
            LAN Webseite angepasst für Smartphone Display
14.10.2015  Einstellung der Anzahl von S88 Modulen über WiFi
            Verbesserung der Kommunikation mit dem ESP
04.11.2015  LocoNet Master- oder Slave-Mode auswählbar
19.12.2015  Support kombinierte UDP Paket für WLAN und LAN
26.12.2015  Add Track-Power-Off after Service Mode
20.02.2016  Speicherreduzierung wenn kein WLAN und LAN genutzt wird
            LocoNet Client Modus Kommunikation mit IB verbessert
            Extra Serial Debug Option für XpressNet
27.02.2016  Änderung Dekodierung DCC14 und DCC28
            Invertierung Fahrtrichtung DCC Decoder DIRF
            LocoNet Slave-Mode ignoriere Steuerbefehle, wenn Slot = 0
02.06.2016  Baud für Debug und WiFi einstellbar
            Software Serial für WiFi wählbar (zB. für Arduino UNO)
            -> WiFi Modul Firmware ab v2.5
17.07.2016 Fix Network UDP Antwortport - Sende Pakete an Quellport zurück
25.07.2016 add busy message for XpressNet (MultiMaus update screen now)
Aug.2016   add Railcom Support and update DCCInterfaceMaster and Booster Hardware,
           support POM read over I2C with external MCU (GLOBALDETECTOR)
26.08.2016 add DHCP for Ethernet Shield
21.11.2016 DCC: fix Railcom - still Problem with Startup: Analog-Power on the rails - Hardware change needed!
26.11.2016 LocoNet: add Uhlenbrock Intellibox-II F13 to F28 support
27.11.2016 LocoNet: fix Speed DIR in OPC_SL_RD_DATA in data byte to 0x80 = B10000000 and OPC_LOCO_DIRF remove invert
27.12.2016 Z21 add CV lesen am Programmiergleis
01.02.2017 add negative DCC output option and seperate this feature from RAILCOM
15.03.2017 fix narrowing conversation inside LNInterface.h
28.03.2017 external Booster active in ServiceMode when no internal Booster
24.04.2017 fix data lost on loconet - s88 timer3 block packets - deactivated
28.04.2017 add MultiMaus support for F13 to F20 without fast flashing
10.05.2017 add XpressNet information for loco speed and function and switch position change
11.05.2017 add internal Booster Short Detection over Current Sence Resistor
25.05.2017 add RailCom Global Reader for Arduino MEGA on Serial3 (POM CV read only)
19.06.2017 fix problems with Arduino UNO compiling
09.07.2017 fix problems when using without XpressNet
23.07.2017 add support for Arduino DUE
26.08.2017 add default speed step setting
09.01.2018 add POM Bit write
21.01.2018 optimize LocoNet Slot system - reduce RAM use
18.08.2018 add support for Arduino DUE XpressNet
02.11.2018 adjust Z21 WiFi data communication and rise up baud rate
22.11.2018 add support for Arduino ESP8266 (WiFi, DCC extern and intern without seperate prog track)
09.06.2019 add extra DCC-Output for S88 and LocoNet without Power-OFF and RailCom
12.04.2020 adjust problems with the Serial communication to ESP8266 WiFi
22.06.2020 remove DUE XpressNet statememts
28.07.2020 change Input Pins VoltInPin and TempPin set only to INPUT Mode.
29.07.2020 add ENC28J60 module - instead of w5100 Shield for MEGA only.
30.07.2020 central startup Railpower sync for all interfaces
04.08.2020 fix size of data type "RailcomCVAdr"
24.10.2020 fix error inside LocoNetInterface with Master-Mode
29.10.2020 reduce timeout for LAN DHCP challange
30.10.2020 add Z21 Interface config file with template
04.12.2020 fix LocoNet Client Mode, don't answer on Master commands!
           remove Ethernet LAN DHCP timeout when using UIP Library
05.01.2021 fix WiFi RX problem with long UDP Packet and zero Packets (LAN_LOCONET_DETECTOR)
           fix wrong Adr in LocoNet Slot data response, when doing a DISPATCH Adr > 127
06.01.2021 add new AREF with 1.1 Volt
07.07.2021 fix EEPROM Problem with ESP32 and ESP8266 when doing a commit
08.01.2021 use ESP DNS library only for ESP32 and set a seperate one for ESP8266
02.03.2021 fix Ethernet S88 Module change if DHCP is on
03.03.2021 fix big UDP ethernet receive when package is empty
17.03.2021 fix error with inactiv Debug on HTTP reading S88Module at Debug.print
           change EEPROM storage for ESP
           add firmware to EEPROM and full EEPROM reset when no firmware is found or when POWER pressed on start
           Serialnumber in Z21-APP show now the firmware version
06.04.2021 fix report LocoNet sensor messages to Z21-APP for display
18.05.2021 add LocoNet2 library to support LocoNet on ESP32
20.05.2021 fix some mistakes in LocoNet packet working
02.06.2021 fix wrong function notifyPower into notifyRailpower
           fix DCCLed calculation when flashing
06.06.2021 fix wrong adr in LocoNet OPC_SW_REQ
10.06.2021 fix power button doesn't change back to csNormal
16.06.2021 add LocoNet drive direction invert for IntelliBox slave usage (#define LnInvDir)
18.06.2021 fix LocoNet Function 9-28 by Uhlenbrock (0xD4 0x20)
21.06.2021 add LAN Interface DHCP change over HTTPCONF
07.07.2021 fix ESP32 DCC Output and Pin config
30.09.2021 fix ethernet to not check port all the time
           fix enc28 ethernet shield bug for boardcast message send out
05.11.2021 add to handel Lok information request central and ask the master (DCC/LocoNet/XpressNet)
           add new debug flag "REQUEST" for global lok, switch and cv requests
07.11.2021 fix usage without WLAN and LAN problem with Z21 header #defines in LNInterface.h
14.11.2021 fix LocoNet from LAN
15.11.2021 fix LocoNet reporting to z21 interface
16.11.2021 fix ethernet update the source port on each request
17.11.2021 add LocoNet Buffer USB connection
           add ACK sence detection with LM357
18.11.2021 add prio delay to LocoNet send
21.11.2021 fix missing feedback data from XpressNet to LocoNet
30.11.2021 add middle Booster sence value
14.12.2021 get Booster sence not when RailCom cutout is on
27.12.2021 add option for OLED Display that shows the configuration data
28.12.2021 fix software number with additional dot
30.12.2021 remove blocking when receiving WLAN data
06.01.2022 slow down sense reading only for ESP8266 to let WLAN work
           add invert option for DCC/Power LED and Service Mode (Prog Relay) output
08.10.2022 fix Short Detection and ACK for WeMos with voltage devider
04.07.2022 don't show the WLAN password
------------------------------------
*/
//**************************************************************
//Load individual Z21 central configuration:
#include "config.h"   //own Z21 configurationfile

//**************************************************************
//Report Upload Sketch Time:
#if defined(DebugBaud) || defined(Z21DISPLAY)
const char* comp_date       = __DATE__ " " __TIME__;    //sketch was compiled
#endif

//**************************************************************
//LocoNet LocoBuffer-USB -> deaktiviere alle DEBUG Funktionen!
#if (defined(LnBufferUSB) && defined(LOCONET)) || (defined(UNO_MCU) && defined(XPRESSNET))
#undef DebugBaud
#undef Debug
#undef DEBUG
#undef DEBUG_WLAN_CONFIG
#undef REPORT
#undef REQEST
#undef LnDEB
#undef XnDEB
#undef Z21DEBUG
#undef Z21DATADEBUG
#undef Z21SYSTEMDATADEBUG
#else
#undef LnBufferUSB    //deaktiviere!
#endif



//**************************************************************
//Firmware store in EEPROM:
#define EEPROMSwVerMSB 0
#define EEPROMSwVerLSB 1

//**************************************************************
//Setup up PIN-Configuration for different MCU (UNO/MEGA/Sanduino)
#include "MCU_config.h"

/**************************************************************
Serial Debug Output:*/
#ifdef DebugBaud
  #ifndef Debug
    #define Debug Serial  //Interface for Debugging
  #endif
#endif

/**************************************************************
ESP OTA Upload:*/
#if defined(ESP_OTA)
#include <ArduinoOTA.h>
#include "ESP_OTA.h"
#endif

/**************************************************************
XpressNet Library:*/
#ifdef XPRESSNET
#include <XpressNetMaster.h>
#endif

/**************************************************************
LocoNet Library:*/
#ifdef LOCONET
#if defined(ESP32_MCU)  //use LocoNet2 Library
#include "LNSerial.h"
LocoNetBus bus;
#include <LocoNetESP32.h>
LocoNetESP32 locoNetPhy(&bus, LNRxPin, LNTxPin, 0);
LocoNetDispatcher parser(&bus);
#else
#include <LocoNet.h>
#endif
#endif

/**************************************************************
BOOSTER EXT:*/
#ifdef BOOSTER_EXT
#define BOOSTER_EXT_ON HIGH
#define BOOSTER_EXT_OFF LOW
#endif

/**************************************************************
BOOSTER INT:*/
#ifdef BOOSTER_INT
//#define BOOSTER_INT_ON LOW    //only for old Mode without RAILCOM support over NDCC!
//#define BOOSTER_INT_OFF HIGH  //only for old Mode without RAILCOM support over NDCC!
//#define BOOSTER_INT_NDCC    //for new RAILCOM Booster3R - GoIntPin activate inverted booster signal
#endif

/**************************************************************
DCC Master to create a DCC Signal:*/
#include <DCCPacketScheduler.h>   //DCC Interface Library

/* Set default SwitchFormat for accessory use */
#ifndef SwitchFormat
#define SwitchFormat ROCO
#endif

//**************************************************************
#if defined(Z21VIRTUAL)
#include <SoftwareSerial.h>
SoftwareSerial SoftSerial(TXvWiFi, RXvWiFi); // init Soft Serial
#undef LAN    //LAN nicht zulassen - Doppelbelegung der Signalleitungen!
#undef HTTPCONF
#undef LOCONET
#endif

//**************************************************************
#if defined(LAN)      //W5100 LAN Interface Library

#define HTTPCONF //activate Website to configure DHCP, IP Adress and Number of S88 Bus Module

#if !defined(MEGA_MCU)
#undef ENC28
#endif

#if defined(ENC28)  //MEGA MCU only!
#include <UIPEthernet.h>
#else
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library
#endif
#endif

#if defined(ESP_WIFI)   //ESP8266 or ESP32
#if defined(ESP8266_MCU)
  #include <ESP8266WiFi.h>
  //#include <ESP8266WebServer.h>
  #if defined ESP_OTA
  #include <ESP8266mDNS.h>  //OTA update
  #endif
#elif defined(ESP32_MCU)
  #include <WiFi.h>
  #include <WiFiAP.h>   //optional?
  #if defined(ESP_OTA)
  #include <ESPmDNS.h>  //ESP32 only, OTA update
  #endif
#endif
#include <WiFiClient.h>
#include <WiFiUDP.h>
#endif

//**************************************************************
//Configure the analog 1.1V refenence voltage:
#ifndef AREF_1V1
  #if defined(EXTERNAL_UREF_1V1)
  #define AREF_1V1 EXTERNAL   //externe 1.1 Volt
  #elif defined(MEGA_MCU)
  #define AREF_1V1 INTERNAL1V1  //Arduino MEGA internal 1,1 Volt
  #elif defined(UNO_MCU)
  #define AREF_1V1 INTERNAL   //Arduino UNO (equal to 1.1 volts on the ATmega168 or ATmega328P)
  #elif defined(ESP32_MCU) | defined(ESP8266_MCU)   //ESP Modul
  #define AREF_1V1      //always 1,1 Volt
  #endif
  //other Arduino's 5,0 Volt internal refence!
#endif

//**************************************************************
static void globalPower(byte state);

#if defined (__cplusplus)                                                                                     // 05.02.22:
  extern "C" {
#endif
void AllLocoData(uint16_t adr, uint8_t data[]); //so function can be found!

#if defined (__cplusplus)                                                                                     // 05.02.22:
              }
#endif

//**************************************************************
//Z21 LAN Protokoll:
#if defined(LAN) || defined (WIFI) || defined(ESP_WIFI)
  #include <z21.h>
  z21Class z21;
#else
//make LocoNet work when no WLAN and LAN is used!
#if defined(LOCONET)
  #include <z21header.h>
  #endif
#endif

//**************************************************************

#if defined(ESP8266_MCU) || defined(ESP32_MCU)
//--> EEPROM Konfiguration ESP:
#define DCC_EESize      767        // Größe des EEPROM
#if defined(ESP8266_MCU)
  #define EESize        4096       // Größe des EEPROM           // Achtung: Die Zahl wird nur beim ESP8266 verwendet beim ESP32 steht die Gr��e in "z21nvs.h" !!!
#endif

#define EEStringMaxSize 40         // Länge String im EEPROM
//Client:
#define EEssidLength    200        // Länge der SSID
#define EEssidBegin     201        // Start Wert
#define EEpassLength    232        // Länge des Passwort
#define EEpassBegin     233        // Start Wert
//AP:
#define EEssidAPLength  264        // Länge der SSID AP
#define EEssidAPBegin   265        // Start Wert
#define EEpassAPLength  298        // Länge des Passwort AP
#define EEpassAPBegin   300        // Start Wert
#define EEkanalAP       299        // Kanal AP
#endif

#ifndef FIXSTORAGE
  #if defined(DUE_MCU)
  #include <DueFlashStorage.h>
  DueFlashStorage Flash;
  #define FIXSTORAGE Flash
  #define FIXMODE write

  #elif defined(ESP32_MCU)  //use NVS on ESP32!
   #include "z21nvs.h"
   z21nvsClass NVSEEPROMZ21;
   #define FIXSTORAGE NVSEEPROMZ21
   #define FIXMODE write

  #else
  #include <EEPROM.h>   //EEPROM - to store number of S88 Module and LAN IP
  #define FIXSTORAGE EEPROM
    #if defined(ESP8266_MCU) || defined(ESP32_MCU)
    #define FIXMODE write
    #else
    #define FIXMODE update
    #endif
  #endif
#endif // FIXSTORAGE

//--> Allgemeine EEPROM Konfiguration:
//S88:
#define EES88Moduls 38  //Adresse EEPROM Anzahl der Module für S88
//LAN:
#define EELANDHCP 39  //aktiviert DHCP auf dem LAN Interface
#define EELANip 40    //Startddress im EEPROM für die IP

//---------------------------------------------------------------
#if defined(LAN)  //W5100 LAN Udp Setup:
EthernetUDP Udp;    //UDP for communication with APP/Computer (Port 21105)
#if defined(Z21MT)
EthernetUDP UdpMT;  //UDP to Z21 Maintenance Tool (Port 21106)
#include "Z21Maintenance.h"
#endif

//---------------------------------------------------------------
boolean LAN_DHCP = false;   //LAN DHCP Modus aus
// The IP address will be dependent on your local network:
IPAddress LAN_ip(192, 168, 0, 111);   //Werkseinstellung ist: 192.168.0.111
/*LAN MAC configuration: */
#ifndef LANmacB2
#define LANmacB2 0xEF
#endif
#ifndef LANmacB1
#define LANmacB1 0xFE
#endif
#ifndef LANmacB0
#define LANmacB0 0xED
#endif
static byte mac[] = { 0x84, 0x2B, 0xBC, LANmacB2, LANmacB1, LANmacB0 };
/*LAN DHCP configuration: */
#ifndef LANTimeoutDHCP
#define LANTimeoutDHCP 5000 //Timeout to wait for a DHCP respose
#endif
#if defined(HTTPCONF) //W5100 LAN config Website:
EthernetServer server(80);  // (port 80 is default for HTTP):
#endif
#endif    //LAN end
//---------------------------------------------------------------
#if defined(ESP_WIFI)
#ifndef ESP_HTTPCONF
#define ESP_HTTPCONF
#endif
#ifndef SddidAP
  #if defined (ESP32)
     #define SssidAP "MIRz21_ESP32"   // Default Z21 AP (SSID)SddidAP                                         // 18.01.22:  Old: "Z21_ESP_Central"
  #else
     #define SssidAP "MIRz21_ESP8266" // Default Z21 AP (SSID)SddidAP                                         // 18.01.22:  Old: "Z21_ESP_Central"
  #endif
#endif
#ifndef SpassAP
#define SpassAP "12345678"  // Default Z21 network password
#endif
#ifndef SkanalAP
#define SkanalAP 3          // Default Kanal des AP
#endif
//AP default Z21 IP
IPAddress AP_ip(192, 168, 0, 111);   //Werkseinstellung ist: 192.168.0.111
IPAddress AP_sb(255, 255, 255, 0);   //Werkseinstellung Subnet

WiFiUDP Udp;
#if defined(ESP_HTTPCONF) //Setting Website
WiFiServer ESPWebserver(80);  //default port 80 for HTTP
#endif
#endif
//--------------------------------------------------------------
//Z21 Protokoll Typ Spezifikationen
#if defined(LAN) || defined (WIFI) || defined(ESP_WIFI)
#include "Z21type.h"    //Z21 Data Information
#endif

//--------------------------------------------------------------
//S88 Singel Bus:
#if defined(S88N)
#include "S88.h"
#endif

//--------------------------------------------------------------
//Dallas Temperatur Sensor:
#if defined(DALLASTEMPSENSE) //&& defined(MEGA_MCU)
#include <OneWire.h>
#include <DallasTemperature.h>
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
// arrays to hold device address
DeviceAddress insideThermometer;
#define DALLAS_RESOLUTION 9  //Temperatur resolution
#endif

//--------------------------------------------------------------
//DCC Interface Master Short Detection:
//EXTERNAL BOOSTER:
#define DetectShortCircuit 0x1FF    //to detect short circuit  (0xFF)
unsigned int ShortTime = 0;            //Time Count for Short Detect
unsigned long LEDcount = 0;    //Timer for Status LED
//INTERNAL BOOSTER:
#if defined(BOOSTER_INT_MAINCURRENT)
  #define BOOSTER_INT_CURRENT_SHORT_DETECT //wenn nicht aktiviert dann hier einschalten!
  byte ShortTimeINT = 0;      //Time Count for internal short detect
  #if defined(ESP8266_MCU)
    //slow down readings on ESP8266 to let WIFI work!
    #define VAmpReadTimeNormal 10  //interval of reading ADC for ESP8266 in ms
    #define VAmpReadTimeService 1  //interval of reading ADC for ESP8266 in ms
    long VAmpLastMillis = 0;  //time we read ADC
  #else //other MCU:
    #define VAmpINTMesureCount 10   //Anzahl des Messungen
    byte CountVAmpINT = 0;      //Zahl des Messung
  #endif
  uint16_t CalcVAmpINT  = 0;  //zwischen Messung
  uint16_t VAmpINT = 0;       //current power on the track
  #ifndef DETECT_SHORT_INT_WAIT
    #if defined(ESP8266_MCU)
      #define DETECT_SHORT_INT_WAIT 3 //because of WeMos internal voltage devider!
    #else
      #define DETECT_SHORT_INT_WAIT 20 //Time after internal short circuit is detected
    #endif
  #endif
  #ifndef DETECT_SHORT_INT_VALUE
    #if defined(ESP8266_MCU)
        #define DETECT_SHORT_INT_VALUE  100  //because of WeMos internal voltage devider!
    #else
      #if defined(AREF_1V1)
        #define DETECT_SHORT_INT_VALUE  1000  //analogRead value for "mA" that is too much (AREF = 1.1V)
      #else
        #define DETECT_SHORT_INT_VALUE  400  //analogRead value for "mA" that is too much (AREF = 5.0V)
      #endif
    #endif
  #endif
#endif

//--------------------------------------------------------------
DCCPacketScheduler dcc;
// #define DCC     //activate DCC Interface (always)                                                          // 18.01.22:  Disabled

//--------------------------------------------------------------
#if defined(DCCGLOBALDETECTOR) && defined(DCC)
#include "Z21_RailCom.h"
#endif

//--------------------------------------------------------------
#if defined(XPRESSNET)
XpressNetMasterClass XpressNet;
#endif

//--------------------------------------------------------------
// certain global XPressnet status indicators
#define csNormal          0x00      // Normal Operation Resumed ist eingeschaltet
#define csEmergencyStop   0x01      // Der Nothalt ist eingeschaltet
#define csTrackVoltageOff 0x02      // Die Gleisspannung ist abgeschaltet
#define csShortCircuit    0x04      // Kurzschluss
#define csServiceMode     0x08      // Der Programmiermodus ist aktiv - Service Mode
byte Railpower = csTrackVoltageOff; // State of RailPower at Startup                                          // 05.02.22:  Maerklin systems start with power off Old: csNormal
uint Z21ButtonLastState = 0;        // store last value of the Push Button for GO/STOP                        // 06.02.22:  Changed to uint8_t to be able to start the action direct when the button was hold for longer than 750ms

//--------------------------------------------------------------
//LocoNet-Bus:
#if defined (LOCONET)
#include "LNInterface.h"
#endif

//--------------------------------------------------------------
//DCC Decoder:
#if defined(DECODER)
#include "DCCDecoder.h"
#endif

//--------------------------------------------------------------
//XpressNet-Bus:
#if defined(XPRESSNET)
#include "XBusInterface.h"
#endif

//--------------------------------------------------------------
//Z21 Ethernet communication:
#if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
#include "Z21_LAN.h"
#endif

//--------------------------------------------------------------
//OLED Display
//#if defined(Z21DISPLAY)
#include "OLEDdisplay.h"
//#endif

//--------------------------------------------------------------
//Z21 Software Reset:
#if defined(SOFT_RESET)
#include <avr/wdt.h>
#define soft_restart()      \
do                          \
{                           \
    wdt_enable(WDTO_15MS);  \
    for(;;)                 \
    {                       \
    }                       \
} while(0)
#endif

//************************* Additional lines from Hardi ********************************

void Toggle_Status_LED(); // Forward definition
void EEPROM_Loco_setup();

//--------------------------------------------------------------------------------------------
#if defined(DEBUG) && !defined(DUE_MCU) && !defined(ESP8266) && !defined(ESP32)
//ONLY for Atmega, not for Arduino DUE or ESP chip (ARM)!
int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif



//--------------------------------------------------------------------------------------------
//ONLY for ESP
#if defined(DEBUG) && (defined(ESP8266) || defined(ESP32))
  //-----------
  int freeRam()
  //-----------
  {
    return ESP.getFreeHeap();
  }
#endif

#include "Dprintf.h"                                                                                          // 30.01.22:

#include "CAN_Bus.h"          // Only active if USE_CAN is defined                                                                                          // 18.01.22:

#include "Serial_Commands.h"  // Only active if USE_SERIAL_COMMANDS is 1                                      // 25.01.22:

#if defined(IR_RECEIVE_PIN)
  #include "IR_Key_Input.h"

  IR_Key_Input_c IR_Key_Input(IR_RECEIVE_PIN);
  //----------------------------------------------------
  IR_Key_t Read_IR_Key_and_Flash_LED(uint16_t &Duration)
  //----------------------------------------------------
  {
     IR_Key_t key = IR_Key_Input.Read_IR_Key(Duration);
     if (key != IR_NO_KEY)
        {
        // Dprintf("IR Key %08X\n", key); // Debug
        Toggle_Status_LED();
        }
     return key;
  }
  #include "IR_Functions.h"
#endif


#if defined (__cplusplus)                                                                                     // 05.02.22:
  extern "C" {
#endif

   // Optional callback functions which must be defined outside
   extern void notifyz21_RailPower(uint8_t State) __attribute__((weak));

#if defined (__cplusplus)
  } // extern "C" {
#endif


//************************* End Additional lines from Hardi ********************************


//--------------------------------------------------------------------------------------------
//POWER set configuration:
static void globalPower (byte state) {
  if (Railpower != state) {

    #if defined(LAN) || defined (WIFI)
    if (Railpower == csServiceMode && state == csShortCircuit) {
      z21.setCVNackSC();  //response SHORT while Service Mode!
      }
    #endif

    Railpower = state;
    #if defined(DEBUG)
        Debug.print(F("Power: "));
        Debug.println(state);
    #endif
    switch (state) {
      case csNormal:
                                        #if defined(DCC)
                                        dcc.setpower(ON);
                                        #if defined(ProgRelaisPin)                                            // 16.01.22:
                                            #if defined(PROG_OUT_INVERT)
                                            digitalWrite(ProgRelaisPin, HIGH);     //ProgTrack
                                            #else
                                            digitalWrite(ProgRelaisPin, LOW);     //ProgTrack
                                            #endif
                                        #endif
                                        #endif
                                        #if defined(BOOSTER_EXT)
                                        if (digitalRead(ShortExtPin) == LOW)
                                          digitalWrite(GoExtPin, BOOSTER_EXT_ON);
                                        #endif

                                        #if (defined(BOOSTER_INT) && !defined(BOOSTER_INT_NDCC))
                                          digitalWrite(GoIntPin, BOOSTER_INT_ON);
                                        #endif

                                        break;
      case csTrackVoltageOff:
                                        #if defined(DCC)
                                        dcc.setpower(OFF);
                                        #if defined(ProgRelaisPin)                                            // 16.01.22:
                                            #if defined(PROG_OUT_INVERT)
                                            digitalWrite(ProgRelaisPin, HIGH);     //ProgTrack
                                            #else
                                            digitalWrite(ProgRelaisPin, LOW);     //ProgTrack
                                            #endif
                                        #endif
                                        #endif
                                        #if defined(BOOSTER_EXT)
                                        digitalWrite(GoExtPin, BOOSTER_EXT_OFF);
                                        #endif

                                        #if (defined(BOOSTER_INT) && !defined(BOOSTER_INT_NDCC))
                                        digitalWrite(GoIntPin, BOOSTER_INT_OFF);
                                        #endif
                                        break;
      case csServiceMode:
                                        #if defined(DCC)
                                            dcc.setpower(SERVICE); //already on!
                                            #if defined(ProgRelaisPin)                                        // 16.01.22:
                                                #if defined(PROG_OUT_INVERT)
                                                   digitalWrite(ProgRelaisPin, LOW);     //ProgTrack
                                                #else
                                                   digitalWrite(ProgRelaisPin, HIGH);     //ProgTrack
                                                #endif
                                            #endif
                                        #endif
                                        #if defined(BOOSTER_EXT)
                                          #if defined(BOOSTER_INT)
                                          digitalWrite(GoExtPin, BOOSTER_EXT_OFF);
                                          #else
                                          if (digitalRead(ShortExtPin) == LOW)
                                            digitalWrite(GoExtPin, BOOSTER_EXT_ON);
                                          #endif
                                        #endif

                                        #if (defined(BOOSTER_INT) && !defined(BOOSTER_INT_NDCC))
                                        digitalWrite(GoIntPin, BOOSTER_INT_ON);
                                        #endif

                                      break;
      case csShortCircuit:
                                      #if defined(DCC)
                                      dcc.setpower(SHORT);  //shut down via GO/STOP just for the Roco Booster
                                      #if defined(ProgRelaisPin)                                              // 16.01.22:
                                          #if defined(PROG_OUT_INVERT)
                                              digitalWrite(ProgRelaisPin, HIGH);     //ProgTrack
                                          #else
                                              digitalWrite(ProgRelaisPin, LOW);     //ProgTrack
                                          #endif
                                      #endif
                                      #endif
                                      #if defined(BOOSTER_EXT)
                                      digitalWrite(GoExtPin, BOOSTER_EXT_OFF);
                                      #endif

                                      #if (defined(BOOSTER_INT) && !defined(BOOSTER_INT_NDCC))
                                      digitalWrite(GoIntPin, BOOSTER_INT_OFF);
                                      #endif
                                      break;
     case csEmergencyStop:
                                        #if defined(DCC)
                                        dcc.eStop();
                                        #endif
                                      break;
      }
    #ifdef ShortLed
        if (Railpower == csShortCircuit)
           digitalWrite(ShortLed, HIGH);   //Short LED show State "short"
        if (Railpower == csNormal)
           digitalWrite(ShortLed, LOW);   //Short LED show State "normal"
    #endif // ShortLed
    #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
        z21.setPower(Railpower);
    #endif
    #if defined(XPRESSNET)
        XpressNet.setPower(Railpower);  //send to XpressNet
    #endif
    #if defined(LOCONET)
        LNsetpower(); //send to LocoNet
    #endif


    #if defined(Z21DISPLAY)
        DisplayUpdateRailPower();
    #endif

    if (notifyz21_RailPower) notifyz21_RailPower(state); // This function waits  1 sec if the power is turned off to send the
                                                         // halt command to all locos. This is important for locos with a super cap.
    }
}

//--------------------------------------------------------------------------------------------
//from DCCPacketScheduler -> notify power state when change into Programming Mode
void notifyRailpower(uint8_t state) {
  if (Railpower != state) {
    #if defined(Z21DEBUG)
    Debug.print(F("dcc "));
    #endif

    globalPower(state);
  }
}

//--------------------------------------------------------------------------------------------
#if defined(DCC)
void ShortDetection() {
  //Short Circuit?
  //Check BOOSTER extern
  #if defined(BOOSTER_EXT)
  if ((digitalRead(ShortExtPin) == HIGH) && (digitalRead(GoExtPin) == BOOSTER_EXT_ON) && (Railpower != csShortCircuit)) {
    ShortTime++;
    if(ShortTime == DetectShortCircuit) {
        globalPower(csShortCircuit);
        #if defined(DEBUG)
        Debug.println(F("TRACK_SHORT_CIRCUIT EXT"));
        #endif
    }
    /*  NOT IN USE ANYMORE from v4.75 on!
    //Before Railpower cut out test change polarity:
    else if (ShortTime == KSRelaisShortCircuit) {
      digitalWrite(KSPin, !digitalRead(KSPin));     //Kehrschleife
      #if defined(DEBUG)
      Debug.print(F("KS "));
      Debug.println( digitalRead(KSPin) );
      #endif
    }
    */
  }
  else ShortTime = 0;
  #endif
  //Check BOOSTER2 (z.B. TLE5206)
  #if defined(BOOSTER_INT)
  //---------------Short2 for CDE external Booster----------------------------------
  #if defined(BOOSTER_EXT_CDE)
  if ((digitalRead(ShortIntPin) == LOW) && (Railpower != csShortCircuit)) {
    globalPower(csShortCircuit);
    #if defined(DEBUG)
    Debug.println(F("TRACK_SHORT_CIRCUIT CDE"));
    #endif
  }
  //---------------Short2 TLE detection----------------------------------
  #elif defined(BOOSTER_INT_TLE5206)
    #if defined(BOOSTER_INT_NDCC)
      if ((digitalRead(ShortIntPin) == LOW) && (Railpower != csShortCircuit)) {
    #else
    //---------------Old: without RailCom support----------------------------------
      if ((digitalRead(ShortIntPin) == LOW) && (digitalRead(GoIntPin) == BOOSTER_INT_ON) && (Railpower != csShortCircuit)) {
    #endif
        globalPower(csShortCircuit);
        #if defined(DEBUG)
        Debug.println(F("TRACK_SHORT_CIRCUIT_INT"));
        #endif
      }
  #endif

  #if defined(BOOSTER_INT_CURRENT_SHORT_DETECT)
    //Check if RailCom is on the rail?
    if (dcc.getRailComStatus() == false) {

      #if defined(ESP8266_MCU)
        //slow down the readings to not stop WIFI!
        uint16_t intervalTime = VAmpReadTimeNormal;
        //change reading time to detect ACK
        if (Railpower == csServiceMode)
          intervalTime = VAmpReadTimeService;
        //request only if we need to:
        if (millis() - VAmpLastMillis >= intervalTime) {
          VAmpLastMillis = millis();
          VAmpINT = analogRead(VAmpIntPin);   //don't middle the sense! Do not read so often!!!
        }
      #else //other MCU
          //Middle the current sense pin
      CalcVAmpINT += analogRead(VAmpIntPin);
      CountVAmpINT += 1;
      if (CountVAmpINT > VAmpINTMesureCount) {
        VAmpINT = CalcVAmpINT / CountVAmpINT;
        CalcVAmpINT = 0;
        CountVAmpINT = 0;
      }
      #endif

      if ((VAmpINT >= DETECT_SHORT_INT_VALUE) && (Railpower != csShortCircuit) ) {
        ShortTimeINT++;
        if (ShortTimeINT == DETECT_SHORT_INT_WAIT) {
          globalPower(csShortCircuit);
          #if defined(DEBUG)
          Debug.print(VAmpINT);
          Debug.print("-t");
          Debug.print(ShortTimeINT);
          Debug.println(F(" TRACK_SHORT_SENCE_INT"));
          #endif
        }
      }
      else ShortTimeINT = 0;
    }
    #endif
  #endif
}
#endif

//----------------------
uint8_t Read_Z21Button()
//----------------------
// Return 0 if the button is pressed
{
  #if defined(Z21ButtonPinAnalogOnly)
    #if 1 // If this is disabled the WLAN gets disconnected after a while ?!?
          // I'm not the only one: https://github.com/esp8266/Arduino/issues/1634
          // The analogRead() can't be called to often:
          //   https://arduino-esp8266.readthedocs.io/en/latest/reference.html
       static uint8_t Button = 0;
       static uint32_t NextCheck = 0;
       if (millis() > NextCheck)
          {
          NextCheck = millis() +100;
          Button = analogRead(Z21ButtonPin) > 500;
          }
       return Button;
    #else
       return analogRead(Z21ButtonPin) > 500;
    #endif
  #else
    return digitalRead(Z21ButtonPin);
  #endif
}

//--------------------------------------------------------------------------------------------
void updateLedButton() {
  static uint32_t ButtonPressTime = 0;
  //Button to control Railpower state
  if ((Read_Z21Button() == LOW) && (Z21ButtonLastState == 0)) {  //Button DOWN
       Z21ButtonLastState = 1;
       LEDcount = ButtonPressTime = millis();
       }
  else {
       // Improved long press detection. Now the long press action is directly performed                      // 06.02.22:
       // when the button is hold for longer than 750 ms.
       // Prior the action was performed when the button was released.
       uint8_t ActButton = Read_Z21Button();
       if (((ActButton == HIGH) || (millis() - ButtonPressTime > 750))  // Button UP or long pressed
          && (Z21ButtonLastState == 1))
          {
          Z21ButtonLastState = 2; // New state: Button processed => The following part is only processed once until the button is released
          #if defined(DEBUG)
              Debug.print(F("Button "));
          #endif
          if (Railpower == csNormal) {
               if(millis() - ButtonPressTime > 750) { //push long?
                    if (FIXSTORAGE.read(52) == 0x00)   //Power-Button (short): 0=Gleisspannung aus, 1=Nothalt
                         globalPower(csEmergencyStop);
                    else globalPower(csTrackVoltageOff);
                    }
               else {
                    if (FIXSTORAGE.read(52) == 0x00)  //Power-Button (short): 0=Gleisspannung aus, 1=Nothalt
                         globalPower(csTrackVoltageOff);
                    else globalPower(csEmergencyStop);
                    }
               }
          else globalPower(csNormal);
          LEDcount = millis();
          }

       if (Z21ButtonLastState == 2)                                                                           // 14.07.22:
          {
          uint32_t t  = millis() - ButtonPressTime;
          if (t > 5000)
             {
             Z21ButtonLastState = 3;
             globalPower(csTrackVoltageOff);
             //Dprintf("Button pressed verry long %lu\n", t);
             Beep(300);
             Read_Lok_Config_from_CAN(&can);
             }
          }
       if (ActButton == HIGH) Z21ButtonLastState = 0; // Button released
       }

  #if defined(DCCLed)                                                                                         // 16.01.22:
      //Update LED
      if (Railpower == csNormal) {
           static uint32_t NextUpd = 0;
           if (millis() > NextUpd) // Update the LED state not permanently to be able to                      // 06.02.22:
              {                     // show the receiving of IR-Signals with the same LED
              NextUpd = millis() + 300;
              digitalWrite(DCCLed, !POWER_LED_INVERT);
              }
           }
      else {  //Flash:
           unsigned long currentMillis = millis();
           if (currentMillis > LEDcount) {
             if (Railpower == csTrackVoltageOff) {
                  if (digitalRead(DCCLed) != POWER_LED_INVERT)
                       LEDcount = currentMillis + 1100;  //long OFF
                  else LEDcount = currentMillis + 300;   //short ON
                  }
             else if (Railpower == csEmergencyStop) {
                  if (digitalRead(DCCLed) != POWER_LED_INVERT)
                       LEDcount = currentMillis + 80;    //short OFF
                  else LEDcount = currentMillis + 700;   //long ON
                  }
             else if (Railpower == csShortCircuit)
                       LEDcount = currentMillis + 200;   //short flash
                  else LEDcount = currentMillis + 50;    //short flash
                  digitalWrite(DCCLed, !digitalRead(DCCLed));
                  }
           }
      #endif // DCCLed
}

//--------------------------------------------------------------------------------------------
#if defined(HTTPCONF) && defined(LAN)
void Webconfig() {
  EthernetClient client = server.available();
  if (client) {
    String receivedText = String(50);
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (receivedText.length() < 50) {
          receivedText += c;
        }
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
          //client.println(F("Refresh: 5"));  // refresh the page automatically every 5 sec
          client.println();   //don't forget this!!!
          //Website:
          client.println(F("<!DOCTYPE html><html><head>"));
          client.println(F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>"));
          client.println(F("<title>Z21</title></head><body><h1>Z21</h1>"));
          //----------------------------------------------------------------------------------------------------
          int firstPos = receivedText.indexOf("?");
          if (firstPos > -1) {
            byte lastPos = receivedText.indexOf(" ", firstPos);
            String theText = receivedText.substring(firstPos+3, lastPos); // 10 is the length of "?A="
            byte S88Pos = receivedText.indexOf("S=");
            #if defined(S88N)
              S88Module = receivedText.substring(S88Pos+2, receivedText.indexOf("HTTP")-1).toInt();
            #endif
            byte Sip = theText.indexOf("A="); //Start IP, nur wenn DHCP aktiv ist Ausdruck vorhanden!
            if (Sip == 0xFF) {  //DHCP off
              LAN_DHCP = false;
              Sip = 0;
            }
            else {
              LAN_DHCP = true;
              Sip += 2;
            }
            byte Aip = theText.indexOf("&B=", Sip);
            byte Bip = theText.indexOf("&C=", Aip);
            byte Cip = theText.indexOf("&D=", Bip);
            byte Dip = theText.substring(Cip+3, S88Pos).toInt();
            Cip = theText.substring(Bip+3, Cip).toInt();
            Bip = theText.substring(Aip+3, Bip).toInt();
            Aip = theText.substring(Sip, Aip).toInt();
            LAN_ip[0] = Aip;
            LAN_ip[1] = Bip;
            LAN_ip[2] = Cip;
            LAN_ip[3] = Dip;
            #if defined(DEBUG)
            #if defined(S88N)
              Debug.print("S88: ");
              Debug.println(S88Module);
            #endif
            if (LAN_DHCP)
              Debug.print("DHCP ");
            Debug.print("IP: ");
            Debug.println(LAN_ip);
            #if !defined(SOFT_RESET)
            Debug.println(F("-> Restart to accept!"));
            #endif
            #endif

            #if !defined(SOFT_RESET)
            client.print(F("<dialog open><p>Reset Z21 to accept!</p></dialog>"));
            #endif

            FIXSTORAGE.FIXMODE(EELANDHCP, LAN_DHCP);
            FIXSTORAGE.FIXMODE(EELANip, Aip);
            FIXSTORAGE.FIXMODE(EELANip+1, Bip);
            FIXSTORAGE.FIXMODE(EELANip+2, Cip);
            FIXSTORAGE.FIXMODE(EELANip+3, Dip);

            #if defined(S88N)
            if (FIXSTORAGE.read(EES88Moduls) != S88Module) {
              FIXSTORAGE.write(EES88Moduls, S88Module);
              #if defined(DEBUG)
              Debug.print("neu S88: ");
              Debug.println(S88Module);
              #endif
              SetupS88();
              #if defined(WIFI)
                WLANSetup();
              #endif
            }
            else {
            #endif

            #if defined(SOFT_RESET)
            soft_restart();     //Reboot MCU!
            #endif

            #if defined(S88N)
            }
            #endif
          }
          //----------------------------------------------------------------------------------------------------
          client.print(F("<form method=get>"));
          client.print(F("DHCP <input type=checkbox name=P"));
          if (LAN_DHCP)
            client.print(" checked");
          client.print(F("><br>"));
          client.print(F("IP: <input type=number min=0 max=254 name=A value="));
          client.println(LAN_ip[0]);
          client.print(F("><input type=number min=0 max=254 name=B value="));
          client.println(LAN_ip[1]);
          client.print(F("><input type=number min=0 max=254 name=C value="));
          client.println(LAN_ip[2]);
          client.print(F("><input type=number min=0 max=254 name=D value="));
          client.println(LAN_ip[3]);
          client.print(F("><br><br>8x S88 Module: <input type=number min=0 max="));
          #if defined(S88N)
          client.print(S88MAXMODULE);
          #else
          client.print("0");
          #endif
          client.print(F(" name=S value="));
          #if defined(S88N)
            client.print(S88Module);
          #else
            client.print("-");
          #endif
          client.print(F("><br><br><input type=submit></form></body></html>"));
          break;
        }
        if (c == '\n')
          currentLineIsBlank = true; // you're starting a new line
        else if (c != '\r')
          currentLineIsBlank = false; // you've gotten a character on the current line
      }
    }
    client.stop();  // close the connection:
  }
}
#endif

//--------------------------------------------------------------------------------------------
#if (defined(ESP8266_MCU) || defined(ESP32_MCU)) && defined(ESP_HTTPCONF)

void Webconfig() {
  WiFiClient client = ESPWebserver.available();
  if (!client)
    return;

  String HTTP_req;            // stores the HTTP request

  if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    client.println();      //don't forget this!!!
                    // AJAX request for switch state
                    if (HTTP_req.indexOf("/ajax_switch") > -1) {
                        #if defined(DEBUG)
                        Debug.println("Reset WLAN conf!");
                        #endif
                        // read switch state and send appropriate paragraph text
                        ssid = HTTP_req.substring(HTTP_req.indexOf("&s=")+3,HTTP_req.indexOf("&p="));
                        pass = HTTP_req.substring(HTTP_req.indexOf("&p=")+3,HTTP_req.indexOf("&As="));
                        ssidAP = HTTP_req.substring(HTTP_req.indexOf("&As=")+4,HTTP_req.indexOf("&Ap="));
                        passAP = HTTP_req.substring(HTTP_req.indexOf("&Ap=")+4,HTTP_req.indexOf("&Ak="));
                        kanalAP = HTTP_req.substring(HTTP_req.indexOf("&Ak=")+4,HTTP_req.indexOf("&S8=")).toInt();
                        #if defined(S88N)
                          S88Module = HTTP_req.substring(HTTP_req.indexOf("&S8=")+4,HTTP_req.indexOf("&nocache")).toInt();
                        #endif

                        if ((kanalAP < 1) || (kanalAP > 13)) {
                          kanalAP = SkanalAP;
                          client.print("Ka. error! ");
                        }
                        if (passAP.length() < 8) {
                          passAP = SpassAP;
                          client.print("Code length error (min. 8)! ");
                        }

                        // write eeprom
                        EEPROMwrite (ssid, EEssidLength, EEssidBegin);
                        EEPROMwrite (pass, EEpassLength, EEpassBegin);

                        EEPROMwrite (ssidAP, EEssidAPLength, EEssidAPBegin);
                        EEPROMwrite (passAP, EEpassAPLength, EEpassAPBegin);
                        FIXSTORAGE.write(EEkanalAP, kanalAP);

                        FIXSTORAGE.commit();

                        ESPSetup();

                        Udp.begin(z21Port);

                        client.println("saved");   //OK!
                    }
                    else {  // HTTP request for web page
                        // send web page - contains JavaScript with AJAX calls
                        client.println("<!DOCTYPE html>");
                        client.println("<html><head><title>Z21</title>");
                        client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>");
                        client.println("<script>");
                        client.println("function SetState() {");
                        client.println("document.getElementById(\"state\").innerHTML = \"wait\";");
                        client.println("nocache = \"&s=\" + document.getElementById(\"ssid\").value;");
                        client.println("nocache += \"&p=\" + document.getElementById(\"pass\").value;");
                        client.println("nocache += \"&As=\" + document.getElementById(\"ssidAP\").value;");
                        client.println("nocache += \"&Ap=\" + document.getElementById(\"passAP\").value;");
                        client.println("nocache += \"&Ak=\" + document.getElementById(\"kanalAP\").value;");
                        client.println("nocache += \"&S8=\" + document.getElementById(\"S88\").value;");
                        client.println("nocache += \"&nocache=\" + Math.random() * 1000000;");
                        client.println("var request = new XMLHttpRequest();");
                        client.println("request.onreadystatechange = function() {");
                        client.println("if (this.readyState == 4){");
//                        client.println("if (this.status == 200){");
//                        client.println("if (this.responseText != null) {");
                        client.println("document.getElementById(\"state\").innerHTML = this.responseText;");
                        client.println("top.window.location.reload(true);");
                        client.println("}}");
                        client.println("request.open(\"GET\", \"ajax_switch\" + nocache, true);");
                        client.println("request.send(null);");
                        //client.println("setTimeout('SetState()', 1000);");
                        client.println("}");
                        client.println("</script>");
                        client.println("</head>");
                        client.println("<body><h1>Z21 Einstellungen</h1><hr>");
                        client.print("<h2>WiFi Access-Point</h2>");
                        client.print("<dl><dd>IP: ");
                        client.print(WiFi.softAPIP());
                        client.print("</dd><dd>Connected Clients: ");
                        client.print(WiFi.softAPgetStationNum());
                        client.print(" of 8</dd><dd>SSID: <input type=\"text\" id=\"ssidAP\" value=\"");
                        client.print(ssidAP);
                        client.print("\"></dd><dd>code: <input type=\"text\" id=\"passAP\" value=\"");
                        client.print(passAP);
                        client.print("\"></dd><dd>Ka.: <input type=\"number\" min=\"1\" max=\"13\" id=\"kanalAP\" value=\"");
                        client.print(kanalAP);
                        client.println("\"></dd></dl>");

                        client.print("<h2>WiFi Client</h2>");
                        client.print("<dl><dd>IP: ");
                        if (WiFi.status() == WL_CONNECTED)
                          client.print(WiFi.localIP());
                        else client.print("none");
                        client.print("</dd><dd>SSID: <input type=text id=\"ssid\" value=\"");
                        client.print(ssid);
                        client.print("\"></dd><dd>Secret Key: <input type=password id=\"pass\" value=\"");    // 04.07.22:
                      //client.print("\"></dd><dd>code: <input type=text id=\"pass\" value=\"");              //   "       Old
                      //client.print(pass);                                                                   //   "       Disabled
                        client.println("\"></dd></dl>");

                        client.println("<h2>S88 Module</h2>");
                        client.print("<dl><dd>8x Anzahl: <input type=number min=\"0\" max=\"62\" id=\"S88\" value=\"");
                        #if defined(S88N)
                          client.print(S88Module);
                          client.print("\"");
                        #else
                          client.print("0\" disabled");
                        #endif
                        client.println("></dd></dl><br>");

                        client.println("<input type=submit onclick=\"SetState()\">");
                        client.println("<p id=\"state\"></p>");
                        client.print("<hr><p>Z21_ESP_Central_Station_v");
                        client.print(Z21mobileSwVer);
                        client.print(SwitchFormat);
                        #if defined (BOOSTER_INT_NDCC)
                        if (FIXSTORAGE.read(EEPROMRailCom) == 0x01)
                          client.print(".RAILCOM");
                        #endif
                        client.println("<br>Copyright (c) 2022 Philipp Gahtow<br>digitalmoba@arcor.de</p>");
                        client.println("</body>");
                        client.print("</html>");
                    }
                    // display received HTTP request on serial port
                    //Serial.print(HTTP_req);
                    HTTP_req = "";            // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                }
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}
#endif

#if defined (__cplusplus)
  extern "C" {
#endif

//--------------------------------------------------------------
//handle Lok Data to request central for all Interfaces to request Status on Master
void AllLocoData(uint16_t adr, uint8_t *data) {
//uint8_t Steps[0], uint8_t Speed[1], uint8_t F0[2], uint8_t F1[3], uint8_t F2[4], uint8_t F3[5]

  //We are LocoNet Slave user?
  #if defined(LOCONET) && !defined(ESP32_MCU) //handle ESP32 with call back function!
    #if !defined(LnSLOTSRV)
    LNGetSetLocoSlot(adr,false);
    LNupdate();      //LocoNet update
    #if defined(REQEST)
    Debug.print(F("Ln Slave "));
    #endif
    #endif
  #endif

  //We are XpressNet Slave user?
  #if defined(XPRESSNET)
    if (XpressNet.getOperationModeMaster() == false) {
      XpressNet.getLocoInfo(adr);
      XpressNet.update(); //XpressNet Update
      #if defined(REQEST)
      Debug.print(F("Xn Slave "));
      #endif
    }
  #endif

  #if defined(DCC)
  dcc.getLocoData(adr, data);
  #endif

  #if defined(REQEST)
  Debug.print(F("LOK Data "));
  Debug.print(adr);
  Debug.print("-");
  Debug.print(data[1]); //speed
  Debug.print("-F0:");
  Debug.println(data[2], BIN); //F0
  #endif
}
#if defined (__cplusplus)
             }
#endif

//--------------------------------------------------------------
//DCC handle back the request switch state
void notifyTrnt(uint16_t Adr, bool State, bool active)
{
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setTrntInfo(Adr, State);
  #endif

  #if defined(LOCONET)
  LNsetTrnt(Adr, State, active);
  #endif
  #if defined(XPRESSNET)
  XpressNet.SetTrntPos(Adr, State, active);
  #endif

  #if defined(REQEST)
  Debug.print(F("DCC Trnt "));
  Debug.print(Adr);
  Debug.print("-");
  Debug.print(State);
  Debug.print("-");
  Debug.println(active);
  #endif
}

//--------------------------------------------------------------
//DCC return a CV value:
void notifyCVVerify(uint16_t CV, uint8_t value) {
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setCVReturn (CV, value);
  #endif

  #if defined(XPRESSNET)
  XpressNet.setCVReadValue(CV, value);
  #endif

  #if defined(LOCONET)
  LNsetCVReturn(CV, value, 0);  //CV Read Okay
  #endif

  #if defined(REQEST)
  Debug.print(F("Verify CV#"));
  Debug.print(CV+1);
  Debug.print(" - ");
  Debug.print(value);
  Debug.print(" b");
  Debug.println(value, BIN);
  #endif
}

//--------------------------------------------------------------
//DCC return no ACK:
void notifyCVNack(uint16_t CV) {
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setCVNack();  //send back to device and stop programming!
  #endif

  #if defined(XPRESSNET)
  XpressNet.setCVNack();
  #endif

  #if defined(LOCONET)
  LNsetCVReturn(CV, 0, 1);  //CV Read Fails
  #endif

  #if defined(REQEST)
  Debug.print("CV#");
  Debug.print(CV+1);
  Debug.println(F(" no ACK"));
  #endif
}

//--------------------------------------------------------------
//Feedback the Current Sence value
#if defined(BOOSTER_INT_CURRENT_SHORT_DETECT)
uint16_t notifyCurrentSence() {
  return VAmpINT;
}
#endif

//--------------------------------------------------------------
//Reset the EEPROM to default:
void EEPROM_Load_Defaults() {

  #if defined(ESP32)
  portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
  portENTER_CRITICAL(&myMutex);
  #endif

  #if defined(DEBUG)
     Debug.println(F("Set all EEPROM to default!"));
  #endif

  FIXSTORAGE.FIXMODE(EEPROMRailCom, 1); //RailCom ON
  FIXSTORAGE.FIXMODE(EEPROMRSTsRepeat, 25); //Reset Pakete (start)
  FIXSTORAGE.FIXMODE(EEPROMRSTcRepeat, 12); //Reset Pakete (fortsetzen)
  FIXSTORAGE.FIXMODE(EEPROMProgRepeat, 12); //Programmier Pakete

  FIXSTORAGE.FIXMODE(EES88Moduls, 62);   //S88 max Module
  //IP Werkseinstellung:
  FIXSTORAGE.FIXMODE(EELANDHCP,   0);  //kein DHCP
  FIXSTORAGE.FIXMODE(EELANip,   192);
  FIXSTORAGE.FIXMODE(EELANip+1, 168);
  FIXSTORAGE.FIXMODE(EELANip+2,   0);
  FIXSTORAGE.FIXMODE(EELANip+3, 111);

  //Default VCC Rail and Prog to 20V:
  FIXSTORAGE.FIXMODE(72, 32);
  FIXSTORAGE.FIXMODE(73, 78);
  FIXSTORAGE.FIXMODE(74, 32);
  FIXSTORAGE.FIXMODE(75, 78);

  //Default Prog option:
  FIXSTORAGE.FIXMODE(52, 0); //Power Button behaviour
  FIXSTORAGE.FIXMODE(53, 3);

  //DCCInterfaceMaster:
  FIXSTORAGE.FIXMODE(50, 1); //RailCom activ
  FIXSTORAGE.FIXMODE(60, 25);  //RESET_START_REPEAT
  FIXSTORAGE.FIXMODE(61, 12);  //RESET_CONT_REPEAT
  FIXSTORAGE.FIXMODE(62, 12);  //PROGRAMMING_REPEAT

  //Z21 Library:
  FIXSTORAGE.FIXMODE(1, 0xE8); //Z21 Serial LSB
  FIXSTORAGE.FIXMODE(0, 0x01); //Z21 Serial MSB

  #if defined(ESP_WIFI)
  FIXSTORAGE.commit();
  #endif

  #if defined(ESP32)
  portEXIT_CRITICAL(&myMutex);
  #endif
}

//--------------------------------------------------------------
//Check the firmware status need update?:
void EEPROM_Setup() {
  #if defined(DEBUG)
  Debug.print(F("Check EEPROM..."));
  #endif


  byte SwVerMSB = FIXSTORAGE.read(EEPROMSwVerMSB);
  byte SwVerLSB = FIXSTORAGE.read(EEPROMSwVerLSB);

  //Check if we already run on this MCU?
  if ((SwVerMSB == 0xFF) && (SwVerLSB == 0xFF))
    {
    //First Startup, set everything to default!
    EEPROM_Load_Defaults();
    }

  //Check if there need to handel an update?
  if ( (SwVerMSB != highByte(Z21mobileSwVer)) || (SwVerLSB != lowByte(Z21mobileSwVer)) ) {
    //Update to new Firmware
    #if defined(DEBUG)
    Debug.println(F("Firmware Update!"));
    #endif
/*
    #if defined(ESP32)
    portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&myMutex);
    #endif
*/
    FIXSTORAGE.FIXMODE(EEPROMSwVerMSB, highByte(Z21mobileSwVer));
    FIXSTORAGE.FIXMODE(EEPROMSwVerLSB, lowByte(Z21mobileSwVer));

    #if defined(ESP_WIFI)
    FIXSTORAGE.commit();
    #endif
/*
    #if defined(ESP32)
    portEXIT_CRITICAL(&myMutex);
    #endif
    */
  }
  else {
    #if defined(DEBUG)
      Debug.println(F("OK"));
    #endif
  }
  #if defined(ESP32)
  yield();
  #endif
}


//--------------------------------------------------------------
//INIT all ports and interfaces:
void setup() {

  //Reduce the reference Voltage to 1,1 Volt!
  #if defined(AREF_1V1) && !defined(ESP32_MCU) && !defined(ESP8266_MCU)
    analogReference(AREF_1V1); //EXTERNAL or INTERNAL1V1
  #endif

  #if defined(DCCLed)                                                                                       // 16.01.22:
    pinMode(DCCLed, OUTPUT);      //DCC Status LED
    #if defined(POWER_LED_INVERT)
      digitalWrite(DCCLed, HIGH);    //DCC LED is in "off" State
    #else
      digitalWrite(DCCLed, LOW);    //DCC LED is in "off" State
    #endif
  #endif
  #ifdef ShortLed
    pinMode(ShortLed, OUTPUT);    //Short Status LED
    digitalWrite(ShortLed, HIGH);    //Short LED showes working and Power up
  #endif
  #ifdef ProgRelaisPin
    pinMode(ProgRelaisPin, OUTPUT);       //ProgTrack-Relais
    #if defined(PROG_OUT_INVERT)
      digitalWrite(ProgRelaisPin, HIGH);     //ProgTrack
    #else
      digitalWrite(ProgRelaisPin, LOW);     //ProgTrack
    #endif
  #endif
  #if defined(BOOSTER_EXT)    //Booster (ROCO) external:
    pinMode(ShortExtPin, INPUT_PULLUP);  //set short pin and Turn on internal Pull-Up Resistor
    pinMode(GoExtPin, OUTPUT);      //GO/STOP Signal
    digitalWrite(GoExtPin, BOOSTER_EXT_OFF);    //set STOP to Booster
  #endif
  #if defined(BOOSTER_INT)    //Booster2 internal:
    #if !defined(BOOSTER_INT_NDCC)
      pinMode(GoIntPin, OUTPUT);    //GO/STOP2 Signal
      digitalWrite(GoIntPin, BOOSTER_INT_OFF);   //set STOP to Booster2 invertet
    #endif
    #if defined(BOOSTER_INT_TLE5206) || defined(BOOSTER_EXT_CDE)
      pinMode(ShortIntPin, INPUT_PULLUP);  //set up short2 PIN and Turn on internal Pull-Up Resistor
    #endif
  #endif
  pinMode(Z21ResetPin, INPUT_PULLUP); //Turn on internal Pull-Up Resistor

  #if !defined(Z21ButtonPinAnalogOnly)
      pinMode(Z21ButtonPin, INPUT_PULLUP); //Turn on internal Pull-Up Resistor
  #endif

  #if defined(MEGA_MCU)
  //pinMode(VAmSencePin, INPUT_PULLUP);  //AC 5A Sensor (for testing only)
  #if defined(AREF_1V1)
      #if defined(VoltIntPin)
        pinMode(VoltIntPin, INPUT);  //Rail Voltage: Rail:100k - Sence - 4,7k - GND
      #endif
      #if !defined(DALLASTEMPSENSE)
        #if defined(TempPin)
          pinMode(TempPin, INPUT);     //Temp.Resistor(15k)
        #endif
      #endif
    #else
      #if defined(VoltIntPin)
        pinMode(VoltIntPin, INPUT_PULLUP);  //Rail Voltage: Rail:100k - Sence - 4,7k - GND
      #endif
      #if !defined(DALLASTEMPSENSE)
        #if defined(TempPin)
          pinMode(TempPin, INPUT_PULLUP);     //Temp.Resistor(15k)
        #endif
      #endif
    #endif
  #endif

  #if defined(HEARTBEATLED_PIN)                                                                               // 16.01.22:
      pinMode(HEARTBEATLED_PIN, OUTPUT);
  #endif

  #if defined(SOFT_RESET)   //Arduino UNO, MEGA, Sangduino ONLY!
    MCUSR = 0;
    wdt_disable();
  #endif

   DisplaySetup();    //Display init

  #if defined(DebugBaud)
    Debug.begin(DebugBaud);
    #if defined(ESP8266_MCU) || defined(ESP32_MCU)
      Debug.println();  //Zeilenumbruch einfügen
    #endif
    Debug.print("Program started Sw-Build:"); Debug.println(comp_date);
  #endif

  #if defined(ESP_WIFI)
    FIXSTORAGE.begin(EESize);  //init EEPROM
    Debug.print("EEPROM initialized Size:"); Debug.println(EESize);
  #endif


    //Check Firmware in EEPROM:
  EEPROM_Setup();   //init Z21 EEPROM defaults

  #if defined(ESP_WIFI)
    Display_Boot_Start_Display("Start Wifi");
    Dprintf("Starting ESPSetup\n");
    ESPSetup();   //ESP8266 and ESP32 WLAN Setup  (Duration 3.803 sec)
    Dprintf("ESPSetup O.K.\n");
    Display_Boot_Start_Display("Boot");
  #endif


  #if defined(DebugBaud)
    Debug.print(F("Z21PG v"));
    Debug.print(String(Z21mobileSwVer).substring(0,1));
    Debug.print(".");
    Debug.print(String(Z21mobileSwVer).substring(1));
    Debug.print(SwitchFormat);

    #if defined(UNO_MCU)
    Debug.print(F(" - UNO"));
    #elif defined(MEGA_MCU)
    Debug.print(F(" - MEGA"));
    #elif defined(SANGUINO_MCU)
    Debug.print(F(" - SANGUINO"));
    #elif defined(DUE_MCU)
    Debug.print(F(" - DUE"));
    #elif defined(ESP8266_MCU)
    Debug.print(F(" - ESP8266"));
    #elif defined(ESP32_MCU)
    Debug.print(F(" - ESP32"));
    #endif

    #if defined (BOOSTER_INT_NDCC)
      if (FIXSTORAGE.read(EEPROMRailCom) == 0x01)
        Debug.print(".RAILCOM");
    #endif
    #if defined(ACKBOOSTER)
      Debug.print(".ACK_BOOST");
    #endif
    #if defined(AREF_1V1)
      Debug.print(".AREF_1V1");
    #endif
  #endif

  #if defined(ESP_WIFI)
    delay(10);
  #endif

  DisplayBoot();

  #if defined(DCC)
    //setup the DCC signal:
    #if defined(BOOSTER_INT_NDCC)
      #if defined(FS14)
      dcc.setup(DCCPin, GoIntPin, DCC14, SwitchFormat, Railpower);
      #elif defined(FS28)
      dcc.setup(DCCPin, GoIntPin, DCC28, SwitchFormat, Railpower);
      #else
      dcc.setup(DCCPin, GoIntPin, DCC128, SwitchFormat, Railpower);
      #endif
    #else
      #if defined(FS14)
      dcc.setup(DCCPin, 0, DCC14, SwitchFormat, Railpower);  //no NDCC and no RAILCOM
      #elif defined(FS28)
      dcc.setup(DCCPin, 0, DCC28, SwitchFormat, Railpower);  //no NDCC and no RAILCOM
      #else
      dcc.setup(DCCPin, 0, DCC128, SwitchFormat, Railpower);  //no NDCC and no RAILCOM
      #endif
    #endif

    //first read a value to mesure the power on the track
    #if defined(BOOSTER_INT_CURRENT_SHORT_DETECT)
      VAmpINT = analogRead(VAmpIntPin);
    #endif
    //for CV reading over RAILCOM activate i2c communication:
    #if defined(DCCGLOBALDETECTOR)
      #if defined(MEGA_MCU)
      RailComSetup(); //init the Serial interface for receiving RailCom
      #endif
    #endif
    //extra DCC Output for S88 or LocoNet
    #if defined(additionalOutPin)
      #if (!defined(LAN) && defined (UNO_MCU)) || defined(MEGA_MCU) || defined(DUE_MCU) || defined(ESP_WIFI)
        dcc.enable_additional_DCC_output(additionalOutPin);
        #if defined(DebugBaud)
          Debug.print(".addOutput");
        #endif
      #endif
    #endif
  #endif

  #if defined(DebugBaud)
    Debug.println();  //finish config line!
    Debug.print(F("Build: "));
    Debug.println(comp_date);
  #endif

  #if defined(LAN)
  //Want to RESET Ethernet to default IP?     // ToDo: Warum wird hier nicht "Z21ResetPin" abgefragt
  if ((Read_Z21Button() == LOW) || (not( (FIXSTORAGE.read(EELANip) == 10) /*Class A */ || (FIXSTORAGE.read(EELANip) == 172) /*Class B */ || (FIXSTORAGE.read(EELANip) == 192)  /*Class C */  ))) {

      EEPROM_Load_Defaults();

      while (Read_Z21Button() == LOW) {  //Wait until Button - "UP"
        #if defined(DEBUG)
          Debug.print(".");
        #endif
        delay(200);   //Flash:
        #if defined(DCCLed)                                                                                       // 16.01.22:
        digitalWrite(DCCLed, !digitalRead(DCCLed));
        #endif
        #ifdef ShortLed
          digitalWrite(ShortLed, !digitalRead(ShortLed));                                                     // 13.01.22:  Old: !digitalRead(DCCLed));
        #endif
      }
      #if defined(DEBUG)
          Debug.println();  //new line!
      #endif
      #if defined(DCCLed)                                                                                       // 16.01.22:
          #if defined(POWER_LED_INVERT)
              digitalWrite(DCCLed, HIGH);    //DCC LED is in "off" State
          #else
              digitalWrite(DCCLed, LOW);     //DCC LED is in "off" State
          #endif
      #endif
      digitalWrite(ShortLed, LOW);    //Short LED is in "off" State
  }
  if (FIXSTORAGE.read(EELANDHCP) == 1)
    LAN_DHCP = true;
  LAN_ip[0] = FIXSTORAGE.read(EELANip);
  LAN_ip[1] = FIXSTORAGE.read(EELANip+1);
  LAN_ip[2] = FIXSTORAGE.read(EELANip+2);
  LAN_ip[3] = FIXSTORAGE.read(EELANip+3);
  #endif

  #if defined(S88N)
    SetupS88();    //S88 Setup
  #endif

  DisplayBoot();

  #if defined(XPRESSNET)
    #if defined(FS14)
    XpressNet.setup(Loco14, XNetTxRxPin);    //Initialisierung XNet Serial und Send/Receive-PIN
    #elif defined(FS28)
    XpressNet.setup(Loco28, XNetTxRxPin);    //Initialisierung XNet Serial und Send/Receive-PIN
    #else
    XpressNet.setup(Loco128, XNetTxRxPin);    //Initialisierung XNet Serial und Send/Receive-PIN
    #endif
    XpressNet.setPower(Railpower);  //send Railpowerinformation to XpressNet
  #endif

  #if defined(DECODER)
    DCCDecoder_init();    //DCC Decoder init
  #endif

  #if defined(LOCONET)
    LNsetup();      //LocoNet Interface init
    LNsetpower();   //send Railpowerinformation to LocoNet
  #endif

  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
    z21.setPower(Railpower);  //send Railpowerinformation to Z21 Interface
  #endif

  DisplayBoot();

  #if defined(WIFI)
    WLANSetup();    //Start ESP WLAN
  #endif

  #if defined(ESP_WIFI)
    #if defined(ESP_HTTPCONF)
      #if defined(DEBUG)
      Debug.print(F("Server Begin..."));
      #endif
      ESPWebserver.begin(); //Start the HTTP server
      #if defined(DEBUG)
      Debug.println("OK");
      #endif
    #endif
    #if defined(DEBUG)
    Debug.print(F("Start UDP listener..."));
    #endif
    Udp.begin(z21Port);   //open Z21 port
    #if defined(DEBUG)
    Debug.println("OK");
    #endif
    #if defined(ESP32)
    yield();
    #endif

    #if defined(ESP_OTA)
      #if defined(DEBUG)
      Debug.print(F("Init OTA..."));
      #endif
      ESP_OTA_Init();
      #if defined(DEBUG)
      Debug.println("OK");
      #endif
    #endif
  #endif

  DisplayBoot();

  #if defined(LAN)
    // start the Ethernet and UDP:
    delay(120); //wait for Ethernet Shield to get up (need longer to boot!)

    #if defined(ENC28)
     /* Disable SD card */
    pinMode(SDSSPIN, OUTPUT);
    digitalWrite(SDSSPIN, HIGH);
    #endif

    Ethernet.init(LANSSPIN);  //SS-Pin Most Arduino shields

    if (LAN_DHCP) {
      #if defined(DEBUG)
          Debug.print(F("IP over DHCP..."));
      #endif
      #if defined(ENC28)
      if (Ethernet.begin(mac) == 0) { //ENC28 UIP Lib doesn't have timeout option!
      #else
      if (Ethernet.begin(mac,LANTimeoutDHCP,2000) == 0) { //Trying to get an IP address using DHCP
      #endif
        #if defined(DEBUG)
          Debug.println(F("fail!"));
        #endif
        LAN_DHCP = false;   //DHCP not found!
      }
      else {
        //Save IP that receive from DHCP
        LAN_ip = Ethernet.localIP();
        if (LAN_DHCP) {
          //Store IP in EEPROM:
          FIXSTORAGE.FIXMODE(EELANip, LAN_ip[0]);
          FIXSTORAGE.FIXMODE(EELANip+1, LAN_ip[1]);
          FIXSTORAGE.FIXMODE(EELANip+2, LAN_ip[2]);
          FIXSTORAGE.FIXMODE(EELANip+3, LAN_ip[3]);
          #if defined(ESP_WIFI)
          FIXSTORAGE.commit();
          #endif
          #if defined(DEBUG)
          Debug.println("SAVE");
          #endif
        }
        #if defined(DEBUG)
        else Debug.println("OK");
        #endif
      }
    }
    if (!LAN_DHCP) { //kein DHCP:
      // initialize the Ethernet device not using DHCP:
      Ethernet.begin(mac,LAN_ip);  //set IP and MAC
    }
    Udp.begin(z21Port);  //UDP Z21 Port 21105

  //#if defined(Z21MT)
  //UdpMT.begin(34472);   //UDP Maintenance Tool 21106
  //#endif

    #if defined(HTTPCONF)
      server.begin();    //HTTP Server
    #endif
  #endif

  DisplayBoot();

  #if defined(DEBUG)
    #if defined(LAN)
    if (LAN_DHCP)
      Debug.print(F("DHCP "));
    Debug.print(F("Eth IP: "));
    Debug.println(LAN_ip);
    #endif
    #if defined(S88N)
      Debug.print(F("S88: "));
      Debug.println(S88Module);
    #endif
    #if !defined(DUE_MCU) //not for the DUE!
      Debug.print(F("RAM: "));
      Debug.println(freeRam());
    #endif
  #endif

  DisplayBoot();

  #if defined(DALLASTEMPSENSE)
  sensors.begin();
  sensors.getAddress(insideThermometer, 0);
  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, DALLAS_RESOLUTION);
  sensors.setWaitForConversion(false);  //kein Warten auf die Umwandlung!

  sensors.requestTemperatures();  //1. Abfrage der Temperatur
  #endif

  DisplayStart();  //Aktualisierung!

  #ifdef ShortLed
    digitalWrite(ShortLed, LOW);    //Short LED goes off - we are ready to work!
  #endif

  EEPROM_Loco_setup();

  #if defined(IR_RECEIVE_PIN)
      //IR_Key_Input.IR_Test(); // Debug
      IR_Functions_setup();
  #endif

  #if defined(USE_CAN)                                                                                        // 18.01.22:
      CAN_setup();
  #endif
}

//--------------------------------------------------------------------------------------------
//run the state machine to update all interfaces
void loop() {
  #if defined(USE_CAN)                                                                                        // 18.01.22:
      CAN_loop();
  #endif

  updateLedButton();     //DCC Status LED and Button

  #if defined(DCC)
  ShortDetection();  //handel short on rail to => power off
  dcc.update();    //handel Rail Data

    #if defined(DCCGLOBALDETECTOR) && defined(MEGA_MCU)
    RailComRead();  //check RailCom Data
    #endif

  #endif

  #if (defined(HTTPCONF) && defined(LAN)) || ((defined(ESP8266_MCU) || defined(ESP32_MCU)) && defined(ESP_HTTPCONF))
    Webconfig();    //Webserver for Configuration
  #endif

  #if defined(S88N)
    notifyS88Data();    //R-Bus geänderte Daten 1. Melden
  #endif

  #if defined(DECODER)
    DCCDecoder_update();    //Daten vom DCC Decoder
  #endif

  #if defined(XPRESSNET)
    XpressNet.update(); //XpressNet Update
  #endif

  #if defined(LOCONET) && !defined(ESP32_MCU) //handle ESP32 with call back function!
    LNupdate();      //LocoNet update
  #endif

  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
    Z21LANreceive();   //Z21 LAN Decoding
  #endif
  /*
  #if defined(LAN) && defined(Z21MT)
    Z21MTreceive();
  #endif
  */
  #if defined(LAN)
     if (LAN_DHCP) {
        Ethernet.maintain(); //renewal of DHCP leases
     }
  #endif

  #if defined(ESP_OTA)
    ArduinoOTA.handle();
  #endif

  SerialCmd_loop(); // Only active if USE_SERIAL_COMMANDS is 1                                                // 25.01.22:

  #if defined(IR_RECEIVE_PIN)
     IR_Functions_loop();
  #endif

  OLED_loop();
}
