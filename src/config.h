#ifndef __CONFIG_H__
#define __CONFIG_H__

/************************************************************************************
 Z21 Ethernet DCC Command Station Interface config file
 Copyright (c) 2015-2022 by Philipp Gahtow
***********************************************************************************/
/*-------------**********************************-----------------------------------
/ ************* CHANGE TO SET UP YOUR INDIVIDUAL CONFIGURATION ***************
/ -------------**********************************-----------------------------------*/
/* => uncomment ("//" or #undef) the following lines, if you not USE the Interface! */
/*----------------------------------------------------------------------------------*/

//#define S88N    //S88N Interface (max 62 * 8 Module)                     // 16.01.22: Disabled

#define WIFI  //WiFi Interface über Serial-Port für Arduino UNO/MEGA/DUE zum ESP 8266
//#define WLAN Serial2  // (DEFAULT = Serial2!) use not the standard WLAN Serial Interface. Select another serial:
//#define Z21VIRTUAL  // WiFi over SoftSerial for UNO only! - LAN and LocoNet will be inaktiv!

//#define LAN       //LAN Ethernet Z21 LAN Kommunikation mit W5100 oder ENC28 Ethernet Shield. Bitte diese IP nur über die Webseite (http://192.168.0.111) ändern! (not for ESP8266 and ESP32)
//#define ENC28     //USE a ENC28J60 module - instead of w5100 Shield (MEGA only!)
//#define LANmacB2 0xEF   //Byte2 (DEFAULT = 84:2B:BC:EF:FE:ED!) change optional LAN MAC Address. MAC starts with: „84:2B:BC:..Byte*2..:..Byte*1..:..Byte*0..“
//#define LANmacB1 0xFE   //Byte1
//#define LANmacB0 0xED   //Byte0
//#define LANTimeoutDHCP 10000 //(DEFAULT!) Timeout to wait for a DHCP respose (Fix default Time: 5 sec)

//#define DCCGLOBALDETECTOR  //DCC Railcom Global Detector for MEGA on Serial Port 3 (RX only) - (not for ESP8266 and ESP32)       // 17.01.22:  Disabled

//#define XPRESSNET   //XpressNet Auto Master/Salve Interface (not for ESP8266 and ESP32)                                          // 17.01.22:  Disabled

//#define LOCONET     //LocoNet Interface (Timer1, Timer5 on MEGA, with LocoNet2 Library on ESP32)                                 // 17.01.22:  Disabled
//#define LnSLOTSRV   //LocoNet Master-Mode: provide a Slot Server for Loco to use FRED & DaisyII                                  // 17.01.22:  Disabled
//#define LnBufferUSB  //LocoNet LocoBuffer-USB at 57600 bps (Achtung: kein Debug über Serial Monitor möglich!)                  // 17.01.22:  Disabled

//#define BOOSTER_EXT //External Booster Interface (zB. ROCO, CD[E])

//#define BOOSTER_INT //internal Booster Interface (zB. TLE5206)
//#define BOOSTER_INT_MAINCURRENT     //Standard Short Circuit Detection over current sence resistor (VAmpIntPin) activate the current sensor for prog track and SHORT CIRCUIT SENCE over MAINCURRENT
//#define DETECT_SHORT_INT_WAIT  20   //Time after internal short circuit is detected (Fix default Value = 3)
//#define DETECT_SHORT_INT_VALUE 1000 //analogRead value for "mA" that is too much (Fix default Value = 400 (AREF 5.0V) and Value = 1000 (AREF 1.1V)

/* (DEFAULT OFF!) activate Short2 detection PIN reading use: (only one can be active!)*/
//#define BOOSTER_INT_TLE5206    //internal Booster with TLE5206
//#define BOOSTER_EXT_CDE        //external CDE Booster

//#define PROG_OUT_INVERT     //invert the Output Signal for Service-Mode (Prog-Relay)

#if defined(ESP32)
  #define POWER_LED_INVERT      0 // Beim ESP32 auf der MobaLedLib Hauptplatine
#else
  #define POWER_LED_INVERT      1 // invert the central Power Ouput Signal
#endif

//#define DALLASTEMPSENSE   //Dallas 18B20 Temperatur Sensor for Arduino MEGA only!                                              // 16.01.22: Disabled
#define Z21DISPLAY     //SSD1306 OLED Display for config Data for Arduino MEGA and ESP only! -->MEGA: 20(SDA), 21(SCL) || -->ESP8266: D2(SDA), D1(SCL)

#define FS128 //default speed steps (Fahrstufen) => possible values are: FS14, FS28, FS128
//#define SwitchFormat IB  //(DEFAULT = ROCO!) Define accessory Address start value => possivle values are: ROCO (+0), IB (+4)

#define Uref 1.10                     // measured reference voltage ARef-pin, individual value for every Arduino
#define senseResist 0.33              // actual resistor for measuring current on track

//#define EXTERNAL_UREF_1V1   //optional: AREF with external 1.1 Volt, to get better CV# read with Arduino UNO

/***************************************************************/
/*------------------DEBUG-ONLY---------------------------------*/
/***************************************************************/
#define Debug Serial //(DEFAULT = Serial!) use not the Default Standard Debug Serial Interface. Select another serial:
#define DebugBaud 115200 //Activate Debug on "Serial" with speed setting!

/*---------- select what to show (options): --------------*/
#define DEBUG    //To see System-DATA on Serial
//#define DEBUG_WLAN_CONFIG  //to see config data of Wifi ESP8266 (IP, Pw, ..)
//#define REPORT   //To see Sensor Messages (LocoNet & S88)
//#define REQEST   //To see answer Request Messages (Lok , Switch, CV)
//#define LnDEB    //To see RAW DATA of LocoNet Protokoll
//#define XnDEB    //To see XpressNet Data
//#define Z21DEBUG //to see Z21 LAN control data
//#define Z21DATADEBUG //to see RAW DATA of Z21 LAN Protokoll
//#define Z21SYSTEMDATADEBUG  //to see the system data mainpower and temp
//#define notifyz21Lo


// Additional config lines by Hardi

#define USE_CAN      // CAN Bus

#if defined (ESP8266)
  #define ROTATE_DISPLAY_180_DEG                  1
  #define OLED_TYP                                13      // If the wrong type is selected the display is not shown correctly, (Artefacts on the left side)
// Achtung: Loetjumper anpassen 2=VDD, 1=GND)  // 13:  1.3"  I2C 128X64  OLED  SH1106   (https://de.aliexpress.com/item/32683739839.html)
//          Auch beim 0.96" Display gibt es    // 96:  0.96" I2C 128X64  OLED  SSD1306  (https://de.aliexpress.com/item/32643950109.html)
//          verschiedene Pinbelegungen.        // 7920 3"    SPI Display (12864B for 3D printers with ST7920 chip) (https://www.amazon.de/dp/B09SLXVH1P)
//          Diese passen u.U. nicht zur Platine              (https://www.amazon.de/AZDelivery-%E2%AD%90%E2%AD%90%E2%AD%90%E2%AD%90%E2%AD%90-Display-Zeichen-Arduino/dp/B07DDKBCY7)
//                                             //            GND, VCC, V0 = NC, RS = VCC, R/W = SPI_DISP_DAT, E = SPI_DISP_CLK, ... PSB = GND, BLA = VCC, BLK = GND
#else // ESP32
  #define ROTATE_DISPLAY_180_DEG                  0
  #define OLED_TYP                                7920
#endif

#define USE_SERIAL_COMMANDS                       1    // Enable serial commands for testing (See "Serial_Commands.h")

#define USE_LIBZ                                  1    // Decompress ZLib compressed files which have been received from the CAN
                                                       // Must be enabled to read the loco symbols from the CSx

#define SPY_CAN_CONFIG_FILE                       0    // Spy config file messages ( 1: Enable/Disable per "spy" in serial monitor, 2: Activ from the start)
                                                       // To uste this function the number of locos has to be reduced: MAX_LOK_DATA 50
#define SPY_CAN_CONFIG_FILE_HEX                   0    // Print the spyed data also as hex.
                                                       // Attention: The ESP8266 dosn't have enough memory. => It will crash with "oom exception" = Out of Memory

#define SET_SPEED_CONTROLLERS_TO_0_AT_STOP        1    // Set the speed of all controllers to 0 when "Stop" is activated.
                                                       // If set to 0 the the locos are also stopped but the controllers
                                                       // still show the last speed. They don't drive with this speed if the halt state
                                                       // is released. Ihis may be confusing.
                                                       // The "HALT" state is disabled if the speed of one loco is changed. But only this
                                                       // loco changes its speed.
                                                       //
                                                       // The buttons could in the Z21 app and the button on the box could be configured
                                                       // in the Z21 app individually.
                                                       // In the section "App settings" (App Einstellungen) the behavior of the button
                                                       // in the app is defined.
                                                       // The hardware button on the box is defined in the section "Z21 settings" (Z21 Einstellungen).


//------------------------------------------------
#define USE_EXT_ADR_TABLE                         0    // Use the extended adress table. (Must be enabled to use different loco protocolls)
/*
#define EA_TYP_MFX   1
#define EA_TYP_DCC   2
#define EA_TYP_SID   3
#define EA_TYP_MM    4

#define EXT_ADR_RANGE_1_TYP              EA_TYP_MFX    // ToDo: Dokumentieren
#define EXT_ADR_RANGE_2_TYP              EA_TYP_DCC
#define EXT_ADR_RANGE_3_TYP              EA_TYP_SID
#define EXT_ADR_RANGE_4_TYP              EA_TYP_MM

#define EXT_ADR_RANGE_END_1                    4000
#define EXT_ADR_RANGE_END_2                    8000
#define EXT_ADR_RANGE_END_3                    9000
*/

#define Z21_ADR_MM_START                       8000    // Start adress for MM locos. The adresses before are used for DCC.
#define Z21_ADR_MFX_START                      8256    // Start adress for MFX locos.

#define TEST_LOCO_SELECTION                       0
//------------------------------------------------



#endif // __CONFIG_H__
