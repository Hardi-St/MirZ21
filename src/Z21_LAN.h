//Z21 LAN and WiFi
//Kommunikation mit der Z21 Library!
//
//Copyright (c) by Philipp Gahtow, year 2021
/*
 * Changes:
 * - Add change für WLAN für multi Paket - meherer Abfragen vom Client
 * - Add change via LAN für multi Paket
 * - Add store for source IP
 * - Change State Report of Volt, mA and temp
 * - Change Ethernet Big UDP handel at packet length = 0
 * - Update ESP Wifi setup
 * - add new function to request WiFi conf data
 * - remove while in Serial read for WiFi
 */
//----------------------------------------------
#if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)

#include "Dprintf.h"

extern void Z21LANreceive();

// buffers for receiving and sending data
#define Z21_UDP_TX_MAX_SIZE 20  //--> Z21 LocoNet tunnel DATA has 20 Byte!
#define Z21_BIG_UDP_MIN_SIZE 4    //--> smallest packet length in a BIG UDP Packet that we can handle

//----------------------------------------------
#if defined(Z21DISPLAY) && defined(BOOSTER_INT_MAINCURRENT)
extern void DisplayUpdateRailData(uint16_t inAm, float volt, float temp);
#endif

void Send_Acc_to_CAN(uint16_t Adr, uint8_t Pos, uint8_t Current);   // Forward definition  (CAN_Bus.h)        // 14.07.22:
void Display_Accessory(uint16_t Adr, uint8_t Pos, uint8_t Current); //   "       "         (IR_Functions.h)

//----------------------------------------------
//Wenn zwei Kommunikationsschnittstellen aktiv sind wird LAN hinten angestellt:
#if defined(LAN) && defined(WIFI)
#define Z21_Eth_offset WLANmaxIP   //shift LAN client behind WiFi client
#else
#define Z21_Eth_offset 0
#endif

//----------------------------------------------
#if defined(ESP_WIFI)
/*
IPAddress Ip(192,168,0,111);
IPAddress Gw(192,168,0,111);
IPAddress Sb(255,255,255,0);
*/
//client ssid and pw stored in EEPROM!
String ssid = "";
String pass = "";
//AP ssid and pw read out from EEPROM:
String ssidAP = "";
String passAP = "";
byte kanalAP = 3;
#endif

//----------------------------------------------
#if defined(LAN) || defined(ESP_WIFI)
typedef struct    //Rückmeldung des Status der Programmierung
{
  byte IP0; //client IP-Adresse
  byte IP1;
  byte IP2;
  byte IP3;
  byte time;  //aktive Zeit
  unsigned int port;    //source Port
} listofIP;
listofIP mem[LANmaxIP];
byte storedIP = 0;  //speicher für IPs
#endif

//----------------------------------------------
unsigned long IPpreviousMillis = 0;       // will store last time of IP decount updated and SystemInfo update

#if defined(WIFI)
byte outData[Z21_UDP_TX_MAX_SIZE];    //store received Serial to send via UDP
byte outDcount = 0;  //position writing out data
byte sendTOO = 0xFF;  //memIP to whom to send the data
byte listofWLANHash[WLANmaxIP+LANmaxIP];
#endif

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#if defined(WIFI)
/* request the config of Serial WLAN ESP8266:
 * 0 = AP IP
 * 1 = AP SSID Name
 * 2 = AP Password
 * 3 = AP Channel
 * 4 = Client IP
 * 5 = Client SSID Name
 * 6 = Client Password
 * 7 = S88 Module
 * 8 = ESP Sw Version
 */
String WLANgetConf(uint8_t val) {
  //make Serial Buffer empty:
  while (WLAN.available() > 0) {
    Z21LANreceive();
  }
  //request the data:
  WLAN.write(0xFE);
  WLAN.print(F("get"));
  WLAN.write(val);
  byte timeout = 0xFF;
  String s = F(" not found!");
  while (WLAN.available() == 0 && timeout > 0) {timeout--; delay(5);}  //wait
  if (timeout > 0) {
    s = WLAN.readString();
  }
  return s;
}

//init ESP8266 serial interface
void WLANSetup() {
    WLAN.begin(WIFISerialBaud);  //UDP to Serial Kommunikation
    WLAN.setTimeout(5);   //sets the maximum milliseconds to wait for serial data
    byte b0 = 0;
    byte b1 = 0;
    #if defined(DEBUG)
      Debug.print(F("WiFi "));
      Debug.print(WIFISerialBaud);
      Debug.print(F("b start.."));
    #endif
    byte timeout = 200;  //try to init at least                                                               // 13.01.22:  Old: 20
    do {
       #if defined(DEBUG)
           Debug.print(".");
       #endif
       WLAN.println(); //let device Flush old Data!
       delay(50);   //wait for ESP8266 to receive!
       #if defined(LED_BLUE_PIN)                                                                               // 14.01.22:
           digitalWrite(LED_BLUE_PIN, !digitalRead(LED_BLUE_PIN));
       #endif
       while (WLAN.available() > 0) {
          b0 = b1;
          b1 = WLAN.read();
          #if defined(DEBUG)
              Debug.write(b1);
          #endif
          }
       timeout--;
    }
    while (b0 != 'O' && b1 != 'K' && timeout > 0);
    if (timeout == 0) {   //Error - no ESP module found!
         #if defined(DEBUG)
           Debug.println("\nWiFi FAIL");
         #endif
         #if defined(LED_BLUE_PIN)                                                                               // 14.01.22:
             digitalWrite(LED_BLUE_PIN, 0);
         #endif
         delay(1);
         }
    else {    //Continue config setting for ESP
         #if defined(LED_BLUE_PIN)                                                                            // 14.01.22:
           digitalWrite(LED_BLUE_PIN, 1);
         #endif
         #if defined(DEBUG)
           Debug.println();
         #endif
         //Setting S88:
         #if defined(DEBUG)
           Debug.print(F("Set ESP S88..."));
         #endif
         WLAN.write(0xFE);   //Send start Bit for Initial Settings
         WLAN.print(F("S88"));
         #if defined(S88N)
           WLAN.write(S88Module);
         #else
           WLAN.write(0xFE);
         #endif
         delay(100);   //wait to receive
         while (WLAN.available() > 0) {
        char c = WLAN.read();
           #if defined(DEBUG)
        Debug.write(c);
           #endif
           }
         #if defined(DEBUG)
           Debug.println();
         #endif

         #if defined(DEBUG_WLAN_CONFIG)
           //Report ESP config data: ---------------------------------------------------
           String list[] = {F("WiFi-AP"),F("SSIP-AP"),F("PASS-AP"),F("CHAN-AP"),F("WiFi-CL"),F("SSIP-CL"),F("PASS-CL"),F("ESP-S88"),F("ESP-Version")};
           //request each singel value:
           for (byte counter = 0; counter < 9; counter++) {
             Debug.print(list[counter]);
             Debug.print(": ");
             Debug.println(WLANgetConf(counter));
             }
         #endif
         //END ESP config data --------------------------------------------------------
         }
    //Read all client Hash:
    WLAN.write(0xFE);
    WLAN.print(F("get"));
    WLAN.write(0x09);
    timeout = 0xFF;
    while (WLAN.available() == 0 && timeout > 0) {timeout--; delay(5);}  //wait
    byte HashData = 0x00;
    byte ClientCount = 0;
    #if defined(Z21DEBUG) || REPORT_WIFI_CLIENTS                                                              // 12.07.22:  Added: REPORT_WIFI_CLIENTS
        Debug.println(F("Report WiFi Clients:"));
    #endif
    while (WLAN.available() > 0) {
      HashData = WLAN.read();
      if (HashData != 0x00) {
        listofWLANHash[ClientCount] = HashData;
        ClientCount++;
        #if defined(Z21DEBUG) || REPORT_WIFI_CLIENTS                                                          // 12.07.22:  Added: REPORT_WIFI_CLIENTS
            Debug.print(ClientCount);
            Debug.print(F(" IPHash: "));
            Debug.println(HashData, HEX);
        #endif
      }
    }
}
#endif

//--------------------------------------------------------------------------------------------
#if defined(ESP_WIFI)
/**********************************************************************************/
//s = String that will be saved,
//laenge = Index to length byte,
//start = index to the first letter
void EEPROMwrite (String s, uint16_t laenge, uint16_t start) {
  byte len = s.length();
  FIXSTORAGE.write(laenge,len);
  for (uint16_t i = start; i < (start+len); i++) {
     FIXSTORAGE.write(i,s[i-start]);
  }
}
/**********************************************************************************/
//laenge = Index to length byte,
//start = index to the first letter
String EEPROMread (uint16_t laenge, uint16_t start) {
  String s = "";
  byte len = FIXSTORAGE.read(laenge);
  if (len < EEStringMaxSize) {
    for (uint16_t i = start; i < (start+len); i++) {
      s += char(FIXSTORAGE.read(i));
    }
  }
  return s;
}
/**********************************************************************************/
boolean tryWifiClient() {
  // WiFi.scanNetworks will return the number of networks found
  byte n = 1; //WiFi.scanNetworks();
/*
  //Try to connect to WiFi
    #if defined(DEBUG)
        Serial.print("Scan start ... ");
        byte x = WiFi.scanNetworks();
        Serial.print(x);
        Serial.println(" network(s) found");
        for (int i = 0; i < x; i++)
        {
          Serial.println(WiFi.SSID(i));
        }
      #endif
*/
  if ((n > 0) && (ssid.length() > 0)) {

     #if defined(DEBUG)
        Serial.println();
        Serial.print("Client: "); Serial.println(ssid.c_str());
      //Serial.print("PW: ");     Serial.println(pass.c_str());                                               // 11.07.22:  Disabled (Enable to debug connection errors)
     #endif

     WiFi.begin(ssid.c_str(), pass.c_str());

     n = 0;
     while(WiFi.waitForConnectResult() != WL_CONNECTED){  // waitForConnectResult(): 3.8 sec
//        WiFi.begin(ssid.c_str(), pass.c_str());
        delay(100);
        n++;
        if (n > 30) {
            WiFi.disconnect();
            return false;
        }
      }
      return true;
  }
  return false;
}

//-------------------------------
void Set_WLAN_EEPROM_to_Default()
//-------------------------------
{
    #if defined(DEBUG)
      Debug.println(F("Reset to default!"));
    #endif

    ssidAP = SssidAP;
    passAP = SpassAP;
    kanalAP = SkanalAP;
/*
    #if defined(ESP32)
    portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&myMutex);
    #endif
*/

    ssid = "";
    pass = "";
    EEPROMwrite (ssid, EEssidLength, EEssidBegin);                                                            // 05.02.22:  Hardi: Added
    EEPROMwrite (pass, EEpassLength, EEpassBegin);                                                            //    "

    EEPROMwrite (ssidAP, EEssidAPLength, EEssidAPBegin);
    EEPROMwrite (passAP, EEpassAPLength, EEpassAPBegin);
    FIXSTORAGE.write(EEkanalAP, kanalAP);

    FIXSTORAGE.commit();
/*
    #if defined(ESP32)
    portEXIT_CRITICAL(&myMutex);
    #endif
*/
}


/**********************************************************************************/
void ESPSetup() {

  #if defined(ESP32)
  WiFi.mode(WIFI_MODE_APSTA);
  #else
  WiFi.mode(WIFI_AP_STA);  //AP & client
  #endif

  // read eeprom for ssid and pass:
  //--------------WIFI CLIENT---------------
  ssid = EEPROMread(EEssidLength, EEssidBegin);
  pass = EEPROMread(EEpassLength, EEpassBegin);
  //--------------ACCESS POINT-------------
  ssidAP = EEPROMread(EEssidAPLength, EEssidAPBegin);
  passAP = EEPROMread(EEpassAPLength, EEpassAPBegin);


  if ((ssidAP.length() > 32) || (ssidAP.length() == 0) || (passAP.length() > 32) || (passAP.length() == 0) || *passAP.c_str() > 128) { //request is OK?
       Set_WLAN_EEPROM_to_Default();
       }
  else {
       kanalAP = FIXSTORAGE.read(EEkanalAP);
       if ((kanalAP < 1) || (kanalAP > 13))
          kanalAP = SkanalAP;
       }
  //Serial.print("AP PWD from EEPROM: '"); Serial.print(passAP); Serial.println("'"); // Debug

  WiFi.softAPConfig(AP_ip, AP_ip, AP_sb);  //set the default IP for Z21

  #if defined(ESP32_MCU)
  WiFi.softAP(ssidAP.c_str(), passAP.c_str() );  //Start AcessPoint, ignore Channel parameter!
  #else
  WiFi.softAP(ssidAP.c_str(), passAP.c_str(), kanalAP, false, 8);    //Start AcessPoint with 8 number of clients
  #endif
  //don't hide SSID and max simultaneous connected stations set to eight!


  #if defined(DEBUG)
    Debug.print(F("AP Name: "));
    Debug.println(ssidAP);
    Debug.print(F("Pw: "));
    Debug.println(passAP);
    Debug.print(F("IP: "));
    Debug.println(WiFi.softAPIP());

  #endif

  if (tryWifiClient()) {  //Try to connect to WiFi   (3.8 sec)
  	#if defined(DEBUG)
    Debug.print(F("Connected to "));
    Debug.println(ssid);
    Debug.print(F("IP: "));
    Debug.println(WiFi.localIP());
	  #endif
  }
  #if defined(DEBUG)
  else {
    Debug.print(F("Connect fail to '"));
    Debug.print(ssid);
    Debug.println(F("'"));
  }
  #endif

}
#endif

//--------------------------------------------------------------------------------------------
#if defined(LAN) || defined(ESP_WIFI)
byte Z21addIP (byte ip0, byte ip1, byte ip2, byte ip3, unsigned int port) {
  //suche ob IP schon vorhanden?
  for (byte i = 0; i < storedIP; i++) {
    if (mem[i].IP0 == ip0 && mem[i].IP1 == ip1 && mem[i].IP2 == ip2 && mem[i].IP3 == ip3) {
      mem[i].time = ActTimeIP; //setzte Zeit
      mem[i].port = port; //update port!
      return i+1;
    }
  }
  if (storedIP >= LANmaxIP) {
    for (byte i = 0; i < storedIP; i++) {
      if (mem[i].time == 0) { //Abgelaufende IP, dort eintragen!
         mem[i].IP0 = ip0;
         mem[i].IP1 = ip1;
         mem[i].IP2 = ip2;
         mem[i].IP3 = ip3;
         mem[i].port = port;
         mem[i].time = ActTimeIP; //setzte Zeit
         return i+1;
      }
    }
    return 0; //keine freien IPs (never reach here!)
  }
  mem[storedIP].IP0 = ip0;
  mem[storedIP].IP1 = ip1;
  mem[storedIP].IP2 = ip2;
  mem[storedIP].IP3 = ip3;
  mem[storedIP].port = port;
  mem[storedIP].time = ActTimeIP; //setzte Zeit
  storedIP++;
  return storedIP;
}
#endif


void Z21LANreceive() {
  //-------------------------------------------------------------------------------------------
  //receive LAN data or we run on ESP MCU:
  #if defined(LAN) || defined(ESP_WIFI)
  byte packetSize = Udp.parsePacket();
  if(packetSize) {  //packetSize
    IPAddress remote = Udp.remoteIP();
    byte packetBuffer[packetSize];
    Udp.read(packetBuffer,packetSize);  // read the packet into packetBufffer
    if (packetSize == packetBuffer[0]) { //normal:
      #if defined(Z21DATADEBUG) || Z21DATADEBUG_LONG
          if (Z21DATADEBUG_LONG == 0 || packetBuffer[0] > 9)                                                                 //   "
             {
             Debug.print(Z21addIP(remote[0], remote[1], remote[2], remote[3], Udp.remotePort()) + Z21_Eth_offset);
             Debug.print(" Z21 RX: ");
             for (byte i = 0; i < packetBuffer[0]; i++) {
               Debug.print(packetBuffer[i], HEX);
               Debug.print(" ");
               }
             Debug.println();
             }
      #endif
      z21.receive(Z21addIP(remote[0], remote[1], remote[2], remote[3], Udp.remotePort()) + Z21_Eth_offset, packetBuffer);
    }
    else {  //kombiniertes UDP Paket:
      #if defined(Z21SYSTEMDATADEBUG)
      Debug.print(packetSize);
      Debug.print(F(" BIG Z21 RX: "));
      for (byte i = 0; i < packetSize; i++) {
        Debug.print(packetBuffer[i], HEX);
        Debug.print(" ");
      }
      Debug.println();
      #endif

      byte readpos = 0;   //data length position des aktuellen Paket
      //durchlaufe alle Pakete:
      do {
        //check if packet length is not empty and okay?
        if (packetBuffer[readpos] >= Z21_BIG_UDP_MIN_SIZE) {
             byte pBuffer[packetBuffer[readpos]];   //make a array of packet length
             for (byte i = 0; i < packetBuffer[readpos]; i++)    //fill up array with packet
               pBuffer[i] = packetBuffer[readpos + i];
             #if defined(Z21DATADEBUG)
             Debug.print("#-- "); //Big UDP = in one Packet!
             Debug.print(Z21addIP(remote[0], remote[1], remote[2], remote[3], Udp.remotePort()) + Z21_Eth_offset);
             Debug.print(F(" Z21 RX: "));
             for (byte i = 0; i < pBuffer[0]; i++) {
               Debug.print(pBuffer[i], HEX);
               Debug.print(" ");
               }
             Debug.println();
             #endif
             z21.receive(Z21addIP(remote[0], remote[1], remote[2], remote[3], Udp.remotePort()) + Z21_Eth_offset, pBuffer);
             readpos = packetBuffer[readpos] + readpos;  //bestimme position des nächsten Paket
             }
        else {
             #if defined(Z21DATADEBUG)
             Debug.print("Length ERROR! ");
             Debug.println(packetBuffer[readpos]);
             #endif
             break; //Stop here!
             }
         }
           while(readpos < packetSize);
    }
  }
  #endif

  //-------------------------------------------------------------------------------------------
  //receive WLAN data:
  #if defined(WIFI)
  if (WLAN.available() > 0) {  //Empfang Z21 UDP Packet over Serial from ESP
      outData[outDcount] = WLAN.read(); //read
      if (sendTOO == 0xFF)
        sendTOO = outData[outDcount];  //Ziel IP/client read out
      else {
        outDcount++;
        //kombinierte UDP Paket:
        if (outData[0] == 0) {  //Double Packet - check 2. Bit Länge = 0x00!
          //Build Packet new and add client at begin:
          outData[0] = sendTOO;  //lese Länge des Packet
          sendTOO = outData[1]; //read back Stored client!
          outData[1] = 0;   //2. Bit der länge ist immer "0"
          outDcount = 2; //reset read the next Byte
          #if defined(Z21DATADEBUG)
          Debug.print("#-- "); //Big UDP = in one Packet!
          #endif
        }
        if (outData[0] <= outDcount) {  //1. Byte gibt länge der Daten an!
          if (sendTOO <= WLANmaxIP) {     //Ziel in range?
              //read Data:
            #if defined(Z21DATADEBUG)
            Debug.print(sendTOO);
            Debug.print(F(" Z21 RX: "));
            for (byte i = 0; i < outData[0]; i++) {
              Debug.print(outData[i], HEX);
              Debug.print(" ");
            }
            Debug.println(F("WLAN ok"));  //Accept
            #endif
            if (outData[0] >= Z21_BIG_UDP_MIN_SIZE) { //Check if there is really Data in?
              z21.receive(sendTOO, outData);
            }
            else if (outData[0] == 2) {   //length == 2, then ESP report client IP-Hash
              listofWLANHash[sendTOO] = outData[1];
              #if defined(Z21DATADEBUG)
              Debug.print(sendTOO + 1);
              Debug.print(F(" Z21 Client Hash: "));
              Debug.println(outData[1], HEX);
              #endif
            }
            else {
              #if defined(Z21DATADEBUG)
              Debug.println("Length ERROR!");
              #endif
            }
            outData[1] = sendTOO;  //Store Client that send the DATA
          }
          #if defined(Z21DATADEBUG)
          else {
            Debug.print(F("Z21 EE "));  //Fail
            Debug.println(sendTOO, HEX);
            WLAN.println(); //new sync!
          }
          #endif
          //Reset to read next data:
          outDcount = 0;
          sendTOO = 0xFF;
        } //END of Data received "normal"!
        else if ((outDcount >= Z21_UDP_TX_MAX_SIZE) || (sendTOO == 'E' && outData[0] == 'E') || (outData[outDcount-2] == 'E' && outData[outDcount-1] == 'E')) {
          #if defined(Z21DATADEBUG)
          Debug.write(sendTOO);  //Fail?
          for (byte i = 0; i < outDcount; i++)
              Debug.write(outData[i]);
          Debug.println(F(" FLUSH!"));
          #endif
          outDcount = 0;  //reset read buffer
          sendTOO = 0xFF;
          WLAN.println(); //new sync!
          //WLAN.flush();   //clear
        }
        else if ((sendTOO == 'O' && outData[0] == 'K') || (outData[outDcount-2] == 'O' && outData[outDcount-1] == 'K')) {  //keine valied Data!
          #if defined(Z21DATADEBUG)
          Debug.println(F("Z21 OK"));
          #endif
          outDcount = 0;  //reset read buffer
          sendTOO = 0xFF;
        }
        //S88 Module config:
        else if (sendTOO == 0xFE && outData[0] == 'S' && outData[1] == '8' && outData[2] == '8' && outDcount > 3) {
          #if defined(S88N)
          if (outData[3] <= S88MAXMODULE) {  // && sendTOO >= 0
            S88Module = outData[3];
            #if defined(REPORT)
            Debug.print(F("Set S88 Module: "));
            Debug.println(S88Module);
            #endif
            if (FIXSTORAGE.read(EES88Moduls) != S88Module) {
              FIXSTORAGE.write(EES88Moduls, S88Module);
              SetupS88();
              WLANSetup();
            }
          }
          #else
            WLAN.write(0XFE);
            WLAN.print(F("S88"));
            WLAN.write(0XFE);  //kein S88 aktiv!
          #endif
          outDcount = 0;   //reset read buffer
          sendTOO = 0xFF;
        }
      }
  }
  #endif

  //Nutzungszeit IP's bestimmen und update SystemInfo
  unsigned long currentMillis = millis();
  if((currentMillis - IPpreviousMillis) >= IPinterval) {
    IPpreviousMillis = currentMillis;
    #if defined(LAN) || defined(ESP_WIFI)
    for (byte i = 0; i < storedIP; i++) {
        if (mem[i].time > 0) {
          mem[i].time--;    //Zeit herrunterrechnen
          #if defined(Z21DATADEBUG)
          if (mem[i].time == 0) {
            Debug.print(i + Z21_Eth_offset + 1);
            Debug.println(F(" LAN client timed out"));
          }
          #endif
        }
    }
    #endif
    //interval:
    #if defined(BOOSTER_INT_MAINCURRENT)
    notifyz21getSystemInfo(0); //SysInfo an alle BC Clients senden!
    #endif
  }

}

//--------------------------------------------------------------------------------------------
#if defined(S88N)
//Ask for Feedback data of group index:
void notifyz21S88Data(uint8_t gIndex) {
  //S88sendon = 'i';
  reportS88Data(gIndex);
}
#endif

#if defined (__cplusplus)                                                                                     // 14.01.22:
    extern "C" {
#endif
// Optional callback functions which must be defined outside
extern void notifyz21_Lan_LocoFkt(uint16_t Adr, uint8_t state, uint8_t fkt)     __attribute__((weak));
extern void notifyz21_Lan_LocoSpeed(uint16_t Adr, uint8_t speed)                __attribute__((weak));
#if defined (__cplusplus)
  } // extern "C" {
#endif

//--------------------------------------------------------------------------------------------
void notifyz21RailPower(uint8_t State)
{
  if (Railpower != State) {
    #if defined(Z21DEBUG)
        Debug.print(F("z21 "));
    #endif

    globalPower(State);
  }
}

//--------------------------------------------------------------------------------------------
void notifyz21LocoState(uint16_t Adr, uint8_t data[])
{
  //#if defined(DCC)                               // 13.01.22:  Disabled "#if" because the loco status array "LokDataUpdate" is still be used
    dcc.getLocoData(Adr, data);                    //            to store the states. Otherwise random data are returned to the phone
  //#endif

  #if defined(LOCONET) && !defined(LnSLOTSRV)
  LNGetSetLocoSlot(Adr, true);  //send status to LocoNet
  #endif
  data[0] = DCC128; // Set the number of steps to 128 because otherwise we get problems in notifyz21LocoSpeed()
  #if defined(Z21DEBUG)
    Debug.print("notifyz21LocoState Adr"); Debug.print(Adr); Debug.print(" F0:"); Debug.println(data[2]);
  #endif
}



//--------------------------------------------------------------------------------------------
void notifyz21LocoFkt(uint16_t Adr, uint8_t state, uint8_t fkt)
{
  //#if defined(DCC)                               // 13.01.22:  Disabled #if because the loco status array "LokDataUpdate" is still be used
    dcc.setLocoFunc(Adr, state, fkt);              //            to store the states. Otherwise random data are returned to the phone
  //#endif

  #if defined(LOCONET)
  if (fkt <= 4) {
    byte DIRF = dcc.getFunktion0to4(Adr) | (dcc.getLocoDir(Adr) << 5);
    sendLNDIRF(Adr,  DIRF);
  }
  else if (fkt >= 5 && fkt <= 8)      sendLNSND(Adr, dcc.getFunktion5to8(Adr));
  else if (fkt >= 9 && fkt <= 12)     sendLNF3 (Adr, dcc.getFunktion9to12(Adr));
  else if (fkt >= 13 && fkt <= 20)    sendLNF4 (Adr, dcc.getFunktion13to20(Adr));
  else if (fkt >= 21 && fkt <= 28)    sendLNF5 (Adr, dcc.getFunktion21to28(Adr));
  #endif

  #if defined(XPRESSNET)
  if (fkt <= 4)                       XpressNet.setFunc0to4(Adr, dcc.getFunktion0to4(Adr));
  else if (fkt >= 5 && fkt <= 8)      XpressNet.setFunc5to8(Adr, dcc.getFunktion5to8(Adr));
  else if (fkt >= 9 && fkt <= 12)     XpressNet.setFunc9to12(Adr, dcc.getFunktion9to12(Adr));
  else if (fkt >= 13 && fkt <= 20)    XpressNet.setFunc13to20(Adr, dcc.getFunktion13to20(Adr));
  else if (fkt >= 21 && fkt <= 28)    XpressNet.setFunc21to28(Adr, dcc.getFunktion21to28(Adr));
  XpressNet.ReqLocoBusy(Adr);   //Lok wird nicht von LokMaus gesteuert!
  #endif

  if (notifyz21_Lan_LocoFkt) notifyz21_Lan_LocoFkt(Adr, state, fkt);                                          // 15.01.22:

  #if defined(Z21DEBUG)
     Debug.print(F("Z21 A:"));
     Debug.print(Adr);
     Debug.print(" Dir: ");
     Debug.print(dcc.getLocoDir(Adr), BIN);
     if (fkt <= 4) {
          Debug.print(F(", F1:"));
       Debug.println(dcc.getFunktion0to4(Adr), BIN);
     }
     else if (fkt >= 5 && fkt <= 8) {
          Debug.print(F(", F2:"));
       Debug.println(dcc.getFunktion5to8(Adr), BIN);
     }
     else if (fkt >= 9 && fkt <= 12) {
          Debug.print(F(", F3:"));
       Debug.println(dcc.getFunktion9to12(Adr), BIN);
     }
     else if (fkt >= 13 && fkt <= 20) {
          Debug.print(F(", F4:"));
       Debug.println(dcc.getFunktion13to20(Adr), BIN);
     }
     else if (fkt >= 21 && fkt <= 28) {
          Debug.print(F(", F5:"));
       Debug.println(dcc.getFunktion21to28(Adr), BIN);
     }
  #endif
}

//--------------------------------------------------------------------------------------------
void notifyz21LocoSpeed(uint16_t Adr, uint8_t speed, uint8_t steps)
{
  if (Adr == 0) {
    #if defined(Z21DEBUG)
    Debug.print(F("Z21 A:"));
    Debug.print(Adr);
    Debug.println(F(" ERROR not a loco!"));
    #endif
    return;
  }
  #if defined(LOCONET)
  switch (steps) {
    case 14: sendLNSPD(Adr, map(speed, 0, 14, 0, 128)); break;
    case 28: sendLNSPD(Adr, map(speed, 0, 28, 0, 128)); break;
    default: sendLNSPD(Adr, speed);
  }
  #endif
  #if defined(XPRESSNET)
  XpressNet.setSpeed(Adr, steps, speed);
  XpressNet.ReqLocoBusy(Adr);   //Lok wird nicht von LokMaus gesteuert!
  #endif
  switch (steps) {
    case 14: dcc.setSpeed14(Adr, speed); break;
    case 28: dcc.setSpeed28(Adr, speed); break;
    default: dcc.setSpeed128(Adr, speed);
  }
  if (notifyz21_Lan_LocoSpeed)                                                                                // 15.01.22:
    {
    if (steps != 128)
       {
       Debug.print(F("Attention: Steps must be set to 128 in the Z21 App for loko adr:")); // Bei 28 Stufen kommt seltsames Zeug von der Z21 App
       Debug.println(Adr);                                                                 // Bei FS:  0  1  2  3  4  5  6  7  8  9 10 11 ...
       }                                                                                   //     Erg: 0  2  18 3  19 4  20 5  21 6 22 7
    else notifyz21_Lan_LocoSpeed(Adr, speed);
    }
  #if defined(Z21DEBUG)
  Debug.print(F("Z21 A:"));
  Debug.print(Adr);
  Debug.print(", S");
  Debug.print(steps);
  Debug.print(":");
  Debug.println(speed, BIN);
  #endif
}

//------------------------------------------------------------
void notifyz21Accessory(uint16_t Adr, bool state, bool active)
//------------------------------------------------------------
// Is called when an accessory command is received from WLAN and (inderect) from CAN
{
  dcc.setBasicAccessoryPos(Adr, state, active);

  Display_Accessory(Adr, state, active); // Print on OLED Display
  Dprintf("Accessory Adr:%i State:%i Power:%i\n", Adr+1, state, active);                                      // 14.07.22:
  Send_Acc_to_CAN(Adr, state, active);
  /*
   WLANmouse W0013 Taster Gerade:
   accessory 12 1 1
   accessory 12 1 0    Kommt beim loslassen der Taste

   Taste Abzweig
   accessory 12 0 1
   accessory 12 0 0

   Die Z21 App liefert genau die gleichen Werte. Auch hier liefert die Weichen Adresse 13 die 12 in der Debugaugabe.
   Hier kann man eine Weiche nur umschalten.
   Beide Zeilen kommen sofort hintereinander den die Maustaste losgelassen wird.
   Mit "Halten" kommt man im Editor zu den Einstellungen.
   Die Anzeige auf in der App und auf der WLANMaus sind synchron. Getestet mit 3 gleichzeig laufenden Z21 Apps und einer Maus.
  */
}

//--------------------------------------------------------------------------------------------
uint8_t notifyz21AccessoryInfo(uint16_t Adr)
//return state of the Address (left/right = true/false)
{
  #if defined(Z21DEBUG)
  Debug.print("Z21 GetAccInfo: ");
  Debug.print(Adr);
  Debug.print("-");
  Debug.println(dcc.getBasicAccessoryInfo(Adr));
  #endif
  return dcc.getBasicAccessoryInfo(Adr);
}

#if defined(LOCONET)
//--------------------------------------------------------------------------------------------
uint8_t notifyz21LNdispatch(uint16_t Adr)
//return the Slot that was dispatched, 0xFF at error!
{
  #if defined(Z21DEBUG)
  Debug.print("Z21 LNdispatch: ");
  Debug.println(Adr);
  #endif
  return LNdispatch(Adr);
}
//--------------------------------------------------------------------------------------------
void notifyz21LNSendPacket(uint8_t *data, uint8_t length)
{
  #if defined(LnDEB)
    Debug.print(F("LOCONET_FROM_LAN "));
  #endif
  LNSendPacket (data, length, Z21bcLocoNet_s, false);   //Report nicht an Z21 Library!
  LNdecode(false); //verarbeite empfangene Daten, aber nicht als Z21 RX!
}

//--------------------------------------------------------------------------------------------
void notifyz21LNdetector(uint8_t client, uint8_t typ, uint16_t Adr) {
  #if defined(Z21DEBUG)
  Debug.print(F("LAN_LOCONET_DETECTOR 0x"));
  Debug.print(Adr);
  Debug.print("-");
  Debug.print(typ, HEX);
  #endif
  if (typ == 0x80) { //SIC Abfrage
    byte data[4];
    data[0] = 0x01; //Typ
    data[1] = Adr & 0xFF;
    data[2] = Adr >> 8;
    data[3] = dcc.getBasicAccessoryInfo(Adr); //Zustand Rückmelder
    #if defined(Z21DEBUG)
    Debug.print(F(" State: "));
    Debug.println(data[3], BIN);
    #endif
    z21.setLNDetector(client, data, 4);
  }
  #if defined(Z21DEBUG)
  else Debug.println();
  #endif
}

#endif  //LOCONET

/*
//--------------------------------------------------------------------------------------------
void notifyz21CANdetector(uint8_t client, uint8_t typ, uint16_t ID) {
  #if defined(Z21DEBUG)
  Debug.print(F("CAN_DETECTOR "));
  Debug.print(typ);
  Debug.print("=");
  Debug.println(ID);
  #endif
}

//z21.setCANDetector(uint16_t NID, uint16_t Adr, uint8_t port, uint8_t typ, uint16_t v1, uint16_t v2);
*/

//--------------------------------------------------------------------------------------------
//read out the current of the booster main
void notifyz21getSystemInfo(uint8_t client) {
#if defined(BOOSTER_INT_MAINCURRENT)
  #if defined(AREF_1V1)
  uint16_t inAm = VAmpINT / senseResist * 1000 * (Uref/1024);  //adjusted to Uref more accurate
  #else
  uint16_t inAm = VAmpINT * 10; //old!
  #endif

  #ifdef DALLASTEMPSENSE
    float temp = sensors.getTempCByIndex(0);
    sensors.requestTemperatures(); // Send the command to get temperatures

  #elif defined(MEGA_MCU)
    float temp = analogRead(TempPin);
    #if defined(AREF_1V1)
    temp = (1024-temp) / (temp / 34.5);
    #else
    temp = (1024-temp) / (temp / 11.5);
    #endif

  #else //other MCU:
    float temp = 0;
  #endif

  #if defined(MEGA_MCU)
    #if defined(AREF_1V1)
    uint16_t volt = ((float) analogRead(VoltIntPin) * 1000 * (100/4.7) * (Uref/1024)); //adjusted to Uref more accurate
    #else
    uint16_t volt = ((float)(analogRead(VoltIntPin)-121) / 0.008); //old!
    #endif
  #else  //other MCU:
    uint16_t volt = 0;
  #endif

  #if defined(Z21SYSTEMDATADEBUG)
  Debug.print("mA: ");
  Debug.print(inAm);
  Debug.print("(");
  Debug.print(VAmpINT); //Volt on VAmpIntPin
  Debug.print(")_V");

  //#if defined(MEGA_MCU)
  //Debug.print(analogRead(VAmSencePin)-512);  //AC 5A Sensor (for testing only)
  //#endif

  #if defined(VoltIntPin) //ESP8266 has only one ADC for current sense!
  Debug.print(analogRead(VoltIntPin));
  Debug.print(":");
  #endif
  Debug.print(volt);  //Rail Voltage: Rail:100k - Sence - 4,7k - GND
  Debug.print("_T:");
  Debug.println(temp);
  #endif

  #if defined(Z21DISPLAY)
  DisplayUpdateRailData(inAm,volt,temp);
  #endif

  z21.sendSystemInfo(client, inAm, volt, temp);  //report System State to z21 client
  //(12-22V): 20V=0x4e20, 21V=0x5208, 22V=0x55F0
#else
  z21.sendSystemInfo(client, 0, 0, 0);  //report zero System State to z21 client
#endif
}


//--------------------------------------------------------------------------------------------
void notifyz21CVREAD(uint8_t cvAdrMSB, uint8_t cvAdrLSB)
{
  #if defined(DCC)
    unsigned int cvAdr = cvAdrMSB << 8 | cvAdrLSB;
    #if defined(Z21DEBUG)
      Debug.print(F("Z21 read CV#"));
      Debug.println(cvAdr+1);
    #endif

    dcc.opsReadDirectCV(cvAdr);  //read cv
  #endif
}

//--------------------------------------------------------------------------------------------
void notifyz21CVWRITE(uint8_t cvAdrMSB, uint8_t cvAdrLSB, uint8_t value)
{
  #if defined(DCC)
    unsigned int cvAdr = cvAdrMSB << 8 | cvAdrLSB;
    #if defined(Z21DEBUG)
      Debug.print(F("Z21 set CV#"));
      Debug.print(cvAdr+1);
      Debug.print(" - ");
      Debug.println(value);
    #endif

    dcc.opsProgDirectCV(cvAdr,value);  //return value from DCC via 'notifyCVVerify'
   #endif
}

//--------------------------------------------------------------------------------------------
void notifyz21CVPOMWRITEBYTE(uint16_t Adr, uint16_t cvAdr, uint8_t value)
{
  #if defined(RCDEB)
  Debug.print(F("Z21 POM Byte A"));
  Debug.print(Adr);
  Debug.print(F(" set CV#"));
  Debug.print(cvAdr+1);
  Debug.print(" - ");
  Debug.println(value);
  #endif

  #if defined(DCC)
  dcc.opsProgramCV(Adr, cvAdr, value);  //set decoder byte
  #endif
}

//--------------------------------------------------------------------------------------------
void notifyz21CVPOMWRITEBIT(uint16_t Adr, uint16_t cvAdr, uint8_t value)
{
  #if defined(RCDEB)
  Debug.print(F("Z21 POM Bit A"));
  Debug.print(Adr);
  Debug.print(F(" set CV#"));
  Debug.print(cvAdr+1);
  Debug.print(" - ");
  Debug.println(value);
  #endif

  #if defined(DCC)
  dcc.opsPOMwriteBit(Adr, cvAdr, value);  //set decoder bit
  #endif
}

//--------------------------------------------------------------------------------------------
void notifyz21CVPOMREADBYTE (uint16_t Adr, uint16_t cvAdr)
{
  #if defined(DCCGLOBALDETECTOR) && defined(DCC)

  #if defined(RCDEB)
  Debug.print(F("Z21 POM A"));
  Debug.print(Adr);
  Debug.print(" read CV#");
  Debug.println(cvAdr+1);
  #endif

  RailcomCVAdr = cvAdr;

  #if defined(MEGA_MCU)
  RailComStart(); //Start Reading Data!
  #endif

  #endif

  #if defined(DCC)
  dcc.opsPOMreadCV(Adr, cvAdr);  //get decoder value
  #endif
}

//--------------------------------------------------------------------------------------------
//Information to DCC Libray via EEPROM (50 to 75) over RailCom, ProgModus, etc.
void notifyz21UpdateConf() {
  #if defined(Z21DEBUG)
  Debug.println("Z21 Conf: ");
  Debug.print(F("RailCom: ")); //RailCom: 0=aus/off, 1=ein/on
  Debug.print(FIXSTORAGE.read(EEPROMRailCom), HEX);
  Debug.print(F(", PWR-B: ")); //Power-Button: 0=Gleisspannung aus, 1=Nothalt
  Debug.print(FIXSTORAGE.read(52), HEX);
  Debug.print(F(", Prog-R: ")); //Auslese-Modus: 0=Nichts, 1=Bit, 2=Byte, 3=Beides
  Debug.print(FIXSTORAGE.read(53), HEX);
  Debug.print(F(", RstP(s): "));
  Debug.print(FIXSTORAGE.read(EEPROMRSTsRepeat));
  Debug.print(F(", RstP(f): "));
  Debug.print(FIXSTORAGE.read(EEPROMRSTcRepeat));
  Debug.print(F(", ProgP: "));
  Debug.print(FIXSTORAGE.read(EEPROMProgRepeat));
  Debug.print(F(", MainV: "));
  Debug.print(word(FIXSTORAGE.read(73),FIXSTORAGE.read(72)));
  Debug.print(F(", ProgV: "));
  Debug.println(word(FIXSTORAGE.read(75),FIXSTORAGE.read(74)));
  #endif

  #if defined(DCC)
  dcc.loadEEPROMconfig();
  #endif
}

//--------------------------------------------------------------------------------------------
//Information Abfrage Z21 Library
uint8_t notifyz21ClientHash(uint8_t client) {

  byte HashIP = 0x00;

  #if defined(LAN) || defined(ESP_WIFI)
  if (client < Z21_Eth_offset) {  //Prüfe über Offset ob WiFi or LAN
  #endif
    #if defined(WIFI)
    //get Client IP-Hash from WiFi
    HashIP = listofWLANHash[client - 1];
    #endif
  #if defined(LAN) || defined(ESP_WIFI)
  }
  else {
    //get Client IP-Hash from Ethernet Interface
    byte cl = client - Z21_Eth_offset - 1;  //senden ohne Offset!
    HashIP = mem[cl].IP0 ^ mem[cl].IP1 ^ mem[cl].IP2 ^ mem[cl].IP3; //make Hash from IP
  }
  #endif

  #if defined(Z21DEBUG) || REPORT_WIFI_CLIENTS                                                                // 12.07.22:  Added: REPORT_WIFI_CLIENTS
  Debug.print(client);
  Debug.print(F(" IPHash: "));
  Debug.print(HashIP, HEX);
  Debug.print(F(" BC: "));
  Debug.println(FIXSTORAGE.read(0x200 | HashIP), BIN);
  #endif

  return HashIP;  //not found!
}

//--------------------------------------------------------------------------------------------
//bug fix for enc28 Ethernet shield with UIP Library - need to do this after each packet!
#if defined (LAN) && defined (ENC28)
void restartENC28() {
  Udp.stop(); // added for enc28
  #if defined(Z21SYSTEMDATADEBUG)
    Debug.print("restart connection: ");// added for enc28
    Debug.println(Udp.begin(z21Port) ? "success" : "failed");// added for enc28
  #else
    Udp.begin(z21Port);
  #endif
}
#endif

//--------------------------------------------------------------------------------------------
void notifyz21EthSend(uint8_t client, uint8_t *data)
{
  #if defined(Z21DATADEBUG) || Z21DATADEBUG_LONG                                                              // 12.07.22:  Added Z21DATADEBUG_LONG
  if (Z21DATADEBUG_LONG == 0 || data[0] > 8)                                                                 //   "
     {
     Debug.print(client);
     Debug.print(F(" Z21 TX: "));
     for (byte i = 0; i < data[0]; i++) {
       if (data[i]<0x0F) Debug.print('0');                                                                    // 12.07.22:  Leading 0
       Debug.print(data[i], HEX);
       Debug.print(" ");
       }
     Debug.println();
     }
  #endif
  if (client == 0) { //all stored
    #if defined(LAN) || defined(ESP_WIFI)
    for (byte i = 0; i < storedIP; i++) {
      if (mem[i].time > 0) {  //noch aktiv?
        IPAddress ip(mem[i].IP0, mem[i].IP1, mem[i].IP2, mem[i].IP3);
        Udp.beginPacket(ip, mem[i].port);    //Broadcast
        Udp.write(data, data[0]);
        Udp.endPacket();
        #if defined (LAN) && defined (ENC28)
        restartENC28(); //fix bug!
        #endif
      }
    }
    #endif

    #if defined(WIFI)
    WLAN.write(client);
    WLAN.write(data, data[0]);
    #endif
  }
  else {
    #if defined(LAN) || defined(ESP_WIFI)
    if (client < Z21_Eth_offset) {  //Prüfe über Offset ob WiFi or LAN
    #endif
      #if defined(WIFI)
      WLAN.write(client);
      WLAN.write(data, data[0]);
      #endif
    #if defined(LAN) || defined(ESP_WIFI)
    }
    else {
      byte cl = client - Z21_Eth_offset - 1;  //senden ohne Offset!
      IPAddress ip(mem[cl].IP0, mem[cl].IP1, mem[cl].IP2, mem[cl].IP3);
      Udp.beginPacket(ip, mem[cl].port);    //no Broadcast
      Udp.write(data, data[0]);
      Udp.endPacket();
    }
    #endif
  }
  #if defined (LAN) && defined (ENC28)
  restartENC28();  //fix bug!
  #endif
}

//---------------------------------
#endif
