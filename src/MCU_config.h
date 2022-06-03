//--------------------------------------------------------------
/*
 * Setup up PIN-Configuration for different MCU
 *
 * Support for:
 *    - Arduino DUE
 *    - ESP8266
 *    - ESP32
 *    - Arduino UNO
 *    - Arduino MEGA
 *    - Sanguino (ATmgega 644p & ATmega 1284p)
 *
 * Copyright (c) by Philipp Gahtow, year 2022
*/

#if defined(__SAM3X8E__)    //SAM ARM Adruino DUE
#define DUE_MCU
#undef Z21VIRTUAL
#undef XPRESSNET

#elif defined(ESP8266) //ESP8266 or WeMos D1 mini
#define ESP8266_MCU
#define ESP_WIFI
#define ESP_OTA        // Uses 3.4% FLASH
#undef Z21VIRTUAL
#undef LAN
#undef DHCP
#undef WIFI

#elif defined(ESP32)   //ESP32 Modul
#define ESP32_MCU
#define ESP_WIFI
#define ESP_OTA        // Uses 3.7% FLASH
#undef Z21VIRTUAL
#undef LAN
#undef DHCP
#undef WIFI

#if defined(DCC)
  #error "Error: DCC must be disabled in the main program"
#endif

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) //Arduino MEGA
#define MEGA_MCU
//#define SOFT_RESET    //for changed on LAN Interface
#undef Z21VIRTUAL

#elif defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644P__)  //Sanguino (other pins!)
#define SANGUINO_MCU
//#define SOFT_RESET    //for changed on LAN Interface
//ACHTUNG SS is on PIN3 (D2)!!!
#undef Z21VIRTUAL

#else //others Arduino UNO / NANO
#define UNO_MCU
//#define SOFT_RESET    //for changed on LAN Interface

#endif

//--------------------------------------------------------------
//Setting PIN CONFIG:
//--------------------------------------------------------------
#if defined(SANGUINO_MCU)
//Power:
#define Z21ResetPin 27  //RESET-Button-Pin bei Neustart betätigen um Standard IP zu setzten!
#define Z21ButtonPin Z21ResetPin  //Pin where the POWER-Button is conected
//DCC and Booster
#define DCCLed 25    //LED to show DCC active
#define DCCPin 12    //Pin for DCC sginal out
#define ShortLed 26     //LED to show Short
#define ShortExtPin 4  //Pin to detect Short Circuit of Booster (detect LOW)
#define GoExtPin 3   //Pin for GO/STOP Signal of Booster
#define ProgRelaisPin  23   //Pin for using Kehrschleifen-Modul
//Booster INT config:
#define GoIntPin 17   //Pin for inverted DCC Signal
#define ShortIntPin 13  //Pin for second Booster like TLE5206 (detect HIGH)
#define VAmpIntPin A4   //Input for Current sensor (CV read)
//XpressNet:
#define XNetTxRxPin  16    //XpressNet Control-Port for Send/Receive at MAX485
//LocoNet:
#define LNTxPin 15    //Sending Pin for LocoNet

//--------------------------------------------------------------
#elif defined(ESP8266_MCU)
// ********************************** ESP8266_MCU ****************************
/*

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
    The ISO CAN Module           |       \ _|+       '--, Wemos D1 Mini|                       Der IR Empf�nger kann auch an D4 angeschlossen werden
    has to be powered           \   Piezo |_|     Rst ==|   ____       |                       Dann flackert die LED automatisch mit. Sie kann aber nicht
    with 5V!                     \       /  |           '---    -------'                       Anderweitig genutzt werden.
                                _|_        _|_                                                 Getestet mit: ...IRremoteESP8266\examples\IRrecvDemo

   CAN Kabel von MS2
   M�rklin E146781
    GND    Schwarz  Sub-D 3, 6
    CAN H  Orange   Sub-D 7
    CAN L  Braun    Sub-D 2
    +18V   Rot      Sub-D 9
 Attention: The pin sequence in HW V. 1.0 is wrong ;-(
            Black and Orange have to be swapped

 Large ESP8266 Modul
 ~~~~~~~~~~~~~~~~~~~

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
            |RST              TX| GPIO1         |_|                    Attention:
            |GND             GND|               |  \                   Pin order doesn't  CAN Kabel von MS2
            |5V              3V3|              _|_                     match with the     M�rklin E146781
            |    # ,_____, #    |                                      CAN module          +18V   Rot      Sub-D 9
            '------| USB |------'                                                          CAN H  Orange   Sub-D 7
                   '-----'                                                                 CAN L  Braun    Sub-D 2
                                                                                           GND    Schwarz  Sub-D 3, 6
                                                                                          Attention: The pin sequence in HW V. 1.0 is wrong ;-(
                                                                                                     Black and Orange have to be swapped
*/
#if defined(DCC)
  #error "Error: DCC must be disabled in the main program"
#endif

//Power:
#define Z21ResetPin   0           // D3 = RESET-Button-Pin bei Neustart betätigen um Standard IP zu setzten!
#define Z21ButtonPinAnalogOnly    // The pin can't be used as digital input
#define Z21ButtonPin  A0          // Pin where the POWER-Button is conected

//DCC and Booster
#define DCCLed        2           // D4 = LED to show DCC active (BUILTIN_LED)

#define CAN_CS_PIN    16          // Pin D0 (GPIO16) is used as chip select for the CAN bus
                                  // Attention the MS2_Bridge uses Pin D8 (GPIO15)

#define PIEZO_PIN        15
#define IR_RECEIVE_PIN   0        // D3=GPIO0  Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.

//--------------------------------------------------------------
#elif defined(ESP32_MCU)
//************************************ ESP32_MCU ************************************
/*

 Tests with the MobaLedLib Mainboard and a ESP32        CAN is connected Maerklin device   CAN Kabel von MS2
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                  3 = CAN H                M�rklin E146781
  TSSP 4P38 .___.                                                 4 = CAN L                 +18V   Rot      Sub-D 9
    oder    |( )|                                                                           CAN H  Orange   Sub-D 7
  TSOP 4836 |___|      ,----------,===============,---------------------,                   CAN L  Braun    Sub-D 2
             |||_5V__________,    |:             :'-----,          ___  |                   GND    Schwarz  Sub-D 3, 6
             ||        |  ___|_   |:             :      |         | C | |
          ,--+o-GND------|14 13|  |:             : ESP  '-------, | A | |   3" LCD Display 7920    SPI Display (12864B for 3D printers with ST7920 chip) (https://www.amazon.de/dp/B09SLXVH1P)
          |  |         | |11 12|  |:             : Adapter      | | N | |   ~~~~~~~~~~~~~~~~~~~
          |  |         | |10 9 |  |:             :   ,----------, '---' |       4 = GND = NL K
          |  '--7------|-|-- 7    |:             :   |2 4 6 8 10|============== 3 = VCC = SS = BL A
    \._   |            | |6  5 |  |:             :   |1 3 5 7 9 |============== 9 = MOSI               ESP D23
     | |--'GND         | |4  3 |  |: ,---------, :---'----  ----'       |       7 = SCK                ESP D18
     |_|----------1----|-|-- 1 |  |: |         | :,---------,           |
    /      +           | '-----'  |: | ESP 32  | :*    OLED |           |   Entweder OLED 0.96" / 1.3" oder 3" LCD Display
    Piezo              | KEY_80   |: |         | :*    0.96"|           |   Das wird in Config.h definiert
                       |          |: |         | :*     or  |           |
                       |          |: |_________| :*     1.3"|           |
                       |          '---------------'---------'           |
                       |            ( )  ( )  ( )                       |
                       |            [*]  [*]  [*]                       |
                       |                                  Hauptplatine  |
                       '------------------------------------------------'
                                              LED
                                             Button

ESP32
Current  67mA@18V    1.2W
        170mA@5V     0.85W => Efficiency 70%

ESP8266 46mA@18V     0.83W
        96mA @5V     0.5W  => Efficiency 60%

*/
//Power:
#define Z21ResetPin      25       // RESET-Button-Pin bei Neustart betätigen um Standard IP zu setzten!      // 02.02.22:  Old: 36
                                  // Rechter Taster der MobaLedLib
#define Z21ButtonPin  Z21ResetPin // Pin where the POWER-Button is conected
//DCC and Booster
#define DCCLed           0        // LED to show DCC active    (0= Blaue LED MLL)                         // 02.02.22:  Old: 32

//efine DCCPin           33       // Pin for DCC sginal out
//efine additionalOutPin 16       // Pin for true DCC Output without Shutdown adn RailCom
//efine ShortLed         27       // LED to show Short
//efine ShortExtPin      4        // Pin to detect Short Circuit of Booster (detect LOW)
//efine GoExtPin         15       // Pin for GO/STOP Signal of Booster
//efine ProgRelaisPin    26       // Pin for using Kehrschleifen-Modul
//Booster INT config:
//efine GoIntPin 25               // Pin for inverted DCC Signal
//#define ShortIntPin    35       // Pin for second Booster like TLE5206 (detect HIGH)
//efine VAmpIntPin       34       // Input for Current sensor (CV read)
//efine TempPin          13       // Temp.sence_resistor (15k) with 46k Pull-Up or DALLAS
//efine VoltIntPin       39       // Rail Voltage: Rail:100k - Sence - 4,7k - GND
//XpressNet
//#define XNetTxRxPin    9        // XpressNet Control-Port for Send/Receive at MAX485
//ndef XPRESSNET
//
//LocoNet
//efine LNTxPin          21       // Sending Pin for LocoNet
//efine LNRxPin          19       // Receiving Pin for LocoNet

                                  //                                                                        ^  3.3V
                                  //                                                                  .___. |
                                  // !!! Wenn an den ESP Pin 5 ein Pieozo angeschlossen ist, !!!      |(O)| |   TSSP 4P38
                                  // !!! dann geht der CAN Bus nich mehr weil das der TX PIN !!!      |___| |     oder
                                  // !!! des CANs ist ;-(                                    !!!       |||__|   TSOP 4836
#define PIEZO_PIN        17       // ESP D17 = MobaLedLib KEY80 Pin 1,                                 ||
#define IR_RECEIVE_PIN   19       // ESP D19 = MobaLedLib KEY80 Pin 7 (Pin 13 = +5V, Pin 14 = GND) ----'|
                                  //                                   "MLL EXT OUT" Pin 8             _|_



#define SPI_DISP_CLK     21       // Pins used for an SPI OLED Display
#define SPI_DISP_DAT     22       // Connected to the I2C socket

#define USE_HARDWARE_SPI
// Hardware SPI for OLED Display typ 7920
// D23 MOSI  KEY80 Pin 6,   MLL_EXT_OUT Pin 9
// D18 SCL                  MLL_EXT_OUT Pin 7


//--------------------------------------------------------------
#elif defined(UNO_MCU) //Arduino UNO
//Power:
#define Z21ResetPin 10  //RESET-Button-Pin bei Neustart betätigen um Standard IP zu setzten!
#define Z21ButtonPin Z21ResetPin  //Pin where the POWER-Button is conected
//DCC and Booster
#define DCCLed 3    //LED to show DCC active
#define DCCPin 6    //Pin for DCC sginal out
#define additionalOutPin 11 //Pin for true DCC Output without Shutdown adn RailCom
#define ShortLed 45     //LED to show Short
#define ShortExtPin 5  //Pin to detect Short Circuit of Booster (detect LOW)
#define GoExtPin  A4   //Pin for GO/STOP Signal of Booster
#define ProgRelaisPin  A5   //Pin for using Kehrschleifen-Modul
//Booster INT config:
#define GoIntPin 4   //Pin for inverted DCC Signal
#define ShortIntPin 2  //Pin for second Booster like TLE5206 (detect HIGH)
#define VAmpIntPin A4   //Input for Current sensor
//XpressNet
#define XNetTxRxPin  9    //XpressNet Control-Port for Send/Receive at MAX485
//LocoNet
#define LNTxPin 7    //Sending Pin for LocoNet

//--------------------------------------------------------------
#else  //Arduino MEGA or DUE
//Power:
#define Z21ResetPin 47  //RESET-Button-Pin bei Neustart betätigen um Standard IP zu setzten!
#define Z21ButtonPin Z21ResetPin  //Pin where the POWER-Button is conected
//DCC and Booster
#define DCCLed 3    //LED to show DCC active
#define DCCPin 6    //Pin for DCC sginal out
#define additionalOutPin 11 //Pin for true DCC Output without Shutdown adn RailCom
#define ShortLed 45     //LED to show Short
#define ShortExtPin 5  //Pin to detect Short Circuit of Booster (detect LOW)
#define GoExtPin  A4   //Pin for GO/STOP Signal of Booster
#define ProgRelaisPin  A5   //Pin for using Kehrschleifen-Modul
//Booster INT config:
#define GoIntPin 39   //Pin for inverted DCC Signal
#define ShortIntPin 41  //Pin for second Booster like TLE5206 (detect HIGH)
#define VAmpIntPin A9   //Input for Current sensor (CV read)
//#define VAmSencePin A8 //ACS712 5A Sensor (for testing only)
#define VoltIntPin A10  //Rail Voltage: Rail:100k - Sence - 4,7k - GND
#define TempPin A11     //Temp.sence_resistor (15k) with 46k Pull-Up or DALLAS
//XpressNet
#define XNetTxRxPin  9    //XpressNet Control-Port for Send/Receive at MAX485
//LocoNet
#define LNTxPin 7    //Sending Pin for LocoNet

#endif

//--------------------------------------------------------------
//Dallas Temperatur Sensor (MEGA only):
#if defined(DALLASTEMPSENSE) && defined(MEGA_MCU)
#define ONE_WIRE_BUS TempPin
#else
#undef DALLASTEMPSENSE
#endif

//--------------------------------------------------------------
//S88 Singel Bus:
#if defined(ESP8266_MCU)
#undef S88N
#endif

#if defined(S88N)
#if defined(ESP32_MCU)
    //Eingänge:
  #define S88DataPin 13      //S88 Data IN
    //Ausgänge:
  #define S88ClkPin 12      //S88 Clock
  #define S88PSPin 14       //S88 PS/LOAD
  #define S88ResetPin 27    //S88 Reset
#else
    //Eingänge:
  #define S88DataPin A0      //S88 Data IN
    //Ausgänge:
  #define S88ClkPin A1      //S88 Clock
  #define S88PSPin A2       //S88 PS/LOAD
  #define S88ResetPin A3    //S88 Reset
#endif
#endif
//--------------------------------------------------------------
//DCC Decoder
#if defined(SANGUINO_MCU)
#undef DECODER
#endif
#if defined(ESP8266_MCU)
#undef DECODER
#endif
#if defined(ESP32_MCU)
#undef DECODER
#endif

   //Eingänge:
#if defined(DECODER)
#define IRQDCCPin 0      //Arduino Interrupt Number (attachInterrupt Funktion)
#define decDCCPin 2      //The Digital PIN where the Interrupt is on
#endif

//--------------------------------------------------------------
//LAN-Interface:
#define LANSSPIN 10   //Chip Select Pin of most Ethernet Shields
#define SDSSPIN 4     //Chip Select Pin SD-Card Reader on Ethernet Shield

//--------------------------------------------------------------
//Wifi-Interface:
#if defined (WIFI)
//Serialport:
#ifndef WLAN  //WLAN defined
  #if defined(MEGA_MCU) || defined(DUE_MCU) //MCU check: Arduino MEGA
  #define WLAN Serial2
  #elif defined(Z21VIRTUAL)
  #define WLAN SoftSerial
  #else
  #define WLAN Serial
  #endif  //END MCU check
#endif  //END WLAN defined

#if defined(Z21VIRTUAL) //Default Serial Baud Rate 1200,2400,4800,9600,14400,19200,28800,38400,57600,115200
//for Arduino UNO only:
#define WIFISerialBaud 38400
#define RXvWiFi 12  //RX-PIN Soft Serial for Arduino UNO WiFi use
#define TXvWiFi 13  //TX-PIN Soft Serial for Arduino UNO WiFi use
#else
#define WIFISerialBaud 500000 //new with Z21_ESPArduinoUDP_v2.90 or higher!
#endif //END Z21VIRTUAL

#endif //End WiFi

//--------------------------------------------------------------
//Remove Display because of Pins already in use!
#if defined(Z21DISPLAY) && defined(UNO_MCU)
#undef DISPLAY
#endif
