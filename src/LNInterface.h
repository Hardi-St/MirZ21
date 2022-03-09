//--------------------------------------------------------------
/*

  LocoNet Master Interface with optional Slot Server
  
Functions:
- drive with and without slot Server
- control turnout switch
- Dispatch (put via Z21)
- fix direction problem in client mode
- add decoding data from Z21 Protokoll and insert into LocoNet
- add programming packages for JMRI Decoder Pro communikation
- add ignore data when slot is empty - no loco set (missing Slot request OPC_LOCO_ADR)

Copyright (c) by Philipp Gahtow, year 2022
  
*/
#if defined(LOCONET)

//**************************************************************
/* default: als SLOT Server Lok-Ereignisse ins LocoNet senden (Master-Mode only!) */
#define TXAllLokInfoOnLN true  //sende alle Lok-Ereignisse ins LocoNet (MASTER-MODE only) - default: true*
#define LnInvDir               //vertauscht die Fahrtrichtung bei Verwendung der Intellibox I oder II als LocoNet Master oder für DAISY II Handregler
//**************************************************************

//STAT1 => D2 = SL_SPDEX, D1 = SL_SPD14, D0 = SL_SPD28
#define LNLOCO14  0x02 //14 Speed Steps
#define LNLOCO28  0x01 //28 Speed Steps
#define LNLOCO128 0x07 //128 Speed Steps

//Setup up PIN-Configuration for different MCU
#include "MCU_config.h"

#if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
#include "z21header.h"
#endif

static void LNupdate();

//config:
#if defined(UNO_MCU) || defined(__AVR_ATmega644P__)
  #if !defined(WIFI) && !defined(LAN)
  #define MaxSlot 20
  #else
  #define MaxSlot 10
  #endif
#else
#define MaxSlot 120    //max. 120 Slots
#endif

typedef struct	//SLOT
{
  uint16_t LAdr;
  uint8_t Status;  
  /* Slot Status 1
      D7-SL_SPURGE
        1=SLOT purge en,ALSO adrSEL (INTERNAL use only, not seen on NET!)
        CONDN/CONUP: bit encoding-Control double linked Consist List

    2 BITS for Consist
      D6-SL_CONUP
      D3-SL_CONDN
        11=LOGICAL MID CONSIST , Linked up AND down
        10=LOGICAL CONSIST TOP, Only linked downwards
        01=LOGICAL CONSIST SUB-MEMBER, Only linked upwards
        00=FREE locomotive, no CONSIST indirection/linking

    2 BITS for BUSY/ACTIVE
      D5-SL_BUSY
      D4-SL_ACTIVE
        11=IN_USE loco adr in SLOT -REFRESHED
        10=IDLE loco adr in SLOT, not refreshed
        01=COMMON loco adr IN SLOT, refreshed
        00=FREE SLOT, no valid DATA, not refreshed

    3 BITS for Decoder TYPE encoding for this SLOT
      D2-SL_SPDEX
      D1-SL_SPD14
      D0-SL_SPD28
        010=14 step MODE
        001=28 step. Generate Trinary packets for this Mobile ADR
        000=28 step/ 3 BYTE PKT regular mode
        011=128 speed mode packets
        111=128 Step decoder, Allow Advanced DCC consisting
        100=28 Step decoder ,Allow Advanced DCC consisting
   */
} TypeSlot;

TypeSlot slot[MaxSlot];

//define a 
#if defined(ESP32_MCU)
lnMsg LnPacket;
#define LnPacketData LnPacket.data
uint8_t LNDataLength = 0;
#else   //other MCUs
lnMsg *LnPacket;
#define LnPacketData LnPacket->data
#endif

#if defined(LnBufferUSB)
static   LnBuf        LnTxBuffer;
#define  RX_BUF_LOW   32 
#define  RX_BUF_HIGH  96
#endif

byte dispatchSlot = 0;    //To put and store a SLOT for DISPATCHING
boolean dpgetSLot = false;
boolean LNgetNext = false; //Z21 hat eine Meldung auf den LocoNet-Bus geschrieben?
boolean LNNextTX = false;
//--------------------------------------------------------------------------------------------
//Define new OPC:
#ifndef OPC_UHLI_FUN
#define OPC_UHLI_FUN   0xD4  //Function 9-28 by Uhlenbrock 
#endif
#ifndef OPC_MULTI_SENSE
#define OPC_MULTI_SENSE 0xD0  //power management and transponding
#endif

/*
//--------------------------------------------------------------------------------------------
//get local the msg length without library!
#if defined(ESP32_MCU)
uint8_t getLnMsgSize(lnMsg *Msg) {
  //return ( ( Msg->sz.command & (uint8_t)0x60 ) == (uint8_t)0x60 ) ? Msg->sz.mesg_size : ( ( Msg->sz.command & (uint8_t)0x60 ) >> (uint8_t)4 ) + 2 ;
  //return lnPacketSize(Msg);
  return Msg->sz.mesg_size;
}
#endif
*/

//--------------------------------------------------------------------------------------------
#if defined(LnDEB)
//Print LN Message
void LNDebugPrint () {
  #if defined(ESP32_MCU)
  uint8_t msgLen = LNDataLength;
  #else
  uint8_t msgLen = getLnMsgSize(LnPacket);   //get Length of Data
  #endif
   // First print out the packet in HEX
        for (uint8_t x = 0; x < msgLen; x++)  {
            // Print a leading 0 if less than 16 to make 2 HEX digits
          if(LnPacketData[x] < 16)
            Debug.print('0');
          Debug.print(LnPacketData[x], HEX);
          Debug.print(' ');
        }
        Debug.println();
}
#endif

//--------------------------------------------------------------------------------------------
#if defined(LnBufferUSB)
void ReportLnBuffer() {
      // Get the length of the received packet
      uint8_t Length = getLnMsgSize( LnPacket ) ;
      // Send the received packet out byte by byte to the PC
      for( uint8_t Index = 0; Index < Length; Index++ )
        #if defined(ARDUINO) && ARDUINO >= 100
          Serial.write(LnPacket->data[ Index ]);
        #else 
          Serial.print(LnPacket->data[ Index ], BYTE);
        #endif
}
#endif

//--------------------------------------------------------------------------------------------
//send bytes into LocoNet
bool LNSendPacket (byte *data, byte len, byte Z21bcType, bool Z21TX = true) 
{
  lnMsg SendPacket;
  
  #if defined(ESP32_MCU)
  for (byte i = 0; i < (len-1); i++) {
    SendPacket.data[i] = *data; //prepare lnMsg to send
    LnPacketData[i] = SendPacket.data[i]; //make a copy
    data++;
  }
  sendMsg(&bus,SendPacket);
  LNDataLength = len;   //save the length of the copy
  
  #else
  for (byte i = 0; i < (len-1); i++) {
    SendPacket.data[i] = *data;
    data++;
  }
  #if defined(LnSLOTSRV)
  LN_STATUS verify = LocoNet.send( &SendPacket, 20); //20 = min carrier detect backoff and we are the Master!
  #else
  LN_STATUS verify = LocoNet.send( &SendPacket); //normal
  #endif  
  #if defined(LnDEB) || defined(DEBUG)
  if (verify != LN_DONE) {
    Debug.print(verify);
    Debug.println(F(" LnSend ERROR!"));
  }
  #endif
    
  #endif

  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  if (Z21TX == true)
    z21.setLNMessage(SendPacket.data, len, Z21bcType, Z21TX);   
  #endif

  #if !defined(ESP32_MCU)
  LnPacket = LocoNet.receive();

  #if defined(LnBufferUSB)
  ReportLnBuffer();  
  #endif
  
  if (SendPacket.data[len-1] == LnPacket->data[len-1] && SendPacket.data[0] == LnPacket->data[0] && SendPacket.data[1] == LnPacket->data[1]) {
    //LNNextTX = true;
    #if defined(LnDEB)
    Debug.print("LnTX: ");
    LNDebugPrint();
    #endif
  }
  else {
    LNgetNext = true;
    LNupdate();
  }
  #endif
  
  return true;
}

//--------------------------------------------------------------------------------------------
//Status req
void LNGetLocoStatus(byte Slot) {
  //default Answer:
  byte SLOT_DATA_READ[] = {OPC_SL_RD_DATA, 0x0E, Slot, 0, 0, 0, 0, 0x07, 0, 0, 0, 0, 0};
  if ((Slot != 0) && (Slot < MaxSlot)) {    //wenn Lok im SLOT Server vorhanden ist --> update
    uint8_t ldata[6];
    AllLocoData(slot[Slot].LAdr, ldata);  //uint8_t Steps[0], uint8_t Speed[1], uint8_t F0[2], uint8_t F1[3], uint8_t F2[4], uint8_t F3[5]

    // DIRF = 0,0,DIR,F0,F4,F3,F2,F1 
    // SND = 0,0,0,0,F8,F7,F6,F5 

    #if defined(LnInvDir)
    byte DIRF = (((~ldata[1]) >> 2) & 0x20) | (ldata[2] & 0x1F);
    #else
    byte DIRF = ((ldata[1] >> 2) & 0x20) | (ldata[2] & 0x1F);
    #endif

    /*
    #if defined(LnDEB)
    Debug.print(F("OPC_SL_RD_DATA "));
    Debug.print(Slot);
    #endif
    */
    
    SLOT_DATA_READ[3] = slot[Slot].Status;
    SLOT_DATA_READ[4] = lowByte(slot[Slot].LAdr & 0x7F);
    SLOT_DATA_READ[5] = lowByte(ldata[1] & 0x7F);
    SLOT_DATA_READ[6] = DIRF;
    SLOT_DATA_READ[9] = lowByte((slot[Slot].LAdr >> 7) & 0x7F);
    SLOT_DATA_READ[10] = lowByte(ldata[3] & 0x0F);
  }
  //only report Lok Slots, no system slots!
  if (Slot < 120)
    LNSendPacket (SLOT_DATA_READ, 0x0E, Z21bcLocoNet_s, true);
}

#if defined(LnSLOTSRV)
//--------------------------------------------------------------------------------------------
byte LNGetSetLocoSlot(unsigned int Adr, bool add) {   //add command for MASTER-Mode only!
  if (Adr == 0)
    return 0;
  byte getSlot = 0;
  for (byte i = 1; i < MaxSlot; i++) {
    if (slot[i].LAdr == Adr)
      return i;  //already inside a SLOT
    if ((getSlot == 0) && (((slot[i].Status & 0x30) >> 4) == 0))
      getSlot = i;  //find a empty SLOT  
  }
  if (getSlot != 0 && add == true) {    //add Adr to SLOT Server!
    slot[getSlot].Status = 0x10;   //ACTIVE - COMMON loco adr IN SLOT
    #if defined(FS14)
    slot[getSlot].Status |= LNLOCO14;
    #elif defined(FS28)
    slot[getSlot].Status |= LNLOCO28;
    #else
    slot[getSlot].Status |= LNLOCO128; 
    #endif
    slot[getSlot].LAdr = Adr;
    
    #if defined(LnDEB)
    Debug.print(F("A:"));
    Debug.print(slot[getSlot].LAdr);
    Debug.print(F(" in Slot: "));
    Debug.println(getSlot);
    #endif

    //Report Slot DATA:
    LNGetLocoStatus(getSlot);
    
    return getSlot;
  }
  return 0;
}
#else //LN Slave-Mode
//--------------------------------------------------------------------------------------------
//Find Slot via Address from the LocoNet Slot Master
byte LNGetSetLocoSlot (unsigned int Adr, bool add) { //add only for MASTER-Mode!
  if (Adr == 0)
    return 0;
  for (byte i = 1; i < MaxSlot; i++) {
    if (slot[i].LAdr == Adr) { // && (getLNSlotState(Slot) != 0)) {
      return i;    //Vorhanden!
    }
  }
  //If not: Master puts Address into a SLOT
  byte getSlot[] = {OPC_LOCO_ADR, lowByte(Adr >> 7), lowByte(Adr & 0x7F), 0x00};  //Request loco address
  LNSendPacket(getSlot, 4, Z21bcLocoNetLocos_s, true);  
  return 0;
}

//--------------------------------------------------------------------------------------------
//Get Slot Information
void LNGetSlotInfo (byte Slot) {
  byte getSlotInfo[] = {OPC_RQ_SL_DATA, Slot, 0, 0x00};   //Request Slot data/status
  LNSendPacket(getSlotInfo, 4, Z21bcLocoNetLocos_s, true);
}

//--------------------------------------------------------------------------------------------
#endif  //LN Slave Mode

//--------------------------------------------------------------------------------------------
//Set slot direction, function 0-4 state 
// DIRF = 0,0,DIR,F0,F4,F3,F2,F1 
void sendLNDIRF (unsigned int Adr, byte DIRF) {
  byte Slot = LNGetSetLocoSlot(Adr, TXAllLokInfoOnLN);
  if (Slot > 0) {
    #if defined(LnInvDir)
    byte setDIRF[] = {OPC_LOCO_DIRF, Slot, lowByte( ((~DIRF) & 0x20) | (DIRF & 0x1F) ), 0x00};
    #else
    byte setDIRF[] = {OPC_LOCO_DIRF, Slot, DIRF, 0x00};
    #endif
    LNSendPacket(setDIRF, 4, Z21bcLocoNetLocos_s, true);  
  }
}
//--------------------------------------------------------------------------------------------
//Set slot speed and update dir
void sendLNSPD (unsigned int Adr, byte SPD) {
  byte Slot = LNGetSetLocoSlot(Adr, TXAllLokInfoOnLN);
  if (Slot == 0)
    return;
  //Check if direction will change also?  
  if (dcc.getLocoDir(Adr) != (SPD >> 7)) {
      //calculate new direction:
      byte DIRF = dcc.getFunktion0to4(Adr) | ((SPD >> 2) & 0x20);
      sendLNDIRF(Adr, DIRF);
  }
  byte setSPD[] = {OPC_LOCO_SPD, Slot, 0x7F, 0x00};
  setSPD[2] &= SPD;
  LNSendPacket(setSPD, 4, Z21bcLocoNetLocos_s, true);  
}
//--------------------------------------------------------------------------------------------
//Set slot second function
// SND = 0,0,0,0,F8,F7,F6,F5 
void sendLNSND (unsigned int Adr, byte SND) {
  byte Slot = LNGetSetLocoSlot(Adr, TXAllLokInfoOnLN);
  if (Slot > 0) {
    byte setSND[] = {OPC_LOCO_SND, Slot, SND, 0x00};
    LNSendPacket(setSND, 4, Z21bcLocoNetLocos_s, true);  
  }
}

//--------------------------------------------------------------------------------------------
// F3 = 0,0,0,0,F12,F11,F10,F9 
void sendLNF3 (unsigned int Adr, byte F3) {
  byte Slot = LNGetSetLocoSlot(Adr, TXAllLokInfoOnLN);
  if (Slot > 0) {
    byte setF3[] = {0xA3, Slot, F3, 0x00};
    LNSendPacket(setF3, 4, Z21bcLocoNetLocos_s, true);  
  }
}

//--------------------------------------------------------------------------------------------
// F4 = F20,F19,F18,F17,F16,F15,F14,F13 
void sendLNF4 (unsigned int Adr, byte F4) {
  byte Slot = LNGetSetLocoSlot(Adr, TXAllLokInfoOnLN);
  if (Slot > 0) {
    byte setF4[] = {OPC_UHLI_FUN, 0x20, Slot, 0x08, 0x7F, 0x00}; // - F19-F13
    setF4[4] &= F4;
    LNSendPacket(setF4, 6, Z21bcLocoNetLocos_s, true);  
    byte F2028 = ((dcc.getFunktion21to28(Adr) & 0x80) >> 1) | ((F4 & 0x80) >> 2);  // - f28, f20, ---
    byte setF2028[] = {OPC_UHLI_FUN, 0x20, Slot, 0x05, F2028, 0x00};  // - F28,F20 ----
    LNSendPacket(setF2028, 6, Z21bcLocoNetLocos_s, true); 
  }
}
//--------------------------------------------------------------------------------------------
// F5 = F28,F27,F26,F25,F24,F23,F22,F21 
void sendLNF5 (unsigned int Adr, byte F5) {
  byte Slot = LNGetSetLocoSlot(Adr, TXAllLokInfoOnLN);
  if (Slot > 0) {
    byte setF5[] = {OPC_UHLI_FUN, 0x20, Slot, 0x09, 0x7F, 0x00}; // - F27-F21
    setF5[4] &= F5;
    LNSendPacket(setF5, 6, Z21bcLocoNetLocos_s, true);  
    byte F2028 = ((F5 & 0x80) >> 1) | ((dcc.getFunktion13to20(Adr) & 0x80) >> 2);  // - f28, f20, ---
    byte setF2028[] = {OPC_UHLI_FUN, 0x20, Slot, 0x05, F2028, 0x00};  // - F28,F20 ----
    LNSendPacket(setF2028, 6, Z21bcLocoNetLocos_s, true); 
  }
}
//--------------------------------------------------------------------------------------------
//Uhlenbrock F9 to F12 Message: A3 Slot F9-12 HEX (A3 09 01 54; A3 09 00 55)
//Uhlenbroch F13 to F20 and F21 to F28: D4 20 Slot F13-20 F20-21 HEX (D4 20 09 08 01 0B; D4 20 09 08 02 08)
/*
 * 18:39:03.972: [Rx - A3 03 01 5E]  Set (Intellibox-II format) loco in slot 3
F9=On F10=Off F11=Off F12=Off.
18:39:06.109: [Rx - A3 03 00 5F]  Set (Intellibox-II format) loco in slot 3
F9=Off F10=Off F11=Off F12=Off.
18:39:07.061: [Rx - A3 03 02 5D]  Set (Intellibox-II format) loco in slot 3
F9=Off F10=On F11=Off F12=Off.
18:39:07.934: [Rx - A3 03 00 5F]  Set (Intellibox-II format) loco in slot 3
F9=Off F10=Off F11=Off F12=Off.
18:39:10.290: [Rx - A3 03 04 5B]  Set (Intellibox-II format) loco in slot 3
F9=Off F10=Off F11=On F12=Off.
18:39:11.023: [Rx - A3 03 00 5F]  Set (Intellibox-II format) loco in slot 3
F9=Off F10=Off F11=Off F12=Off.
18:39:13.519: [Rx - A3 03 08 57]  Set (Intellibox-II format) loco in slot 3
F9=Off F10=Off F11=Off F12=On.
18:39:14.205: [Rx - A3 03 00 5F]  Set (Intellibox-II format) loco in slot 3
F9=Off F10=Off F11=Off F12=Off.

18:39:15.188: [Rx - D4 20 03 08 01 01]  Set (Intellibox-II format) loco in
slot 3 F13=On F14=Off F15=Off F16=Off F17=Off F18=Off F19=Off
18:39:16.483: [Rx - D4 20 03 08 00 00]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=Off F17=Off F18=Off F19=Off
18:39:23.441: [Rx - D4 20 03 08 02 02]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=On F15=Off F16=Off F17=Off F18=Off F19=Off
18:39:26.576: [Rx - D4 20 03 08 00 00]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=Off F17=Off F18=Off F19=Off
18:39:28.823: [Rx - D4 20 03 08 04 04]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=On F16=Off F17=Off F18=Off F19=Off
18:39:30.227: [Rx - D4 20 03 08 00 00]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=Off F17=Off F18=Off F19=Off
18:39:42.176: [Rx - D4 20 03 08 08 08]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=On F17=Off F18=Off F19=Off
18:39:45.187: [Rx - D4 20 03 08 00 00]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=Off F17=Off F18=Off F19=Off
18:39:47.121: [Rx - D4 20 03 08 10 10]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=Off F17=On F18=Off F19=Off
18:39:48.291: [Rx - D4 20 03 08 00 00]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=Off F17=Off F18=Off F19=Off
18:40:02.300: [Rx - D4 20 03 08 20 20]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=Off F17=Off F18=On F19=Off
18:40:03.486: [Rx - D4 20 03 08 00 00]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=Off F17=Off F18=Off F19=Off
18:40:04.516: [Rx - D4 20 03 08 40 40]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=Off F17=Off F18=Off F19=On
18:40:05.296: [Rx - D4 20 03 08 00 00]  Set (Intellibox-II format) loco in
slot 3 F13=Off F14=Off F15=Off F16=Off F17=Off F18=Off F19=Off

18:40:09.710: [Rx - D4 20 03 05 20 2D]  Set (Intellibox-II format) loco in
slot 3 F20=On F28=Off
18:40:16.091: [Rx - D4 20 03 05 00 0D]  Set (Intellibox-II format) loco in
slot 3 F20=Off F28=Off

18:40:16.964: [Rx - D4 20 03 09 01 00]  Set (Intellibox-II format) loco in
slot 3 F21=On F22=Off F23=Off F24=Off F25=Off F26=Off F27=Off
18:40:26.449: [Rx - D4 20 03 09 00 01]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=Off F25=Off F26=Off F27=Off
18:40:29.554: [Rx - D4 20 03 09 02 03]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=On F23=Off F24=Off F25=Off F26=Off F27=Off
18:40:31.020: [Rx - D4 20 03 09 00 01]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=Off F25=Off F26=Off F27=Off
18:40:32.252: [Rx - D4 20 03 09 04 05]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=On F24=Off F25=Off F26=Off F27=Off
18:40:33.329: [Rx - D4 20 03 09 00 01]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=Off F25=Off F26=Off F27=Off
18:40:46.885: [Rx - D4 20 03 09 08 09]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=On F25=Off F26=Off F27=Off
18:40:47.790: [Rx - D4 20 03 09 00 01]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=Off F25=Off F26=Off F27=Off
18:40:50.473: [Rx - D4 20 03 09 10 11]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=Off F25=On F26=Off F27=Off
18:40:51.128: [Rx - D4 20 03 09 00 01]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=Off F25=Off F26=Off F27=Off
18:40:56.183: [Rx - D4 20 03 09 20 21]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=Off F25=Off F26=On F27=Off
18:41:00.083: [Rx - D4 20 03 09 00 01]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=Off F25=Off F26=Off F27=Off
18:41:02.142: [Rx - D4 20 03 09 40 41]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=Off F25=Off F26=Off F27=On
18:41:03.484: [Rx - D4 20 03 09 00 01]  Set (Intellibox-II format) loco in
slot 3 F21=Off F22=Off F23=Off F24=Off F25=Off F26=Off F27=Off

18:41:06.292: [Rx - D4 20 03 05 40 4D]  Set (Intellibox-II format) loco in
slot 3 F20=Off F28=On
18:41:07.274: [Rx - D4 20 03 05 00 0D]  Set (Intellibox-II format) loco in
slot 3 F20=Off F28=Off
 */


//--------------------------------------------------------------------------------------------
//Check if Slot can be dispatched
byte LNdispatch (uint16_t Adr) {
  dispatchSlot = LNGetSetLocoSlot(Adr, true);  //add to SLOT
  #if defined(LnDEB)
    Debug.print(Adr);
    Debug.print(F(" Loco in Dispatch Slot: "));
    Debug.println(dispatchSlot);
  #endif
  #if !defined(LnSLOTSRV)  //At Slave Mode ask Master to dispatch 
   if (dispatchSlot > 0) {
    byte SetStat1[] = {OPC_SLOT_STAT1, dispatchSlot, 0x20, 0x00};
    LNSendPacket(SetStat1, 4, Z21bcLocoNetLocos_s, true); 
    byte NullMove[] = {OPC_MOVE_SLOTS, dispatchSlot, 0x00, 0x00};
    LNSendPacket(NullMove, 4, Z21bcLocoNetLocos_s, true); 
   }
   else { //no Slot for loco that should be dispatched - get a Slot!
      dpgetSLot = true;  //get ready for Slot
      return 0;
   }
  #endif
  if (((slot[dispatchSlot].Status & 0x30) >> 4) != B11) {  //not 11=IN_USE
    return dispatchSlot;
  }
  dispatchSlot = 0;  //clear
  return 0;
}

//--------------------------------------------------------------------------------------------
//LocoNet Interface init
void LNsetup() {
  #if defined(ESP32_MCU)
  locoNetPhy.begin();

    parser.onPacket(CALLBACK_FOR_ALL_OPCODES, [](const lnMsg *rxPacket) mutable {

        LNDataLength = rxPacket->length();
        byte equaldata = 0;
        for (byte i = 0; i < LNDataLength; i++) {
          //check if we maybe send this data out?
          if (LnPacket.data[i] == rxPacket->data[i]) {
            equaldata++;  //count bytes that are equal.
          }
          else LnPacket.data[i] = rxPacket->data[i];
        }
        //check if we had tx this message?
        if (equaldata != LNDataLength) {  //(tx from us?)
          LNupdate();   //decode Message
        }
 
    });

  #else
  // First initialize the LocoNet interface
  LocoNet.init(LNTxPin);
  #endif

  #if defined(LnBufferUSB)
  // Configure the serial port for 57600 baud
  Serial.begin(57600);
  // Initialize a LocoNet packet buffer to buffer bytes from the PC 
  initLnBuf(&LnTxBuffer) ;
  
  #if defined(LnBufferUSB_CTS)
     // Configure the CTS pin for hardware flow control to control 
    // the serial data flow from the PC to the LocoNet 
  pinMode(Ln_RX_CTS_PIN,OUTPUT);
  digitalWrite(Ln_RX_CTS_PIN,LOW);
  #endif
  
  #endif
}

//--------------------------------------------------------------------------------------------
//Send railpower state to LocoNet devices
void LNsetpower() {
  byte code[] = { OPC_GPOFF, 0x00};
  if (Railpower == csNormal)
    code[0] = OPC_GPON;
  else if (Railpower == csEmergencyStop)
    code[0] = OPC_IDLE;  //B'cast emerg. STOP
  LNSendPacket(code,2, Z21bcLocoNet_s, true);
}

//--------------------------------------------------------------------------------------------
//Trnt Daten senden
void LNsetTrnt(uint16_t Adr, boolean state, boolean active) {
  //OPC_SW_REQ
  //dcc.setBasicAccessoryPos(LnPacketData[1] | ((LnPacketData[2] & 0x0F) << 7),(LnPacketData[2] >> 5) & 0x01, (LnPacketData[2] >> 4) & 0x01);  //Adr, State, on/off
  byte lAdr = Adr & 0x7F;
  byte hAdr = (Adr >> 7) & 0x0F;
  byte Trnt[] = {OPC_SW_REQ, lAdr, hAdr, 0x00};
  bitWrite(Trnt[2], 5, state);
  bitWrite(Trnt[2], 4, active);
  LNSendPacket (Trnt, 4, Z21bcLocoNet_s, true);  
}

//--------------------------------------------------------------------------------------------
//OPC SL Data Auswerten
void LN_OPC_SL_DATA() {
  if (LnPacketData[2] < MaxSlot) {
    slot[LnPacketData[2]].LAdr = (LnPacketData[9] << 7) | (LnPacketData[4] & 0x7F);    //ADR2 | ADR
    slot[LnPacketData[2]].Status = LnPacketData[3];  //Save new Status
  
                    #if defined(LnInvDir)
                    uint8_t speed = (LnPacketData[5] & 0x7F) | (((~LnPacketData[6]) << 2) & 0x80); //DIR, S, S, S, S, S, S, S
                    #else
                    uint8_t speed = (LnPacketData[5] & 0x7F) | ((LnPacketData[6] << 2) & 0x80); //DIR, S, S, S, S, S, S, S
                    #endif
                     
                    #if defined(LnDEB)
                    Debug.print(slot[LnPacketData[2]].LAdr);
                    Debug.print(", ");
                    Debug.println(speed, BIN);
                    #endif
                                      
    if ((slot[LnPacketData[2]].Status & B111) == LNLOCO14)
       dcc.setSpeed14(slot[LnPacketData[2]].LAdr, speed);  //DIRF & SPD
    else {
      if ((slot[LnPacketData[2]].Status & B111) == LNLOCO28)  
           dcc.setSpeed28(slot[LnPacketData[2]].LAdr, speed);  //DIRF & SPD
      else dcc.setSpeed128(slot[LnPacketData[2]].LAdr, speed);  //DIRF & SPD
    }
    dcc.setFunctions0to4(slot[LnPacketData[2]].LAdr, LnPacketData[6] & 0x1F);  //DIRF = - F0 F4 F3 F2 F1
    dcc.setFunctions5to8(slot[LnPacketData[2]].LAdr, LnPacketData[10] & 0x0F);  //SND = - F8 F7 F6 F5
  }
}

//--------------------------------------------------------------------------------------------
//CV Rückmeldung
void LNsetCVReturn(uint16_t CV, uint8_t value, uint8_t fail) {
  //This <E7> response is issued whenever a Programming task is completed. 
/* [EF 0E 7C 2B 00 00 00 00 00 1C 00 7F 7F 55]  Byte Read in Direct Mode on Service Track: CV29.
 * [81 7E]  Master is busy.
 * [B4 6F 01 25]  LONG_ACK: The Slot Write command was accepted.
 * [E7 0E 7C 2B 01 00 20 07 00 1C 00 7F 7F 7B]  Programming Response: 
 * Read Byte in Direct Mode on Service Track Failed, 
 * Service Mode programming track empty: CV29 value 0 (0x00, 00000000b).
 * 
 * [EF 0E 7C 2B 00 00 00 00 00 1C 00 7F 7F 55]  Byte Read in Direct Mode on Service Track: CV29.
 * [B4 6F 01 25]  LONG_ACK: The Slot Write command was accepted.
 * [E7 0E 7C 2B 00 00 20 07 00 1C 0E 7F 7F 74]  Programming Response: 
 * Read Byte in Direct Mode on Service Track Was Successful: CV29 value 14 (0x0E, 00001110b).
 * [EF 0E 7C 2B 00 00 00 00 00 00 00 7F 7F 49]  Byte Read in Direct Mode on Service Track: CV1.
 * [B4 6F 01 25]  LONG_ACK: The Slot Write command was accepted.
 * [E7 0E 7C 2B 00 00 20 07 00 00 03 7F 7F 65]  Programming Response: 
 * Read Byte in Direct Mode on Service Track Was Successful: CV1 value 3 (0x03, 00000011b).
 * [EF 0E 7C 2B 00 00 00 00 00 06 00 7F 7F 4F]  Byte Read in Direct Mode on Service Track: CV7.
 * [B4 6F 01 25]  LONG_ACK: The Slot Write command was accepted.
 * [E7 0E 7C 2B 00 00 20 07 00 06 23 7F 7F 43]  Programming Response: 
 * Read Byte in Direct Mode on Service Track Was Successful: CV7 value 35 (0x23, 00100011b).
 * [EF 0E 7C 2B 00 00 00 00 00 07 00 7F 7F 4E]  Byte Read in Direct Mode on Service Track: CV8.
 * [B4 6F 01 25]  LONG_ACK: The Slot Write command was accepted.
 * [E7 0E 7C 2B 00 00 20 07 02 07 11 7F 7F 72]  Programming Response: 
 * Read Byte in Direct Mode on Service Track Was Successful: CV8 value 145 (0x91, 10010001b).
 */

  
  #if defined(LnDEB)
  Debug.print("Ln CV: ");
  #endif  
  uint8_t CVH = (CV >> 7) | ((value >> 6) & 0x02) | ((CV >> 4) & 0x30);
  byte SLOT_DATA[] = {OPC_SL_RD_DATA, 0x0E, 0x7C, 0x2B, fail & 0x01, 0, 0x20, 0x07, CVH, CV & 0x7F, value & 0x7F, 0x7F, 0x7F};
  LNSendPacket (SLOT_DATA, 0x0E, Z21bcLocoNet_s, true);
}

//--------------------------------------------------------------------------------------------
//LocoNet Daten dekodieren
void LNdecode(bool Z21Report) {  
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  //Z21 Broadcast-Flag:
  byte LnZ21bcType = Z21bcLocoNet_s;  //Z21bcLocoNet or Z21bcLocoNetLocos or Z21bcLocoNetSwitches
  #endif
  
    switch (LnPacketData[0]) {  //OPC = Operation-Code
        case OPC_SL_RD_DATA: {  //for LN Slave-Mode ONLY
                  #if !defined(LnSLOTSRV)  //At Slave Mode ask Master to dispatch 

                   #if defined(LnDEB)
                   Debug.print(F("OPC_SL_RD_DATA Slot: "));
                   Debug.print(LnPacketData[2]);
                   Debug.print(" ");
                   #endif

                   if (LnPacketData[2] < MaxSlot) {
                      /* Slot: 0  dispatch
                       *       1 - 119   active locos     */
                    
                     LN_OPC_SL_DATA();
  
                     if (dpgetSLot == true) {  //dispatch this Slot!
                      #if defined(LnDEB)
                      Debug.println("Dispatch");
                      #endif
                      if (((slot[LnPacketData[2]].Status & 0x30) >> 4) != 0x01) {    //not BUSY!
                        byte SetStat1[] = {OPC_SLOT_STAT1, LnPacketData[2], lowByte((slot[LnPacketData[2]].Status & 0xC0) | (slot[LnPacketData[2]].Status & 0x0F) | 0x20), 0x00}; //set BUSY
                        LNSendPacket(SetStat1, 4, Z21bcLocoNet_s, true); 
                      }
                      byte NullMove[] = {OPC_MOVE_SLOTS, LnPacketData[2], 0x00, 0x00};
                      LNSendPacket(NullMove, 4, Z21bcLocoNet_s, true); 
                      dpgetSLot = false;  //reset
                     }
                   }
                   #endif
                  break; }
        case OPC_WR_SL_DATA: {  //Write slot data
                  uint8_t SLOT_RX = LnPacketData[2];
                   if (SLOT_RX < MaxSlot) {
                      /* Slot: 0  dispatch
                       *       1 - 119   active locos     */
                      #if defined(LnDEB)
                      Debug.print(F("OPC_WR_SL_DATA: "));
                      #endif

                      LN_OPC_SL_DATA();
                      
                      #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                      z21.setLocoStateExt (slot[SLOT_RX].LAdr);
                      #endif
                      #if defined(XPRESSNET)
                      XpressNet.setSpeed(slot[SLOT_RX].LAdr, 128, (LnPacketData[5] & 0x7F) | ((LnPacketData[6] << 2) & 0x80));
                      XpressNet.setFunc0to4(slot[SLOT_RX].LAdr, LnPacketData[6] & B00011111);
                      XpressNet.setFunc5to8(slot[SLOT_RX].LAdr, LnPacketData[10] & 0x0F);
                      XpressNet.ReqLocoBusy(slot[SLOT_RX].LAdr);   //Lok wird nicht von LokMaus gesteuert!
                      #endif
                   }
                   else { //OPC_WR_PT_DATA
                    /*Slot: 120 - 127   reserved for System and Master control
                     *      123   Fast Clock
                     *      124   Programming Track
                     *      127   Command Station Options */
                     if (SLOT_RX == 0x7C) { //Write PT slot data.
                        uint16_t cvAdr = LnPacketData[9] | ( (LnPacketData[8] & 0x01) << 7) | ( (LnPacketData[8] & 0x30) << 4);
                        uint8_t value = ((LnPacketData[8] << 5) & 0x80) | (LnPacketData[10] & 0x7F);
                        
                        #if defined(LnDEB)
                        Debug.print(F("CV#: "));
                        Debug.print(cvAdr+1);
                        Debug.print(F(" Value: "));
                        Debug.print(value);
                        #endif
                        
                        if (LnPacketData[3] == 0x2B) {   //Read in Direct Mode on Service Track
                          #if defined(LnDEB)
                          Debug.print(F(" Direct Mode Read "));
                          #endif
                          #if defined(DCC)  
                          dcc.opsReadDirectCV(cvAdr);  //read cv
                          #endif
                        }
                        else if (LnPacketData[3] == 23) { //Read in Paged Mode on Service Track
                          #if defined(LnDEB)
                          Debug.print(F(" Paged Mode Read "));
                          #endif
                        }
                        else if (LnPacketData[3] == 24) { //Read in Paged Mode on Service Track
                          #if defined(LnDEB)
                          Debug.print(F(" Paged Mode Read "));
                          #endif
                          #if defined(DCC) 
                          dcc.opsProgDirectCV(cvAdr,value);  //return value from DCC via 'notifyCVVerify'
                          #endif
                        }
                     }
                     else if (SLOT_RX == 0x7B){ //Write Fast Clock slot data. 
                      #if defined(LnDEB)
                      Debug.print(F("OPC_WR_CL_DATA: "));
                      #endif
                     }
                   }
                  //Response:
                  //0=busy/aborted, 1=accepted(OPC_SL_RD_DATA), 0×40=accepted blind(OPC_SL_RD_DATA), 0x7F=not implemented 
                  byte ACK[] = {OPC_LONG_ACK, OPC_WR_SL_DATA & B01111111, 1, 0x00};  
                  LNSendPacket (ACK, 4, LnZ21bcType, true);    //Send ACK
                  LNGetLocoStatus(SLOT_RX);  //Send OPC_SL_RD_DATA
                  break; }
        case OPC_RQ_SL_DATA: //Request slot data/status block
                  #if defined(LnSLOTSRV)
                  LNGetLocoStatus(LnPacketData[1]);
                  #endif
                  break;
        case OPC_LOCO_ADR: { //0xBF = Request loco address
                  #if defined(LnSLOTSRV) 
                  //add to a SLOT:
                  byte newSlot = LNGetSetLocoSlot((LnPacketData[1] << 7) | (LnPacketData[2] & 0x7F), true); //ADR2:7 ms-bits = 0 bei kurzer Adr; ADR:7 ls-bit
                  if (dispatchSlot != 0 && LnPacketData[1] == 0 && LnPacketData[2] == 0)
                    newSlot = dispatchSlot;
                  #if defined(LnDEB)
                  Debug.print("get Slot: ");
                  Debug.println(newSlot);
                  #endif  
                  if (newSlot == 0) {
                    //0xB4 = OPC_LONG_ACK No free slot available 
                    byte Fail[] = {OPC_LONG_ACK, 0x3F, 0x00, 0x00}; //LACK,0 is returned[<B4>,<3F>,<0>,<CHK>]
                    LNSendPacket (Fail, 4, Z21bcLocoNet_s, true);
                    #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                    //z21.setLNMessage(Fail, 4, Z21bcLocoNet_s, false);   
                    #endif
                  }
                  else {
                    //Report Slot DATA:
                    LNGetLocoStatus(newSlot);
                  }
                  #endif
                  break;
        }
        case OPC_MOVE_SLOTS: { //0xBA = Move slot SRC to DST
                  #if defined(LnSLOTSRV) 
                  if (LnPacketData[1] == 0) {  //SRC = 0
                    //SLOT READ DATA of DISPATCH Slot
                    if (dispatchSlot != 0) {
                      slot[dispatchSlot].Status = slot[dispatchSlot].Status | 0x30;  //IN_USE
                      LNGetLocoStatus(dispatchSlot); //Give slot that was DISPATCHED
                      dispatchSlot = 0;  //reset the Dispatch SLOT
                      break;
                    }
                  }
                  else if (LnPacketData[1] == LnPacketData[2]) {  //NULL move
                    //SRC=DEST is set to IN_USE , if legal move -> NULL move
                    slot[LnPacketData[1]].Status = slot[LnPacketData[1]].Status | 0x30;  //B00011111;  //IN_USE
                    LNGetLocoStatus(LnPacketData[1]); 
                    break;
                  }
                  else if (LnPacketData[2] == 0) {  //DST = 0
                    //DISPATCH Put, mark SLOT as DISPATCH;
                    dispatchSlot = LnPacketData[1];
                    //RETURN slot status <0xE7> of DESTINATION slot DEST if move legal
                    LNGetLocoStatus(dispatchSlot); 
                    break;
                  }
                  //RETURN Fail LACK code if illegal move <B4>,<3A>,<0>,<chk>
                  byte Fail[] = {OPC_LONG_ACK, OPC_MOVE_SLOTS & 0x7F, 0x00, 0x00};
                  LNSendPacket (Fail, 4, LnZ21bcType, true);
                  
                  #endif
                  break;
        }
        case OPC_LINK_SLOTS: 
                  break;  //Link slot ARG1 to slot ARG2
        case OPC_UNLINK_SLOTS: 
                  break;  //Unlink slot ARG1 from slot ARG2
        case OPC_SLOT_STAT1: 
                  slot[LnPacketData[1]].Status = LnPacketData[2];
                  break;  
        case 0xBE: { //OPC_EXP_REQ_SLOT (0xBE)
                    /* Example:
                     * Ln: ED 0F 01 49 42 0D 00 15 00 00 00 00 00 00 0F 
                     * Ln: E5 0F 00 49 4B 0B 00 15 00 52 11 15 00 00 5F 
                     * Ln: BE 00 15 54
                     */
                  #if defined(LnDEB)
                  Debug.println("OPC_EXP_REQ_SLOT");
                  #endif  
                  break; }
        case OPC_LOCO_SPD: {    //0SSSSSS
                  if (LnPacketData[1] < MaxSlot) {
                    if (slot[LnPacketData[1]].LAdr == 0) {//unbekannt!
                      #if not defined(LnSLOTSRV)
                      LNGetSlotInfo(LnPacketData[1]); //request data
                      #endif
                      break;
                    }
                    #if defined(LnDEB)
                    Debug.print(slot[LnPacketData[1]].LAdr);
                    Debug.print(" OPC_LOCO_SPD: ");
                    Debug.println((dcc.getLocoDir(slot[LnPacketData[1]].LAdr) << 7) | LnPacketData[2], BIN);
                    #endif
                    if ((slot[LnPacketData[1]].Status & B111) == LNLOCO14)
                      dcc.setSpeed14(slot[LnPacketData[1]].LAdr, (dcc.getLocoDir(slot[LnPacketData[1]].LAdr) << 7) | map(LnPacketData[2],0,128,0,14));
                    else {
                      if ((slot[LnPacketData[1]].Status & B111) == LNLOCO28)
                        dcc.setSpeed28(slot[LnPacketData[1]].LAdr, (dcc.getLocoDir(slot[LnPacketData[1]].LAdr) << 7) | map(LnPacketData[2],0,128,0,28));
                      else dcc.setSpeed128(slot[LnPacketData[1]].LAdr, (dcc.getLocoDir(slot[LnPacketData[1]].LAdr) << 7) | LnPacketData[2]);
                    }
  
                    #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                    z21.setLocoStateExt (slot[LnPacketData[1]].LAdr);
                    LnZ21bcType = Z21bcLocoNetLocos_s;           
                    #endif
                    #if defined(XPRESSNET)
                    XpressNet.setSpeed(slot[LnPacketData[1]].LAdr, 128, (dcc.getLocoDir(slot[LnPacketData[1]].LAdr) << 7) | LnPacketData[2]);
                    XpressNet.ReqLocoBusy(slot[LnPacketData[1]].LAdr);   //Lok wird nicht von LokMaus gesteuert!
                    #endif
                  }
                  break;    
        }  
        case OPC_LOCO_DIRF: { //0,0,DIR,F0,F4,F3,F2,F1
                  if (LnPacketData[1] < MaxSlot) {
                    if (slot[LnPacketData[1]].LAdr == 0) {//unbekannt!
                      #if not defined(LnSLOTSRV)
                      LNGetSlotInfo(LnPacketData[1]); //request data
                      #endif
                      break;
                    }
                  
                    #if defined(LnInvDir)
                    LnPacketData[2] = ((~LnPacketData[2]) & 0x20) | (LnPacketData[2] & 0x1F);
                    #endif
                    
                    #if defined(LnDEB)
                    Debug.print(slot[LnPacketData[1]].LAdr);
                    Debug.print(" OPC_LOCO_DIRF: ");
                    Debug.println(LnPacketData[2], BIN);
                    #endif
                    byte lokspeed = dcc.getLocoSpeed(slot[LnPacketData[1]].LAdr);  //lese aktuelle Geschwindigkeit
                    bitWrite(lokspeed, 7, ((LnPacketData[2] >> 5) & 0x01));  //Fahrrichtung
  
                    if ((slot[LnPacketData[1]].Status & B111) == LNLOCO14)
                    dcc.setSpeed14(slot[LnPacketData[1]].LAdr, lokspeed );  //update DIRF in DCC library
                    else { 
                      if ((slot[LnPacketData[1]].Status & B111) == LNLOCO28)
                         dcc.setSpeed28(slot[LnPacketData[1]].LAdr, lokspeed );  //update DIRF in DCC library 
                      else dcc.setSpeed128(slot[LnPacketData[1]].LAdr, lokspeed );  //update DIRF in DCC library
                    }
                    dcc.setFunctions0to4(slot[LnPacketData[1]].LAdr, LnPacketData[2] & B00011111);	//- F0 F4 F3 F2 F1
  
                    #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                    z21.setLocoStateExt (slot[LnPacketData[1]].LAdr);
                    LnZ21bcType = Z21bcLocoNetLocos_s;           
                    #endif
                    
                    #if defined(XPRESSNET)
                    XpressNet.setFunc0to4(slot[LnPacketData[1]].LAdr, LnPacketData[2] & B00011111);
                    XpressNet.ReqLocoBusy(slot[LnPacketData[1]].LAdr);   //Lok wird nicht von LokMaus gesteuert!
                    #endif
                  }
                  break;
        }
        case OPC_LOCO_SND: { //0,0,0,0,F8,F7,F6,F5 
                  if (LnPacketData[1] < MaxSlot) {
                    if (slot[LnPacketData[1]].LAdr == 0) {//unbekannt!
                      #if not defined(LnSLOTSRV)
                      LNGetSlotInfo(LnPacketData[1]); //request data
                      #endif
                      break;
                    }
  
                    #if defined(LnDEB)
                    Debug.print(slot[LnPacketData[1]].LAdr);
                    Debug.print(" OPC_LOCO_SND: ");
                    Debug.println(LnPacketData[2], BIN);
                    #endif
                    
                    dcc.setFunctions5to8(slot[LnPacketData[1]].LAdr, LnPacketData[2]);	//- F8 F7 F6 F5
                    
                    #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                    z21.setLocoStateExt (slot[LnPacketData[1]].LAdr);
                    LnZ21bcType = Z21bcLocoNetLocos_s;           
                    #endif
                    
                    #if defined(XPRESSNET)
                    XpressNet.setFunc5to8(slot[LnPacketData[1]].LAdr, LnPacketData[2]);
                    XpressNet.ReqLocoBusy(slot[LnPacketData[1]].LAdr);   //Lok wird nicht von LokMaus gesteuert!
                    #endif
                  }
                  break; 
        }
        case OPC_IMM_PACKET: { //Digitrax OPC_LOCO_F912 = Functions 9-12
                  LnZ21bcType = Z21bcLocoNetLocos_s;           
                  #if defined(LnSLOTSRV) 
                  //unkommend to make Daisy2 work!
                  //byte ACK[] = {OPC_LONG_ACK, OPC_IMM_PACKET & 0x7F, 0x00, 0x00};  //busy
                  //LNSendPacket (ACK, 4, LnZ21bcType, true);    //Send ACK 
                  #endif
                  break;
        }
        case 0xA3: {  //0,0,0,0,F12,F11,F10,F9 by Uhlenbrock
                  if (LnPacketData[1] < MaxSlot) {
                    if (slot[LnPacketData[1]].LAdr == 0) {//unbekannt!
                      #if not defined(LnSLOTSRV)
                      LNGetSlotInfo(LnPacketData[1]); //request data
                      #endif
                      break;
                    }
                    dcc.setFunctions9to12(slot[LnPacketData[1]].LAdr, LnPacketData[2]);  //- F12 F11 F10 F9
                    
                    #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                    z21.setLocoStateExt (slot[LnPacketData[1]].LAdr);
                    LnZ21bcType = Z21bcLocoNetLocos_s;
                    #endif
                    
                    #if defined(XPRESSNET)
                    XpressNet.setFunc9to12(slot[LnPacketData[1]].LAdr, LnPacketData[2]);
                    XpressNet.ReqLocoBusy(slot[LnPacketData[1]].LAdr);   //Lok wird nicht von LokMaus gesteuert!
                    #endif
                  }
                  break; 
        }
        case OPC_UHLI_FUN: {  //Function 9-28 by Uhlenbrock (0xD4)
                  if (LnPacketData[2] >= MaxSlot) 
                    break;
                  if (LnPacketData[1] == 0x20) {
                    if (slot[LnPacketData[2]].LAdr == 0) { //unbekannt Slot in 3. Byte!
                      #if not defined(LnSLOTSRV)    
                      LNGetSlotInfo(LnPacketData[2]);   //request data
                      #endif
                      break;
                    }
                    #if defined(LnDEB)
                    Debug.print("OPC_UHLI_FUN: ");
                    #endif
                    byte Func = 0x00;
                    if (LnPacketData[3] == 0x07) { //Arg3
                      Func = ((LnPacketData[4] >> 4) & 0x07) | (dcc.getFunktion9to12(slot[LnPacketData[2]].LAdr) & 0x08);
                      #if defined(LnDEB)
                      Debug.print("9-12:");
                      Debug.println(Func, BIN);
                      #endif  
                      dcc.setFunctions9to12(slot[LnPacketData[2]].LAdr, Func);	//- F12 F11 F10 F9
                      #if defined(XPRESSNET)
                      XpressNet.setFunc9to12(slot[LnPacketData[2]].LAdr, Func);
                      #endif
                    }
                    if (LnPacketData[3] == 0x08) {  //for F13 to F19
                      Func = (LnPacketData[4] & 0x7f) | (dcc.getFunktion13to20(slot[LnPacketData[2]].LAdr) & 0x80);
                      #if defined(LnDEB)
                      Debug.print("13-20:");
                      Debug.println(Func, BIN);
                      #endif  
                      dcc.setFunctions13to20(slot[LnPacketData[2]].LAdr, Func);  //F20 to F13
                      #if defined(XPRESSNET)
                      XpressNet.setFunc13to20(slot[LnPacketData[2]].LAdr, Func);
                      #endif
                    }
                    if (LnPacketData[3] == 0x09) { //for F21 to F27
                      Func = (LnPacketData[4] & 0x7f) | (dcc.getFunktion21to28(slot[LnPacketData[2]].LAdr) & 0x80);
                      #if defined(LnDEB)
                      Debug.print("21-27:");
                      Debug.println(Func, BIN);
                      #endif
                      dcc.setFunctions21to28(slot[LnPacketData[2]].LAdr, Func);  //F28 to F21
                      #if defined(XPRESSNET)
                      XpressNet.setFunc21to28(slot[LnPacketData[2]].LAdr, Func);
                      #endif
                    }
                    if (LnPacketData[3] == 0x05) { //for F12, F20 and F28
                      Func = ((LnPacketData[4] >> 1) & 0x08) | (dcc.getFunktion9to12(slot[LnPacketData[2]].LAdr) & 0x07);   //add F12
                      #if defined(LnDEB)
                      Debug.print("9-12:");
                      Debug.print(Func, BIN);
                      #endif
                      dcc.setFunctions9to12(slot[LnPacketData[2]].LAdr, Func);  //F9 to F12
                      Func = ((LnPacketData[4] << 2) & 0x80) | (dcc.getFunktion13to20(slot[LnPacketData[2]].LAdr) & 0x7f);  //add F20
                      #if defined(LnDEB)
                      Debug.print("; 13-20:");
                      Debug.print(Func, BIN);
                      #endif
                      dcc.setFunctions13to20(slot[LnPacketData[2]].LAdr, Func);  //F20 to F13
                      #if defined(XPRESSNET)
                      XpressNet.setFunc13to20(slot[LnPacketData[2]].LAdr, Func);
                      #endif
                      Func = ((LnPacketData[4] << 1) & 0x80) | (dcc.getFunktion21to28(slot[LnPacketData[2]].LAdr) & 0x7f);  //add F28
                      #if defined(LnDEB)
                      Debug.print("; 21-28:");
                      Debug.println(Func, BIN);
                      #endif
                      dcc.setFunctions21to28(slot[LnPacketData[2]].LAdr, Func);  //F28 to F21
                      #if defined(XPRESSNET)
                      XpressNet.setFunc21to28(slot[LnPacketData[2]].LAdr, Func);
                      #endif
                    }
                    #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                    z21.setLocoStateExt (slot[LnPacketData[2]].LAdr);
                    LnZ21bcType = Z21bcLocoNetLocos_s;
                    #endif

                    #if defined(XPRESSNET)
                    XpressNet.ReqLocoBusy(slot[LnPacketData[2]].LAdr);   //Lok wird nicht von LokMaus gesteuert!
                    #endif
                  }
                  break;    
        }
        case OPC_SW_STATE: {  //Request state of switch. 
                  //dcc.getBasicAccessoryInfo(Address+inc)
                  #if defined(LnSLOTSRV)
                  //byte LOPC = LnPacketData[0] & 0x7F;  //Kopie der Kommando-Codes. Das 7. Bit wird 0 gesetzt 
                  byte ACK[] = {OPC_LONG_ACK, 0x00, 0x00, 0x00};  //Fail!!
                  ACK[1] = LnPacketData[0] & B01111111;
                  LNSendPacket (ACK, 4, Z21bcLocoNetSwitches_s, true);    //Send ACK
                  #endif
                  break;
        }
        case OPC_SW_ACK: { //Request switch with acknoledge function.
                  dcc.setBasicAccessoryPos(LnPacketData[1] | ((LnPacketData[2] & 0x0F) << 7),(LnPacketData[2] >> 5) & 0x01, (LnPacketData[2] >> 4) & 0x01);  //Adr, State, on/off
                  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                  LnZ21bcType = Z21bcLocoNetSwitches_s;
                  #endif
                  #if defined(LnSLOTSRV)
                  //byte LOPC = LnPacketData[0] & B01111111;  //Kopie der Kommando-Codes. Das 7. Bit wird 0 gesetzt 
                  byte ACK[] = {OPC_LONG_ACK, 0x00, 0x7F, 0x00};  //Succsess
                  ACK[1] = LnPacketData[0] & 0x7F;
                  LNSendPacket (ACK, 4, LnZ21bcType, true);    //Send ACK
                  #endif
                  break;
        }        
        case OPC_SW_REQ: { //Request switch function 
                  uint16_t Adr = (LnPacketData[1] | ((LnPacketData[2] & 0x0F) << 7));
                  dcc.setBasicAccessoryPos(Adr,(LnPacketData[2] >> 5) & 0x01, (LnPacketData[2] >> 4) & 0x01);  //Adr, State, on/off
                  LnZ21bcType = Z21bcLocoNetSwitches_s;
                  #if defined(LnDEB)
                  Debug.print(F("OPC_SW_REQ:"));
                  Debug.print(Adr);
                  Debug.print(((LnPacketData[2] >> 5) & 0x01) ? "=left" : "=right");
                  Debug.println(((LnPacketData[2] >> 4) & 0x01) ? "=on" : "=off");
                  #endif                
                  break;
        }
        case OPC_SW_REP: { //Turnout sensor state report 
                  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                  //LnPacketData[1] = 0,A6,A5,A4,A3,A2,A1,A0 
                  //LnPacketData[2] =   0,X,I,L,A10,A9,A8,A7
                  byte Rdata[4];
                  uint16_t Adr = (LnPacketData[1] | ((LnPacketData[2] & 0x0F) << 7)) + 1;
                  Rdata[0] = 0x01; //Typ
                  Rdata[1] = lowByte(Adr);  //A7-A0
                  Rdata[2] = highByte(Adr);  //A10-A8
                  Rdata[3] = (LnPacketData[2] >> 5) & 0x01; //L, Rückmelde-Zustand
                  z21.setLNDetector(0, Rdata, 4);
                  
                  #if defined(LnDEB)
                  Debug.print(F("OPC_SW_REP:"));
                  Debug.print(Adr);
                  Debug.println(((LnPacketData[2] >> 5) & 0x01) ? "=on" : "=off");
                  #endif
                  
                  #endif
                  break;         
        }
        case OPC_INPUT_REP: { //0xB2 = Besetztmelder - LAN_LOCONET_DETECTOR
                  //LnPacketData[1] = 0,A6,A5,A4,A3,A2,A1,A0 
                  //LnPacketData[2] = 	0,X,I,L,A10,A9,A8,A7
                  byte Rdata[4];
                  uint16_t Adr = (LnPacketData[1] | ((LnPacketData[2] & 0x0F) << 7));
                  Adr <<= 1;
                  Adr += (LnPacketData[2] & OPC_INPUT_REP_SW) ? 2 : 1;
                  Rdata[0] = 0x01; //Typ
                  Rdata[1] = lowByte(Adr);  //A7-A0
                  Rdata[2] = highByte(Adr);  //A10-A8
                  Rdata[3] = (LnPacketData[2] >> 4) & 0x01; //L, Rückmelde-Zustand
                  
                  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                  z21.setLNDetector(0, Rdata, 4);
                  #endif
                  
                  #if defined(REPORT)
                  Debug.print(F("LN Sensor:"));
                  Debug.print(Adr);
                  Debug.println(((LnPacketData[2] >> 4) & 0x01) ? "=on" : "=off");
                  #endif
                  
                  break; }
        case OPC_MULTI_SENSE: {
                  byte Rdata[4];
                  Rdata[0] = LnPacketData[1]; //Type
                  Rdata[1] = LnPacketData[4]; //Adr
                  Rdata[2] = LnPacketData[3]; //Adr
                  Rdata[3] = LnPacketData[2]; //zone and section
                  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
                  z21.setLNDetector(0, Rdata, 4); 
                  #endif
                  break; }
        case OPC_PEER_XFER: {   //0xE5
                  #if defined(LnDEB)
                  Debug.println(F("OPC_PEER_XFER"));
                  #endif  
                  break; }
        case 0xE6: { //OPC_ALM_READ  // Undocumented name
                  #if defined(LnDEB)
                  Debug.println(F("OPC_ALM_READ"));
                  #endif
        break;}
        case 0xEE: { //OPC_ALM_WRITE // Undocumented name
                  #if defined(LnDEB)
                  Debug.println(F("OPC_ALM_WRITE"));
                  #endif
        break;}
        //Zustand Gleisspannung
        case OPC_GPOFF: {
                        if (Railpower != csTrackVoltageOff)
                          globalPower(csTrackVoltageOff); 
                        break; }
        case OPC_GPON:  {
                        if (Railpower != csNormal) 
                           globalPower(csNormal); 
                        break; }
        case OPC_IDLE:  {
                        if (Railpower != csEmergencyStop) 
                          globalPower(csEmergencyStop); 
                        break; }
      }

   #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
   if (Z21Report) {
      //Report the RX-LocoNet Frame to z21:
      #if defined(ESP32_MCU)
      z21.setLNMessage(LnPacketData, LNDataLength, LnZ21bcType, false);   //RX Packet 
      #else
      z21.setLNMessage(LnPacketData, getLnMsgSize(LnPacket), LnZ21bcType, false);   //RX Packet 
      #endif
   }
   #endif
}

//--------------------------------------------------------------------------------------------
//LocoNet update via each loop
void LNupdate() {
  // Check for any received LocoNet packets
  if (LNgetNext == false) {
    #if defined(ESP32_MCU)
    //Receive a Packet via LocoNet2 call back function!!!!
    //for (byte i = 0; i < LNDataLength; i++) {
     // LnPacketData = LNData;
    //}
    #else
    LnPacket = LocoNet.receive();
    #endif
  }
  #if !defined(ESP32_MCU)
  if( LnPacket ) {
  #endif  
    LNgetNext = false;
    #if defined(LnDEB)
      Debug.print("Ln: ");
      LNDebugPrint();
    #endif
    
    LNdecode(true); //verarbeite empfangene Daten!

    #if defined(LnBufferUSB)
    ReportLnBuffer();  
    #endif
    
  #if !defined(ESP32_MCU)   
  }    //end if (LnPacket)
  #endif

  #if defined(LnBufferUSB)
    // Check to see if there are any bytes from the PC
    if(int charWaiting = Serial.available()) {
      #if defined(LNBufferUSB_CTS)
      // If the number of bytes waiting is less than RX_BUF_LOW enable CTS
      if( charWaiting < RX_BUF_LOW )
        digitalWrite(Ln_RX_CTS_PIN,LOW);
      // If the number of bytes waiting is more than RX_BUF_HIGH disable CTS
      else if( charWaiting > RX_BUF_HIGH )
        digitalWrite(Ln_RX_CTS_PIN,HIGH);
      #endif
      
      // Read the byte
      uint8_t inByte = Serial.read() & 0xFF;
    
      // Add it to the buffer
      addByteLnBuf( &LnTxBuffer, inByte ) ;
    
      // Check to see if we have received a complete packet yet
      LnPacket = recvLnMsg( &LnTxBuffer ) ;
      if(LnPacket ) { // Send the received packet from the PC to the LocoNet
        LNSendPacket (LnPacket->data, getLnMsgSize( LnPacket ), Z21bcLocoNet_s, false);   //Report nicht an Z21 Library!
        LNdecode(false); //verarbeite empfangene Daten, aber nicht als Z21 RX! 
      }
    }
  #endif
}
//--------------------
#endif
