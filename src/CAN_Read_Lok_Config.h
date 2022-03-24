/*
 Read the Loc configuration from as Maerklin MS2

 ACHTUNG: Eine Debug Ausgabe dauert mindestens 3.2ms unabhängig von ihrer
          Länge und der Baudrate (siehe "Untersuchung Debug Ausgaben.docx").
          Während die Konfigurationsdaten gelesen werden dürfen keine seriellen
          Ausgaben gemacht werden sonst gehen CAN Messages verloren da diese
          im Abstand von 0.5 ms kommen!
          Es kann auch passieren, dass die Reihenfolge der CAN Messages
          durcheinander gerät. Das liegt daran, dass der MCP2515 zwei
          Empfangspuffer hat. Die Auslese Funktion liest immer den ersten
          Puffer zuerst aus.

 In den Dateien
   MS2:  "Lokabfrage per Progamm an MS2.xlsx"
   CS3:  "Lokabfrage per Progamm an CS3 (loknamen 0 10).xlsx"
 findet man Aufzeichnungen der Abfragen.


 Berechnung der Adressen:
 ~~~~~~~~~~~~~~~~~~~~~~~~
 Siehe "1.3.1.2  Einbindung bestehender Gleisprotokolle, Bildung der 'Loc-ID'" in "cs2CAN-Protokoll-2_0.pdf"

 DCC Adressen haben die UID 0xC000 + DCC Adresse
 MFX beginnen bei 0x4000

 So wie es aussieht werden die uid's der MFX Lokomotiven irgend wie hochgezählt
 Das passiert aber nicht in der Reihenfolge wie sie erkannt wurden.
 Die Chipkarten wurden alphabetisch eingelesen.
 Warum aber die Lücke zwischen 4001 und 4005? Genau 3 Adressen werden nicht benutzt?

 Ausgelesen:
 Name               uid     uid Dez Adr uid - Adr Adr Hex Typ
 ------------------ ---     ------- --- --------- ------- ---
 Rocksy             4001    16385   4     16381   0004
 ICE                4005    16389   79    16310   004F
 Kroko              4006    16390   70    16320   0046
 Browny             4007    16391   112   16279   0070
 Cora               4008    16392   42    16350   002A
 Kittel Dampftrie   4009    16393   41    16352   0029
 Lolo               400A    16394   76    16318   004C
 Helsinki           400B    16395   18    16377   0012
 Molly              400C    16396   62    16334   003E
 Blackfoot          400D    16397   50    16347   0032
 Rotfuchs           400E    16398   74    16324   004A
 Back Pearl         400F    16399   111   16288   006F
 Susi               4010    16400   61    16339   003D
 Murnau             4011    16401   109   16292   006D
 Blaufuchs          4012    16402   5     16397   0005
 Smokie             4013    16403   24    16379   0018
 Onkel Bert         4014    16404   53    16351   0035
 Hondekop           4015    16405   3     16402   0003
 Tante Olga         4016    16406   18    16388   0012
 Rolf               C049    49225   73    49152   0049    DCC
 Wendy              C050    49232   80    49152   0050    DCC
 Glaskasten         C06C    49260   108   49152   006C    DCC
 Messwagen          C259    49753   601   49152   0259    DCC

 Eine Berrechnung der Adressen bringt auch nichts wenn einige der
 Lokomotiven nicht als DCC Lok Konfiguriert sind. In der

 Hier habe ich mal zusätzlich die mfxuid ausgelesen:

      Adr:  50 uid: 400D mfxuid: FFBFF806 4290770950 Blackfoot
      Adr: 111 uid: 400F mfxuid: F9F75D2F 4193738031 Back Pearl
      Adr:   5 uid: 4012 mfxuid: F9F74093 4193730707 Blaufuchs
      Adr: 112 uid: 4007 mfxuid: FFC78E76 4291268214 Browny
      Adr:  42 uid: 4008 mfxuid: F9F6AF57 4193693527 Cora
      Adr: 108 uid: C06C mfxuid: 00000001          1 Glaskasten
      Adr:  18 uid: 400B mfxuid: FFBFF686 4290770566 Helsinki
      Adr:   3 uid: 4015 mfxuid: F9F763C3 4193739715 Hondekop
      Adr:  79 uid: 4005 mfxuid: F9F623E7 4193657831 ICE
      Adr:  41 uid: 4009 mfxuid: F9FF71D1 4194267601 Kittel Dampftrie
      Adr:  70 uid: 4006 mfxuid: FFD5ACF3 4292193523 Kroko
      Adr:  76 uid: 400A mfxuid: F9F740C4 4193730756 Lolo
      Adr: 601 uid: C259 mfxuid: 00000001          1 Messwagen
      Adr:  62 uid: 400C mfxuid: FFCF553F 4291777855 Molly
      Adr: 109 uid: 4011 mfxuid: F9FC5E1C 4194065948 Murnau
      Adr:  53 uid: 4014 mfxuid: F9F623D2 4193657810 Onkel Bert
      Adr:   4 uid: 4001 mfxuid: FFC124C5 4290847941 Rocksy
      Adr:  73 uid: C049 mfxuid: 00000001          1 Rolf
      Adr:  74 uid: 400E mfxuid: F9FF9AA7 4194278055 Rotfuchs
      Adr:  24 uid: 4013 mfxuid: F9F6AF63 4193693539 Smokie
      Adr:  61 uid: 4010 mfxuid: FFC122AD 4290847405 Susi
      Adr:  18 uid: 4016 mfxuid: F9F6AFBB 4193693627 Tante Olga
      Adr:  80 uid: C050 mfxuid: 00000001          1 Wendy

 Der verknüpfung zwischen uid mfxuid wird mit dem Befehl: MFX Bind (0x04) hergestellt:
      00044F70 -  6    F9 F7 40 93 00 12   mfxuid: F9F74093 4193730707 Blaufuchs
      00044F70 -  6    F9 F6 AF 57 00 08   mfxuid: F9F6AF57 4193693527 Cora
      00044F70 -  6    F9 F6 AF 63 00 13   mfxuid: F9F6AF63 4193693539 Smokie
        ~~                         ~~~~~
        MFX Bind                   Zugewiesene uid + 0x4000 = uid

 Die mfxuid ist eine eindeutige Seriennummer einer mfx Lok. Über den Befehl: MFX Bind(0x04) wird der Lok eine
 MFX-SID zugeordnet. Diese Nummer + 0x4000 bildet die uid über welche die mfx Lok angesprochen wird.

 Ich verstehe immer noch nicht wie die Zuordnung gemacht wird und warum wieder die gleiche SID zugeordnet wird.

 Man könnte den MFX Bind Befehl abhören und damit sicherstellen, das die richtige UID einer Lok verwendet wird.

 Wie Adressiert man die MFX Lokomotiven von der Z21 App aus?
 Die DCC Adresse geht eigentlich nicht. Sie wird im MFX Betrieb nicht benutzt. Es können mehrer Lokomotiven
 die gleiche DCC Adresse haben. Ohne Programmiergerät kann die DCC Adresse der Lok nur umständlich verändern.
 Am besten wäre es wenn man die SID in der Z21 App verwendet. Vielleicht ermöglicht man beides.
 Die SID wird einfach nur hochgezählt => Mehr als 1000 Nummern wird man nicht haben. Man könnte ab 8000
 wahlweise

 Test mit leerer MS2. Die Lokomotiven wurden nacueinander auf das kurze Gleis gesetzt.
  Test 1:
  - "Glaskasten" (DCC) Manuell erkannt
  - Rotfuchs automatisch erkannt
  - Molly automatisch erkannt:
      Adr: 108 uid: C06C mfxuid: 00000001          1 DCC-108
      Adr:  74 uid: 4005 mfxuid: F9FF9AA7 4194278055 Rotfuchs
      Adr:  62 uid: 4006 mfxuid: FFCF553F 4291777855 BR 99.61 (Molly)

  Test 2:
  - Werkseinstellungen
  - Lolo automatisch erkannt
      Adr:  76 uid: 4005 mfxuid: F9F740C4 4193730756 BR 78 / pr. T18  (Lolo)
  - Rotfuchs automatisch erkannt
      Adr:  76 uid: 4005 mfxuid: F9F740C4 4193730756 BR 78 / pr. T18  (Lolo)
      Adr:  74 uid: 4006 mfxuid: F9FF9AA7 4194278055 Rotfuchs
    Er hat jetzt die 4006 bekommen
  - Molly automatisch erkannt
      Adr:  76 uid: 4005 mfxuid: F9F740C4 4193730756 BR 78 / pr. T18  (Lolo)
      Adr:  74 uid: 4006 mfxuid: F9FF9AA7 4194278055 Rotfuchs
      Adr:  62 uid: 4007 mfxuid: FFCF553F 4291777855 BR 99.61         (Molly)

  - Abschalten der Versorgungsspannung
      Adr:  76 uid: 4005 mfxuid: F9F740C4 4193730756 BR 78 / pr. T18
      Adr:  74 uid: 4006 mfxuid: F9FF9AA7 4194278055 Rotfuchs
      Adr:  62 uid: 4007 mfxuid: FFCF553F 4291777855 BR 99.61
    Die Adressen und Reihenfolge bleibt erhalten
  - Blaufuchs über Karte eingelesen
      Adr:  76 uid: 4005 mfxuid: F9F740C4 4193730756 BR 78 / pr. T18
      Adr:  74 uid: 4006 mfxuid: F9FF9AA7 4194278055 Rotfuchs
      Adr:  62 uid: 4007 mfxuid: FFCF553F 4291777855 BR 99.61
      Adr:   5 uid: 4012 mfxuid: F9F74093 4193730707 Blaufuchs
    Er bekommt die gleiche SID wie auf der Anlage
  - ICE Über Karte eingelesen
      Adr:  76 uid: 4005 mfxuid: F9F740C4 4193730756 BR 78 / pr. T18
      Adr:  74 uid: 4006 mfxuid: F9FF9AA7 4194278055 Rotfuchs
      Adr:  62 uid: 4007 mfxuid: FFCF553F 4291777855 BR 99.61
      Adr:   5 uid: 4012 mfxuid: F9F74093 4193730707 Blaufuchs
      Adr:  79 uid: 4008 mfxuid: F9F623E7 4193657831 ICE
    Da die SID 4005 welche er auf der Anlage hat in dem Test bereits belegt ist
   bekommt er eine neu SID zugewiesen
      Das waren die Daten von der Anlage: Adr:  79 uid: 4005 mfxuid: F9F623E7 4193657831 ICE

   Test 3:
   - Werkseinstellungen
   - ICE
      Adr:  79 uid: 4007 mfxuid: F9F623E7 4193657831 ICE
  - Molly automatisch erkannt
      Adr:  79 uid: 4007 mfxuid: F9F623E7 4193657831 ICE
      Adr:  62 uid: 4005 mfxuid: FFCF553F 4291777855 BR 99.61 (Molly)
    Sie hat die SID von Molly bekommen
  - Lolo automatisch erkannt
      Adr:  79 uid: 4007 mfxuid: F9F623E7 4193657831 ICE
      Adr:  62 uid: 4005 mfxuid: FFCF553F 4291777855 BR 99.61
      Adr:  76 uid: 4006 mfxuid: F9F740C4 4193730756 BR 78 / pr. T18
    Auch sie bekommt eine andere Nummer
  - Rocksy eingesteckt (Früher SID 4001), aber die Karte wird nicht richtig akzeptiert
    Die Lok wird angezeigt, verschwindet aber wenn man auf Shift drückt
  - Abschalten der Versorgungsspannung
  - Rocksy eingesteckt => Jetzt bleibt sie erhalten
    Und taucht auch in der Liste auf:
      Adr:  79 uid: 4007 mfxuid: F9F623E7 4193657831 ICE
      Adr:  62 uid: 4005 mfxuid: FFCF553F 4291777855 BR 99.61
      Adr:  76 uid: 4006 mfxuid: F9F740C4 4193730756 BR 78 / pr. T18
      Adr:   4 uid: 4001 mfxuid: FFC124C5 4290847941 Rocksy
    Sie bekommt die gleiche Nummer wie auf der Anlage

 */

#if defined(USE_CAN)

#include <string>
#include "MS2_EEPROM_Adr.h"


#define CRC_RETRYS             3   // Number of retries in case of an CRC error

#define LOK_NAME_LEN           16  // Laenge der Namen in der MS2. In der CS2/3 können die namen belibig lang sein
#define LOK_NAME_LEN_STR      "16" // Beim Anfragen über den CAN werden aber nur die ersten 16 Zeichen berücksichtigt

#define TIMEOUT_CONF_LINE      800 // [ms]    Die 3. CS2 laesst sich manchmal sehr viel Zeit mit der Antwort 525ms
                                   //         Die anderen sind viel schneller (max 50 ms)
                                   //         Sie hat die Hardware Version 2.0, die anderen die 2.1

#define MAX_BOOTTIME_CENTRAL (5*60)// [sec] Maximal boottime of the CS2. The program waits for this long to read the coco config

#define CREATE_NOT_FOND_LOCOS  1   // Create locos if they are receifed via CAN or LAN (Z21 App)

bool Create_CSV = 1;

// Forward definitions
void Print_Config_File_ASCII_or_HEX(byte* &Buffer, uint32_t size);
int8_t Read_Config_File_from_CAN(CAN_CLASS &can, const char *ConfName, byte* &Buffer, uint32_t &size, unsigned long AnswId);
const char *Get_Central_Typ_Str();
uint8_t Get_Central(CAN_CLASS *can);


#if defined(__arm__)
      #include <DueFlashStorage.h>
      DueFlashStorage FlashStore;
      #define FSTORAGE    FlashStore
      #define FSTORAGEMODE write

#elif defined(ESP32)  //use NVS on ESP32!
//      #include "z21nvs.h"
//      z21nvsClass NVSZ21;
//      #define FSTORAGE NVSZ21
//      #define FSTORAGEMODE write
//
      #include <esp_task_wdt.h>
      #ifndef WDT_RESET
        #define WDT_RESET()  esp_task_wdt_reset()
      #endif
#else
     // AVR based Boards follows
     #include <EEPROM.h>
     #define FIXSTORAGE    EEPROM
     #if defined(ESP8266) || defined(ESP32) //ESP8266 or ESP32
         #define FIXMODE write
     #else
         #define FIXMODE update
     #endif
     #if defined(ESP8266)
       #ifndef WDT_RESET
         #define WDT_RESET()  wdt_reset()
       #endif
     #endif
#endif


typedef struct
  {
  //uint16_t uid;                // 2     // 0: Undefined, 1-0x03FF=MM, 0x0800-0x0BFF=?,  0x4000-0x7FFF: MFX, 0xC000-0xFFFF:DCC
  //uint16_t Adresse;            // 2
  uint16_t Z21Adr;             // 2
  uint16_t DCC;                // 2
  char     Name[LOK_NAME_LEN]; // 16
  uint8_t  FktTyp[32];         // 32
  } Const_Lok_Data_t;          // 52 (20) bytes

#if !defined(ESP8266) && !defined(ESP32)
    #error "This programm could only be compiled for an ESP because it needs a lot of RAM"
#endif

typedef struct
  {
  uint16_t  Lok_Cnt;
  Const_Lok_Data_t Const_Lok_Data[MAX_LOK_DATA];
  } All_EEPROM_Data_t;

All_EEPROM_Data_t All_EEPROM_Data;

#include "EEPROM_Read_Write.h"

//------------------------------
void ContProc_Delay(uint32_t ms)
//------------------------------
{
  uint32_t e = micros() + (uint32_t)1000 * ms;
  while (micros() < e)
    {
    //ContProc();
    WDT_RESET();
    }
}

//----------------------
void Toggle_Status_LED()
//----------------------
{
  #if defined(DCCLed)
    digitalWrite(DCCLed, !digitalRead(DCCLed));
  #endif
}

//-----------------------------
void Set_Status_LED(uint8_t On)
//-----------------------------
{
  #if defined(DCCLed)
    digitalWrite(DCCLed, On);
  #endif
}

//----------------------------------
uint16_t uid_to_Z21Adr(uint16_t uid)
//----------------------------------
{
  if (     uid <= 255)                                                    return uid          + Z21_ADR_MM_START;   // +8000
  else if (uid >= 0x4000 && uid < 0x4000 + (10000 - Z21_ADR_MFX_START))   return uid - 0x4000 + Z21_ADR_MFX_START;  // +8256
  else if (uid >= 0xC000 && uid < 0xC000 + Z21_ADR_MM_START)              return uid - 0xC000;

  Dprintf("Error converting uid: %04X to Z21Adr\n", uid);
  return 0;
}

//-------------------------------------
uint16_t Z21Adr_to_uid(uint16_t Z21Adr)
//-------------------------------------
{
  if (     Z21Adr < Z21_ADR_MM_START)  return Z21Adr                     + 0xC000; // DCC
  else if (Z21Adr < Z21_ADR_MFX_START) return Z21Adr - Z21_ADR_MM_START;           // MM
  else                                 return Z21Adr - Z21_ADR_MFX_START + 0x4000; // MFX
}

//------------------------------------
uint16_t Z21Adr_to_DCC(uint16_t Z21Adr)
//------------------------------------
{
  if (     Z21Adr < Z21_ADR_MM_START)  return Z21Adr;                    // DCC
  else if (Z21Adr < Z21_ADR_MFX_START) return Z21Adr - Z21_ADR_MM_START; // MM
  else                                 return 0;                         // MFX (Cant me calculated)
}


uint8_t Add_Detected_Loco_to_EEPROM(uint16_t Z21Adr); // Forward definitioen

//---------------------------------------
uint16_t Find_Index_from_ID(uint16_t uid)
//---------------------------------------
// Find the uid in the EEPROM array Const_Lok_Data[Nr].uid
{
  uint16_t i;
  for (i = 0; i < Lok_Cnt; i++)
      if (Z21Adr_to_uid(Read_Lok_Z21Adr_from_EEPROM(i)) == uid) return i;
  #if CREATE_NOT_FOND_LOCOS
     /*
     if (Z21Adr == 0) // Function called from CAN
        {
        ToDo
        #if EXT_ADR_RANGE_3_TYP != EA_TYP_SID
            #error "ToDo: Not finished EXT_ADR_RANGE_3_TYP mut be equal EA_TYP_SID at the moment"
        #endif
        // 1-0x03FF=MM, 0x0800-0x0BFF=?,  0x4000-0x7FFF: MFX, 0xC000-0xFFFF:DCC
        if (uid <= 0x3FF)                        Z21Adr = uid + 9000;                         // MM
        else if (uid >= 0x4000 && uid <= 0x7FFF) Z21Adr = uid - 0x4000 + EXT_ADR_RANGE_END_2; // MFX
        else if (uid >= 0xC000)                  Z21Adr = uid - 0xC000 + EXT_ADR_RANGE_END_1; // DCC
        else return 0xFFFF;
        }
     */
     Dprintf("Error: uid %04X not found in Find_Index_from_ID() => Adding the loco\n", uid);
     if (!Add_Detected_Loco_to_EEPROM(uid_to_Z21Adr(uid))) return 0xFFFF;
     return Lok_Cnt-1;
  #endif
  return 0xFFFF; // Not found
}

/*
uint8_t Add_Detected_Loco_to_EEPROM(uint16_t Z21Adr); // Forward definition

//--------------------------------------------------------
uint16_t Find_Index_from_ID(uint16_t Z21Adr, uint16_t uid)
//--------------------------------------------------------
// Find the uid in the EEPROM array Const_Lok_Data[Nr].uid
{
  uint16_t i;
  for (i = 0; i < Lok_Cnt; i++)
      if (Read_Lok_uid_from_EEPROM(i) == uid) return i;
  #if CREATE_NOT_FOND_LOCOS
     if (Z21Adr == 0) // Function called from CAN
        {
        ToDo
        #if EXT_ADR_RANGE_3_TYP != EA_TYP_SID
            #error "ToDo: Not finished EXT_ADR_RANGE_3_TYP mut be equal EA_TYP_SID at the moment"
        #endif
        // 1-0x03FF=MM, 0x0800-0x0BFF=?,  0x4000-0x7FFF: MFX, 0xC000-0xFFFF:DCC
        if (uid <= 0x3FF)                        Z21Adr = uid + 9000;                         // MM
        else if (uid >= 0x4000 && uid <= 0x7FFF) Z21Adr = uid - 0x4000 + EXT_ADR_RANGE_END_2; // MFX
        else if (uid >= 0xC000)                  Z21Adr = uid - 0xC000 + EXT_ADR_RANGE_END_1; // DCC
        else return 0xFFFF;
        }
     Dprintf("Error: uid %04X not found in Find_Index_from_ID() => Adding the loco\n", uid);
     if (!Add_Detected_Loco_to_EEPROM(Z21Adr)) return 0xFFFF;                                                 // 15.03.22:
     return Lok_Cnt-1;
  #endif
  return 0xFFFF; // Not found
}
*/
// ***************************************
#if USE_EXT_ADR_TABLE
// ***************************************
/*
#if EXT_ADR_RANGE_END_1 > EXT_ADR_RANGE_END_2 || EXT_ADR_RANGE_END_2 > EXT_ADR_RANGE_END_3 || EXT_ADR_RANGE_END_3 > 9999
  #error "Error: The EXT_ADR_RANGE_END_* must be in ascending order and < 10000"
#endif

#if EXT_ADR_RANGE_2_TYP == EXT_ADR_RANGE_3_TYP || EXT_ADR_RANGE_2_TYP == EXT_ADR_RANGE_4_TYP || EXT_ADR_RANGE_3_TYP == EXT_ADR_RANGE_4_TYP
  #error "Error: The EXT_ADR_RANGE_*_TYPEs bust be different"
#endif

#define EXT_ADR_RANGE_2  (EXT_ADR_RANGE_END_1)  // Offset for the DCC adress calculation in the range
#define EXT_ADR_RANGE_3  (EXT_ADR_RANGE_END_2)
#define EXT_ADR_RANGE_4  (EXT_ADR_RANGE_END_3)


#if   EA_TYP_DCC == EXT_ADR_RANGE_2_TYP
                                         #define EXT_ADR_OFFS_DCC  EXT_ADR_RANGE_2
#elif EA_TYP_DCC == EXT_ADR_RANGE_3_TYP
                                         #define EXT_ADR_OFFS_DCC  EXT_ADR_RANGE_3
#elif EA_TYP_DCC == EXT_ADR_RANGE_4_TYP
                                         #define EXT_ADR_OFFS_DCC  EXT_ADR_RANGE_4
#else
 #error "ERROR: DCC Range is not defined in EXT_ADR_RANGE_*_TYP"
#endif

#if   EA_TYP_MM  == EXT_ADR_RANGE_2_TYP
                                         #define EXT_ADR_OFFS_MM   EXT_ADR_RANGE_2
#elif EA_TYP_MM  == EXT_ADR_RANGE_3_TYP
                                         #define EXT_ADR_OFFS_MM   EXT_ADR_RANGE_3
#elif EA_TYP_MM  == EXT_ADR_RANGE_4_TYP
                                         #define EXT_ADR_OFFS_MM   EXT_ADR_RANGE_4
#else
 #error "ERROR: MM  Range is not defined in EXT_ADR_RANGE_*_TYP"
#endif

#if   EA_TYP_SID == EXT_ADR_RANGE_2_TYP
                                         #define EXT_ADR_OFFS_SDI  EXT_ADR_RANGE_2
#elif EA_TYP_SID == EXT_ADR_RANGE_3_TYP
                                         #define EXT_ADR_OFFS_SDI  EXT_ADR_RANGE_3
#elif EA_TYP_SID == EXT_ADR_RANGE_4_TYP
                                         #define EXT_ADR_OFFS_SDI  EXT_ADR_RANGE_4
#else
 #error "ERROR: SID Range is not defined in EXT_ADR_RANGE_*_TYP"
#endif


//--------------------------------------------
uint16_t Find_Index_from_Adr_All(uint16_t Adr)
//--------------------------------------------
// Is called if the adress has no special offset
{
  uint16_t i;
  for (i = 0; i < Lok_Cnt; i++)
      {
      //char Name[LOK_NAME_LEN+1]; Dprintf("%2i %-16s %4i\n", i, Read_Lok_Name_from_EEPROM(i, Name), Read_Lok_Adr_from_EEPROM(i)); // Debug
      if (Read_Lok_Adr_from_EEPROM(i) == Adr) return i;
      }
  Dprintf("Find_Index_from_Adr_All(%i)\n", Adr);
  #if CREATE_NOT_FOND_LOCOS
     Dprintf("Error: Adr %i not found in Find_Index_from_Adr_All() => Adding the loco\n", Adr);
     if (!Add_Detected_Loco_to_EEPROM(Adr)) return 0xFFFF;                                                    // 15.03.22:
     return Lok_Cnt-1;
  #else
     return 0xFFFF; // Not found
  #endif
}

//----------------------------------------------------------------------------------------------------------------------------
uint16_t Find_Index_from_Adr_in_uid_Range(uint16_t Z21Adr, uint16_t Adr, uint16_t uid_Start, uint16_t uid_End, uint8_t AddNew)
//----------------------------------------------------------------------------------------------------------------------------
{
  uint16_t i;
  for (i = 0; i < Lok_Cnt; i++)
      {
      //char Name[LOK_NAME_LEN+1]; Dprintf("%2i %-16s %4i\n", i, Read_Lok_Name_from_EEPROM(i, Name), Read_Lok_Adr_from_EEPROM(i)); // Debug
      uint16_t uid = Read_Lok_uid_from_EEPROM(i);
      if (Read_Lok_Adr_from_EEPROM(i) == Adr && uid_Start <= uid && uid <= uid_End) return i;
      }
  #if CREATE_NOT_FOND_LOCOS
    if (AddNew)
         {
         Dprintf("Find_Index_from_Adr_in_uid_Range(%i, %i, %04X, %04X\n", Z21Adr, Adr, uid_Start, uid_End);
         Dprintf("Error: Adr not found in Find_Index_from_Adr_in_uid_Range(Adr:%i, %i - %i) => Adding the loco\n", Adr, uid_Start, uid_End);
         if (!Add_Detected_Loco_to_EEPROM(Z21Adr)) return 0xFFFF;                                                    // 15.03.22:
         return Lok_Cnt-1;
         }
    else return 0xFFFF;
  #else
    return 0xFFFF; // Not Found
  #endif
}

//----------------------------------------
uint16_t Find_Index_from_Adr(uint16_t Adr)
//----------------------------------------
{
  uint16_t Ix;
  if   (   Adr <= EXT_ADR_RANGE_END_1) // Range  1
       {
       #if EXT_ADR_RANGE_1_TYP == EA_TYP_MFX
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr, 0x4000, 0x7FFF, 0);  // First check if there is a MFX Loco
       #elif EXT_ADR_RANGE_1_TYP == EA_TYP_DCC
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr, 0xC000, 0xFFFF, 0);  // First check if there is a DCC Loco
       #else
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr,      0, 0x03FF, 0);  // First check if there is a MM  Loco
       #endif
       if (Ix == 0xFFFF) // Not found?
          Ix = Find_Index_from_Adr_All(Adr);
       }
  else if (Adr <= EXT_ADR_RANGE_END_2) // Range 2
       {
       #if   EXT_ADR_RANGE_2_TYP == EA_TYP_MFX
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr - EXT_ADR_RANGE_2, 0x4000, 0x7FFF, 1);
       #elif EXT_ADR_RANGE_2_TYP == EA_TYP_DCC
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr - EXT_ADR_RANGE_2, 0xC000, 0xFFFF, 1);
       #elif EXT_ADR_RANGE_2_TYP == EA_TYP_MM
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr - EXT_ADR_RANGE_2, 0,      0x03FF, 1);
       #else
                                                 Ix = Find_Index_from_ID(              Adr, Adr - EXT_ADR_RANGE_2 + 0x4000);
       #endif
       }
  else if (Adr <= EXT_ADR_RANGE_END_3) // Range 3
       {
       #if   EXT_ADR_RANGE_3_TYP == EA_TYP_MFX
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr - EXT_ADR_RANGE_3, 0x4000, 0x7FFF, 1);
       #elif EXT_ADR_RANGE_3_TYP == EA_TYP_DCC
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr - EXT_ADR_RANGE_3, 0xC000, 0xFFFF, 1);
       #elif EXT_ADR_RANGE_3_TYP == EA_TYP_MM
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr - EXT_ADR_RANGE_3, 0,      0x03FF, 1);
       #else
          Dprintf("Adr: %i => %i = 0x%04X\n", Adr, Adr - EXT_ADR_RANGE_3 + 0x4000, Adr - EXT_ADR_RANGE_3 + 0x4000);
                                                 Ix = Find_Index_from_ID(              Adr, Adr - EXT_ADR_RANGE_3 + 0x4000);
       #endif
       }
  else {                               // Range 4
       #if   EXT_ADR_RANGE_4_TYP == EA_TYP_MFX
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr - EXT_ADR_RANGE_4, 0x4000, 0x7FFF, 1);
       #elif EXT_ADR_RANGE_4_TYP == EA_TYP_DCC
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr - EXT_ADR_RANGE_4, 0xC000, 0xFFFF, 1);
       #elif EXT_ADR_RANGE_4_TYP == EA_TYP_MM
                                                 Ix = Find_Index_from_Adr_in_uid_Range(Adr, Adr - EXT_ADR_RANGE_4, 0,      0x03FF, 1);
       #else
                                                 Ix = Find_Index_from_ID(              Adr, Adr - EXT_ADR_RANGE_4 + 0x4000);
       #endif
       }

  if (Ix < Lok_Cnt)
       {
       //char Name[LOK_NAME_LEN+1]; Dprintf("Adr:%4i uid:%04X %s\n", Adr, Read_Lok_uid_from_EEPROM(Ix), Read_Lok_Name_from_EEPROM(Ix, Name)); // Debug
       }
  else {
       Dprintf("Loko not found %i\n", Adr);
       // ToDo: Reread Lok List if not listed in Not Found Lokos
       }
  return Ix;
}
// ***************************************
*/
#else  // USE_EXT_ADR_TABLE
/*
//----------------------------------------
uint16_t Find_Index_from_Adr(uint16_t Adr) // Old
//----------------------------------------
{
  uint16_t i;
  for (i = 0; i < Lok_Cnt; i++)
      {
      //char Name[LOK_NAME_LEN+1]; Dprintf("%2i %-16s %4i\n", i, Read_Lok_Name_from_EEPROM(i, Name), Read_Lok_Adr_from_EEPROM(i)); // Debug
      if (Read_Lok_Adr_from_EEPROM(i) == Adr) return i;
      }
  Dprintf("Error: Adr %i not found in Find_Index_from_Adr()\n", Adr);
  return 0xFFFF; // Not found
}
*/
#endif // USE_EXT_ADR_TABLE

//-------------------------------------------
uint16_t Find_Index_from_Adr(uint16_t Z21Adr)
//-------------------------------------------
{
  uint16_t i;
  for (i = 0; i < Lok_Cnt; i++)
      {
      //char Name[LOK_NAME_LEN+1]; Dprintf("%2i %-16s %4i\n", i, Read_Lok_Name_from_EEPROM(i, Name), Read_Lok_Adr_from_EEPROM(i)); // Debug
      if (Read_Lok_Z21Adr_from_EEPROM(i) == Z21Adr) return i;
      }
  #if CREATE_NOT_FOND_LOCOS
    Dprintf("Find_Index_from_Adr(%i)\n", Z21Adr);
    Dprintf("Error: Z21Adr not found in Find_Index_from_Adr_in_uid_Range(%i) => Adding the loco\n", Z21Adr);
    if (!Add_Detected_Loco_to_EEPROM(Z21Adr)) return 0xFFFF;                                                    // 15.03.22:
    return Lok_Cnt-1;
  #else
    Dprintf("Error: Z21Adr %i not found in Find_Index_from_Adr()\n", Z21Adr);
    return 0xFFFF; // Not found
  #endif
}

/*
//---------------------------------
char *Lok_Name_from_ID(uint16_t id)
//---------------------------------
{
  static char Name[LOK_NAME_LEN+1]; // static because the function returns a pointer to the name.
  uint16_t i;                        // ToDo: This function is only used once check if the loco is available => Replace it to save memory
  for (i = 0; i < Lok_Cnt && *Read_Lok_Name_from_EEPROM(i, Name); i++)
      {
      if (Read_Lok_uid_from_EEPROM(i) == id) return Name;
      }
  sprintf(Name, "(%08X)", id);  // The '(' at the begin is used in Program_IR_Lok() to reread the lok names from the MS2
  return Name;
}
*/
//----------------------------------------------------
uint16_t Find_Lokname_in_Lok_Data(const char *Lokname)
//----------------------------------------------------
{
  uint16_t i;
  char Name[LOK_NAME_LEN+1];
  for (i = 0; i < MAX_LOK_DATA && i < Lok_Cnt && *Read_Lok_Name_from_EEPROM(i, Name); i++)
      if (strcmp(Name, Lokname) == 0) return i;

  return 0xFFFF;
}



//----------------------------------------------------------------------------------------------------------
void Set_Message_Filter_to_single_Message(CAN_CLASS *can, uint32_t Mask, uint32_t Filt, uint8_t ClearBuffer)
//----------------------------------------------------------------------------------------------------------
{
  can->init_Mask(0,1,Mask);   // The active bits must match with the following line
  can->init_Filt(0,1,Filt);   // Only this message is passed
  can->init_Filt(1,1,Filt);   // Same mask because no other message is wanted

  can->init_Mask(1,1,Mask);   // The second file uses the same mask
  can->init_Filt(2,1,Filt);   // and filter
  can->init_Filt(3,1,Filt);
  can->init_Filt(4,1,Filt);
  can->init_Filt(5,1,Filt);

  if (ClearBuffer)
     {
     byte rxBuf[8], len = 0;
     unsigned long rxId;
     for (uint8_t i = 0; i < 2; i++) // Try to read two messages to be sure that the filters are empty
         {
         if (can->checkReceive() == CAN_MSGAVAIL)
            can->readMsgBuf(&rxId, &len, rxBuf);
         }
     }
}

//-------------------------------------------------
uint32_t Read_Var_Hex(char* p, const char* VarName)
//-------------------------------------------------
// Read the variable from the config data
{
  unsigned Val = 0; // In case of an error we return 0
  if (p)
     {
     p = strstr(p, VarName);
     if (p)
        {
        p += strlen(VarName);
        char *e = strchr(p, '\n');
        if (e)
           {
           *e = '\0';
           sscanf(p, "%x", &Val);
           *e = '\n'; // Restore to be able to scan the file again in case the sequence of the vatiable is other then expected
           }
        }
     }
  return Val;
}

//--------------------------------------------------
uint16_t Read_Var_Dec(char* &p, const char* VarName)
//--------------------------------------------------
// Read the variable from the config data
// The function is able to read
//  "..typ=" and "..typ2=" if searched for "..typ"
// p is placed to the end of the variable.
// If the variable is not found p is set to 0
{
  uint32_t Val = 0xFFFF; // In case of an error we return 0xFFFF
  if (p)
     {
     p = strstr(p, VarName);
     if (p)
        {
        p += strlen(VarName);
        while (*p != '=') p++;
        p++; // skip also the '='
        char *e = strchr(p, '\n');
        if (e)
           {
           *e = '\0';
           sscanf(p, "%u", &Val);
           *e = '\n'; // Restore to be able to scan the file again in case the sequence of the vatiable is other then expected
           p = e + 1;
           }
        }
     }
  return 0xFFFF & Val;
}

//--------------------------
void Rename_Loco(char *Line)                                                                                  // 15.03.22:
//--------------------------
{
  char* token = strtok(Line, "\t");
  if (token == NULL) return;
  uint16_t Nr;
  char Name[LOK_NAME_LEN+1];
  for (Nr = 0; Nr < Lok_Cnt; Nr++)
    {
    if (strcmp(token, Read_Lok_Name_from_EEPROM(Nr, Name)) == 0) break;
    }
  if (Nr >= Lok_Cnt)
     {
     Dprintf("Error: Loco '%s' not found\n", token);
     return ;
     }
  if ((token = strtok(NULL, " ")) != NULL)
     {
     Write_Lok_Name_to_EEPROM(Nr, token);
     EEPROM_Loko_Write();
     Dprintf("ren ok\n");
     }
}

//-------------------------------
void Define_Functions(char *Line)
//-------------------------------
{
  char* token = strtok(Line, "\t");
  if (token == NULL) return;
  uint16_t Nr;
  char Name[LOK_NAME_LEN+1];
  for (Nr = 0; Nr < Lok_Cnt; Nr++)
    {
    if (strcmp(token, Read_Lok_Name_from_EEPROM(Nr, Name)) == 0) break;
    }
  if (Nr >= Lok_Cnt)
     {
     Dprintf("Error: Loco '%s' not found\n", token);
     return ;
     }

  uint8_t FktArr[32];
  uint8_t FktNr = 0;
  memset(FktArr, 0, 32);

  while ((token = strtok(NULL, " ")) != NULL)
      {
      int16_t Val = atoi(token);
      if (Val < 0 || Val > 255)
         {
         Dprintf("Error: Wrong function number %s\n", token);
         return ;
         }
      if (FktNr > 31) return ;
      FktArr[FktNr++] = Val;
      if (FktNr > 31) break;
      }
  Write_FktTyp_to_EEPROM(Nr, FktArr);
  EEPROM_Loko_Write();                                                                                        // 15.03.22:
  Dprintf("fkt ok\n");
}

// Abspeichern der Lokfunktionen
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Es sind bis zu 32 Funktionen pro Lok möglich. Pro Funktion wird ein Byte benötigt.
// Dadurch ist der Speicherbedarf im EEPROM recht hoch. Momentan werden für Name, Adresse
// und uid 20 Bytes benötigt.
// Mit den Lokfunktionen werden daraus 53 Bytes. Daduch reduziert sich die Anzahl der
// speicherbaren Lokomotiven auf 77 wenn nichts anderes im EEPROM steht ;-(
// Aber bei den wenigsten Lokomotiven sind alle 32 Funktionstasten benutzt.
// - Nur die Funktionen Ablegen welche benutzt sind
// - Wenn der Speicherplatz nicht reicht, dann müssen die Funktionen einer Lok
//   nachgeladen werden.
// - Der Funktionstyp 0 hat keine Funktion und kann auch nicht von der CS2/MS2
//   aktiviert werden => Die letzten Funktionstasten mit dem Typ 0 müssen nicht
//   gespeichert werden


//---------------------------------------------------------------------
void Read_FuncTypes(char *Buffer, uint8_t FktArr[32], const char *Name)
//---------------------------------------------------------------------
// Return: Number of used functions
//
// "lokinfo" aus der CS2:                                           "lokinfo aus der MS2"
//   .fkt                                                             .fkt
//   ..nr=0                                                           ..typ=1
//   ..typ=1                                                          ..dauer=0
//   .fkt                                                             ..wert=0
//   ..nr=1                                                           .fkt
//   ..typ=41                                                         ..typ=7
//   :              Ab der Funktion 16 wird das Schlüsselwort         ..dauer=0
//   :              ".fkt2", "..nr2" und"..typ2" verwendet            ..wert=0
//   .fkt2                                                            :
//   ..nr2=16                                                         .fkt2
//   ..typ2=105                                                       ..typ2=128
//                                                                    ..dauer2=0
//                                                                    ..wert2=0
// ToDo: Untersuchen wie es bei der MS2 mit alter Software aussieht
//
{
  Dprintf("%s\n", Name);
  uint8_t FktNr = 0;
  char *p = Buffer;
  memset(FktArr, 0, 32);
  while (1)
    {
    uint16_t Val = (uint16_t)Read_Var_Dec(p, "..typ");
    if (Val == 0x80) Val = 0; // Ungültige Moment Funktion
    if (p == NULL) break;
//    if (strcmp(Name, "Tante Olga")== 0) Dprintf("%2i: FktTyp:%3i %s \n", FktNr, Val & 0x7F, Val & 0x80?"Moment":"Dauer "); // Debug
    if ((Val & 0x7F) > 127) Val = (FktNr + 50) | (Val & 0x80); // In case new versions generate numbers > 127 we replace them by the "F0", ... symbol

    if (Central_Typ == CENTR_MS2 && (Val & 0x7F) == 11)  // Fehler in der MS2? Das Symbol "Pfeife" auf der MS2 wird unter der Nummer 11 gespeichert.
        Val = 12 | (Val & 0x80);                         // In der CSx hat die "Pfeife" aber die Nummer 12. Hier ist die 11 die Trillerpfeife
                                                         // welche es bei der MS2 gar nicht gibt.

    FktArr[FktNr] = Val;
    FktNr++;
    }
  //Dprintf("Used Keys:%i\n\n", LastUsed+1); // Debug
}

//---------------------------------------------------------------------
uint8_t Read_Var_Str(char* p, const char* VarName, std::string &StrVar)
//---------------------------------------------------------------------
// Read the variable from the config data
{
  if (p)
     {
     p = strstr(p, VarName);
     if (p)
        {
        p += strlen(VarName);
        char *e = strchr(p, '\n');
        if (e)
           {
           *e = '\0';
           Dprintf("Read_Var_Str:'%s' ", p);
           StrVar = p;
           *e = '\n';
           return 1;
           }
        }
     }
  return 0;
}


//--------------------------------------------------------------------------
void Add_Lokdetails_to_Array(char *Buffer, uint16_t LokNr, const char *Name)
//--------------------------------------------------------------------------
{
  uint16_t uid = (uint16_t)Read_Var_Hex(Buffer, ".uid=0x");
  Write_Lok_Z21Adr_to_EEPROM(LokNr, uid_to_Z21Adr(uid));

  uint16_t Adr = (uint16_t)Read_Var_Hex(Buffer, ".adresse=0x");
  Write_Lok_DCC_to_EEPROM(LokNr, Adr);
  /*                                                                                                          // 18.03.22:
  uint16_t uid = (uint16_t)Read_Var_Hex(Buffer, ".uid=0x");
  Write_Lok_uid_to_EEPROM(LokNr, uid);

  uint16_t Adr = (uint16_t)Read_Var_Hex(Buffer, ".adresse=0x");
  Write_Lok_Adr_to_EEPROM(LokNr, Adr);

  //unsigned long mfxuid = Read_Var_Hex(p, ".mfxuid=0x"); Dprintf("Adr: %3i uid: %04X mfxuid: %08lX %10lu %s\n", Adr, uid, mfxuid, mfxuid, Name); // Debug
  */

  uint8_t FktArr[32];
  Read_FuncTypes(Buffer, FktArr, Name);
  Write_FktTyp_to_EEPROM(LokNr, FktArr);

  Write_OLED("%3i %s\n", Adr, Name);
}


//----------------------------------------------------------------------------
int8_t Read_Loko_Details(CAN_CLASS *can, uint16_t Start, unsigned long AnswId)
//----------------------------------------------------------------------------
// Read the deltails "adresse" and "uid" from the last added 40 lokos
// Return:
//    1: OK
//  < 0: Error
//   -1: CRC Error
//   -2: Timeout beim lesen der Länge und CRC  (5 Sek)
//   -3: Timeout beim lesen des Datenblocks    (TIMEOUT_CONF_LINE = 500 ms)
//   -4: Nicht genügend Speicher vorhanden
//   -5: Abbuch durch tastendruck
{
  int16_t Res = 0;
  for (uint16_t Nr = Start; Nr < Lok_Cnt; Nr++)
    {
    byte *Buffer;
    uint32_t size;
    char cmd[30], Name[LOK_NAME_LEN+1];
    sprintf(cmd, "lokinfo~%s", Read_Lok_Name_from_EEPROM(Nr, Name)); // '~' is replaced by '\0' in Read_Config_File_from_CAN()
    while(strlen(cmd) < 17) strcat(cmd, "~");
    //Dprintf("cmd:'%s'\n", cmd); delay(10); // Debug
    int8_t Retry = CRC_RETRYS;

    do {
       Toggle_Status_LED();
       Res = Read_Config_File_from_CAN(*can, cmd, Buffer, size, AnswId);

       if (Res == -1)  // CRC Error
            {
            Retry--;
            Dprintf("Loko Details CRC Error Retry %i\n", Retry); delay(10); // Wait until the serial data are send
            }
       if (Res > 0) // Block erfolgreich gelesen
            {
            Retry = CRC_RETRYS;
            //Print_Config_File_ASCII_or_HEX(Buffer, size); // Debug
            Add_Lokdetails_to_Array((char*)Buffer, Nr, Name);
            }
       if (Res >= 0) free(Buffer); // 0 Is returned if the file is binary and could not be decoded
       } while (Res == -1 && Retry >= 0);
    if (Res <= 0) break; // Exit the for() loop
    }
  return Res;
}

//---------------------------------------------------
void Add_Loknames_to_Array(byte *Buffer, int NumLoks)
//---------------------------------------------------
// Search for ".name=" and read the following string
{
  char *p = (char*)Buffer;
  while (p && Lok_Cnt < MAX_LOK_DATA)
    {
    p = strstr(p, ".name=");
    if (p)
       {
       p += strlen(".name=");
       char *e = strchr(p, '\n');
       if (e)
          {
          *e = '\0';
          //Dprintf("Writing to EEPROM(%2i): '%s' \n", Lok_Cnt, p); // Debug
          if (*p)
             {
             Write_Lok_Name_to_EEPROM(Lok_Cnt, p);
             Lok_Cnt++;
             }
          //if (NumLoks > 0 && Lok_Cnt >= NumLoks) break; // The old MS2 software returns the prior lok in addition if the end of the list is reached
          p = e+1;
          }
       }
    }
}

//--------------------------
int GetNumloks(byte *Buffer)
//--------------------------
// MS2 alte SW 2.7:
//   [numloks]
//   .wert=9
// CS2
//   numloks
//   .wert=59
// Der String ".wert=" kommt sonst nicht vor => DAnach wird gesucht
{
  int NumLoks = -1;
  char *p = (char*)Buffer;
//  Serial.println(Buffer);
  p = strstr(p, ".wert=");
  if (p) sscanf(p+strlen(".wert="), "%i", &NumLoks);
  return NumLoks;
}

//#define REQ_CSX     0
//#define REQ_MS2     1
//#define REQ_OLD_MS  2

//-----------------------------------------------------------------------------------------------------------------
int16_t Read_Lok_Names_from_CAN(CAN_CLASS *can, uint16_t Start, uint8_t ReqCnt, unsigned long AnswId, int &NumLoks)
//-----------------------------------------------------------------------------------------------------------------
// Request_Typ: REQ_CSX    = CSx     "loknamen0 40"  (0 is replaced by Start)
//              REQ_MS2    = MS2     "lokliste"
//              REQ_OLD_MS = Old MS2 "loknamen0 2"   (0 is replaced by Start)
// Return:
//  < 0: in case of an error in Read_Config_File_from_CAN()
//   -1: CRC Error
//   -2: Timeout beim lesen der Länge und CRC  (5 Sek)
//   -3: Timeout beim lesen des Datenblocks    (TIMEOUT_CONF_LINE = 800 ms)
//   -4: Nicht genügend Speicher vorhanden
//   0.. the number of read lokos
// NumLoks is only read when start=0 and ReqCnt > 0
{
  char cmd[20];
  if (ReqCnt > 0)
       sprintf(cmd, "loknamen%i %i", Start, ReqCnt);  // CS2 or old MS2 Ver. 2.7
  else strcpy( cmd, "lokliste");                      // MS2 Ver. 3.121

  byte *Buffer;
  uint32_t size;
  int16_t Ret, Res = Read_Config_File_from_CAN(*can, cmd, Buffer, size, AnswId);
  if (Res > 0)
        {
        //Print_Config_File_ASCII_or_HEX(Buffer, size); // Debug
        if (ReqCnt > 0 && Start == 0) NumLoks = GetNumloks(Buffer);
        int16_t Old_Lok_cnt = Lok_Cnt;
        Add_Loknames_to_Array(Buffer, NumLoks);
        Ret = Lok_Cnt - Old_Lok_cnt;
        }
   else Ret = Res;
   if (Res >= 0) free(Buffer); // 0 Is returned if the file is binary and could not be decoded
   return Ret;
}
/*
typedef struct
  {
  uint16_t Ix;
  //uint8_t  Prio;
  uint16_t Adr;
  } Sort_Dat_t;

//-------------------------------------------------
int Comare_Sort_Dat(const void * a, const void * b)
//-------------------------------------------------
// Sort by
// 1. Adr
// 2. Prio
// 3. Ix
{
  if (((Sort_Dat_t*)a)->Adr - ((Sort_Dat_t*)b)->Adr != 0)
       return ((Sort_Dat_t*)a)->Adr - ((Sort_Dat_t*)b)->Adr;
//  else {
//       if (((Sort_Dat_t*)a)->Prio - ((Sort_Dat_t*)b)->Prio != 0)
//            return ((Sort_Dat_t*)a)->Prio - ((Sort_Dat_t*)b)->Prio;
//       else return ((Sort_Dat_t*)a)->Ix  -  ((Sort_Dat_t*)b)->Ix;
//       }
}
*/
#if   EXT_ADR_RANGE_1_TYP == EA_TYP_MFX
                                                       #define PRIO_MFX  1
      #if EXT_ADR_RANGE_2_TYP == EA_TYP_DCC
                                                       #define PRIO_DCC  2
                                                       #define PRIO_MM   3
      #else
                                                       #define PRIO_MM   2
                                                       #define PRIO_DCC  3
      #endif
#elif EXT_ADR_RANGE_1_TYP == EA_TYP_DCC
                                                       #define PRIO_DCC  1
      #if EXT_ADR_RANGE_2_TYP == EA_TYP_MFX
                                                       #define PRIO_MFX  2
                                                       #define PRIO_MM   3
      #else
                                                       #define PRIO_MM   2
                                                       #define PRIO_MFX  3
      #endif

#elif EXT_ADR_RANGE_1_TYP == EA_TYP_MM
                                                       #define PRIO_MM   1
      #if EXT_ADR_RANGE_2_TYP == EA_TYP_MFX
                                                       #define PRIO_MFX  2
                                                       #define PRIO_DCC  3
      #else
                                                       #define PRIO_DCC  2
                                                       #define PRIO_MFX  3
      #endif
#else
  #error "ERROR EXT_ADR_RANGE_1_TYP must be MFX, DCC or MM"
#endif


//------------------------------
int utf8_strlen(const char *str)
//------------------------------
{
    uint16_t i,ix,q;
    for (q=0, i=0, ix=strlen(str); i < ix; i++, q++)
        {
        char c = str[i];
        if      (c>=0   && c<=127)   i+=0;
        else if ((c & 0xE0) == 0xC0) i+=1;
        else if ((c & 0xF0) == 0xE0) i+=2;
        else if ((c & 0xF8) == 0xF0) i+=3;
        else return 0;//invalid utf8
        }
    return q;
}

//--------------------------
void PrintSpace(uint8_t cnt)
//--------------------------
{
  for (uint8_t i = 0; i < cnt; i++)
      Dprintf(" ");
}

int8_t Get_FuncKeys_and_Set_FuKy_Max(uint16_t Lok_Nr, uint8_t FktTyp[32]); // Forward definition

//-----------------------------------
void Print_Lok_Data(uint8_t Mode = 1)
//-----------------------------------
// Mode 0: Unsorted list
{
  /*
  Sort_Dat_t *Sort_Dat = (Sort_Dat_t*)malloc(Lok_Cnt*sizeof(Sort_Dat_t));
  for (uint16_t i = 0; i < Lok_Cnt; i++)
    {
    Sort_Dat[i].Ix = i;
    //Sort_Dat[i].DCC = Read_Lok_DCC_from_EEPROM(i);
    //uint16_t Z21Adr = Read_Lok_Z21Adr_from_EEPROM(i);

    if (uid <= 0x07FF)        Sort_Dat[i].Prio = PRIO_MM;  // MM
    else if (uid <= 0x7FFF)   Sort_Dat[i].Prio = PRIO_MFX; // MFX
    else if (uid >= 0xC000)   Sort_Dat[i].Prio = PRIO_DCC; // DCC
    else                      Sort_Dat[i].Prio = PRIO_MFX; // Should mot be possible, but in case...
    }
// ToDo: Sort by name?
*/
//  if (Mode >= 1) qsort(Sort_Dat, Lok_Cnt, sizeof(Sort_Dat_t), Comare_Sort_Dat);

  //uint16_t Prior_z21_adr = 0;
  uint8_t First = 1;
  for (uint16_t i = 0; i < Lok_Cnt; i++)
    {
    char Name[LOK_NAME_LEN+1];
    Read_Lok_Name_from_EEPROM(i, Name);
    uint16_t Z21Adr = Read_Lok_Z21Adr_from_EEPROM(i);
    uint16_t DCC    = Read_Lok_DCC_from_EEPROM(i);
    uint16_t uid    = Z21Adr_to_uid(Z21Adr);
    if (Mode >= 1)
         { // Calculate the z21 adress
         /*
         uint16_t z21_Adr = Sort_Dat[j].Adr;
         // Wenn die vorangegangene Lok die gleiche Adresse hat, dann muss ein Offset zur Z21 Adr addiert werden
         // Die Daten in Sort_Dat sind sortiert nach:
         //   1. Adresse
         //   2. MFX, DCC, MM    (definiert über EXT_ADR_RANGE_1_TYP, EXT_ADR_RANGE_2_TYP, EXT_ADR_RANGE_3_TYP, EXT_ADR_RANGE_4_TYP)
         //   3. Position in der Liste
         if (j > 0                            // Not the first loco
            && z21_Adr == Sort_Dat[j-1].Adr)  // Prior loco has got the same adress
            {
            if (     uid <= 0x03FF)   z21_Adr += EXT_ADR_OFFS_MM;                   // MM  loco
            else if (uid <  0x7FFF)   z21_Adr  = EXT_ADR_OFFS_SDI + (uid - 0x4000); // MFX loco               // Warum hier nicht "<= 0x7FFF" ?
            else                      z21_Adr += EXT_ADR_OFFS_DCC;                  // DCC loco
            }
         char z21_str[10];
         sprintf(z21_str, "%4i", z21_Adr);
         if (Prior_z21_adr == z21_Adr) strcat(z21_str, "!"); // Add "!" in case a adress is used several times. That's possible
         Prior_z21_adr = z21_Adr;                            // if not both locos are located on the track at the same time.
                                                             // Attenton: The first name will be displayed
         char OptAdr[20];
         */
         char s[20];
         if (uid <= 0x03ff)      strcpy(s,  "MM");
         else if (uid <= 0x7FFF) sprintf(s, "MFX");
         else                    strcpy(s,  "DCC");
         if (First)
            {
            Dprintf("z21Ad Adr  uid  Typ Nr  Name               %s\n", Mode>=2?"Functions":"");
            Dprintf("~~~~~ ~~~~ ~~~~ ~~~ ~~~ ~~~~~~~~~~~~~~~~~~ %s\n", Mode>=2?"~~~~~~~~~":"");
            First = 0;
            }
         Dprintf("%-5i %4i %04X %-3s %3i \"%s\"", Z21Adr, DCC, uid, s, i+1, Name);
         if (Mode >= 2)
            {
            // Function keys
            PrintSpace(16 - utf8_strlen(Name));
            uint8_t FktTyp[32];
            int8_t MaxUsedFkt = Get_FuncKeys_and_Set_FuKy_Max(i, FktTyp);
            for (uint8_t i = 0; i <= MaxUsedFkt; i++)
                Dprintf("%4i", FktTyp[i]);
            }
         Dprintf("\n");

         }
    else { // Mode == 0
         if (First)
            {
            Dprintf("Adr  uid  Nr  Name\n");
            Dprintf("~~~~ ~~~~ ~~~ ~~~~~~~~~~~~~~~~\n");
            First = 0;
            }
         Dprintf("%4i %04X %3i %s\n", DCC, uid, i, Name);
         }
    }
  Dprintf("Total: %i\n", Lok_Cnt);
  //free(Sort_Dat);
}

//----------------------------------------------------------------------------------
void Update_OLED_Reading_loko_Config_Display(uint16_t DotCnt, uint32_t StartWaiting)
//----------------------------------------------------------------------------------
{
  char Dots[41];
  int8_t  RemChr = 20 - strlen(Get_Central_Typ_Str())*2;  // Characters after the Central Typ String (Not 100 Exakt, because the font 3 is not 2 times as large as the font 1
  if (DotCnt >= RemChr) DotCnt = RemChr;
  memset(Dots, '.', DotCnt);
  Dots[DotCnt] = '\0';
  if (DotCnt >= RemChr) sprintf(Dots+DotCnt, "\n(Waiting %lis)", (millis() - StartWaiting)/1000);
             //1234567890123
  Write_OLED("\14\n\23"            // Clear screen, font 3 (Medium size font)
             "Reading loco\n"
             "config from\n"
             "%s\21%s\n", Get_Central_Typ_Str(), Dots); // \21 = small font
}

//----------------------------------------------
uint8_t Read_Lok_Config_from_CAN(CAN_CLASS *can)
//----------------------------------------------
// Mit diesem Befehl kann man die Loknamen und die zugehoerige
// uid Adresse aus der MS2 oder der CS2/3 abfragen.
// Dazu muss die neue Software Version 3.121 oder großer (getestet mit 3.148)
// auf der MS2 installiert sein.
// Die Software erkennt man daran, dass unten bis zu 40 Lokomotiven stehen koennen
// Bei der Version 2.x koennen nur 11 Lokomotiven verwendet werden.
// Für die MS2 und die CS2/3 müssen unterschiedliche Anfragen verwendet werden.
// Die MS2 Software 2.7 untescheidet sich auch noch von der Version 3.121.
// Ich vermute, dass mit der Version 3.0 die neue Variante verwendet wird.
// Die Antworten unterscheiden sich leider ebenso.
// Bei der CS2 könnte man auch die komprimiert Liste "Read lokstat von CS2.txt"
// verwenden. Allerdings ist die unkomprimierte Liste so gross, das der Speicher
// des EPP8266 nicht reicht.
//
//    MS2                                CS2
//    Anfrage:"lokliste"                 Anfrage: "loknamen0 200"
//    "Read lokliste von MS2.txt"        "Read loknamen0 200 von CS2.txt"
//    ---------------------------        --------------------------------
//    [lokliste]                         [lokomotive]
//          .dv=0                        lok
//      .idx=0                            .name=038 193-9 DB
//     .llindex=0                        lok
//         .name=Blackfoot                .nr=1
//            .crc=49620                  .name=051 013-1
//         .idx=1                        lok
//     .llindex=1                         .nr=2
//         .name=Back Pearl               .name=103 113-7
//           .crc=17322                  lok
//         .idx=2                         .nr=3
//     .llindex=2                         .name=110 487-6 DB AG
//         .name=Blaufuchs               lok
//            .crc=27553                  .nr=4
//         .idx=3                         .name=140 396-3 DB
//     .llindex=3                        lok
//         .name=Browny                   .nr=5
//       .crc=1339                        .name=185 401-7 DB AG
//       :                               :
// In dieser Funktion werden nur die Namen ausgelesen.
// Bei der MS2 kommen immer alle Lokomotiven. Die Liste ist geschätzt maximal 3K
// groß, da nur 40 Lokomotiven möglich sind.
// Bei der CS2/3 ist die Anzahl deutlich größer. Darum werden die Daten in
// Blöcken abgefragt.
// Der ATMega2560 hat nur 8K RAM. Zum Speichern von 40 Lokomotiven im RAM werden
// 880 Byte benötigt => Das passt auch beim ATMega2560.
//
// Die "lokliste" Abfrage ist nicht in "cs2CAN-Protokoll-2_0.pdf" dokumentiert ;-(
//
// Es werden Blöcke a 40 Lokomotiven abgefragt.
// Nach jedem Block werden die Details zu den Lokomotiven ausgelesen.
{
  Dprintf("Starting Read_Lok_Config_from_CAN\n");
  if (Get_Central(can) == CENTR_UNKNON) return 0;
  uint16_t DotCnt = 1;
  uint32_t StartReadLoko = millis();
  Update_OLED_Reading_loko_Config_Display(DotCnt, StartReadLoko);
  Lok_Cnt = 0;
  uint16_t Start = 0;


  unsigned long AnswId = 0; // Central_CanId;    // Die Bestimmung der AnswId in ProcPing() funktioniert noch nicht
  if (Central_Typ == CENTR_CSx) AnswId = Hash;   // wenn zwei MS2 an einer GB hängen. Ich weiß nicht welche MS2
                                                 // der Master ist.
                                                 // Darum wird der Filter (AnswId = Central_CanId) momentan
                                                 // deaktiviert (AnswId=0) wenn eine MS2 erkannt wurde.
                                                 // Bei zwei MS2 kommt es nicht zu Problemen. Vermutlich
                                                 // weil die Slave MS2 nicht sofort Daten anfragt.
                                                 // => Die Central_CanId wird nicht benutzt

//  uint8_t  Request_Typ = REQ_MS2;
//  if (Central_Typ == CENTR_CSx) Request_Typ = REQ_CSX;
  uint8_t ReqCnt;
  if (Central_Typ == CENTR_CSx) ReqCnt = 40;
  else { // MS2
       if (Central_Ver >= 0x0300) // >= Ver. 3.0
            ReqCnt = 0;  // command "lokliste" is used
       else ReqCnt = 2;  // command "loknamen0 2"   (0 is replaced by Start)
       }

  int16_t Res;
  int8_t Retry = CRC_RETRYS;
  int NumLoks = 0; // NumLoks only set if ReqCnt > 0
  EEPROM_Loko_Clear_All();

  do {
     Toggle_Status_LED();
     Res = Read_Lok_Names_from_CAN(can, Start, ReqCnt, AnswId, NumLoks); // *** Read the loco names ***
     switch (Res)
        {
        case -1: // CRC Error
                 Retry--;
                 Dprintf("Loko Names CRC Error Retry %i\n", Retry); delay(10); // Wait until the serial data are send
                 break;
        case -2: // Central doesn't answer the first CAN message (Still booting)
                 Retry = CRC_RETRYS;
                 Update_OLED_Reading_loko_Config_Display(++DotCnt, StartReadLoko);
                 break;
        }
     if (Res > 0) // Block erfolgreich gelesen
        {
        Retry = CRC_RETRYS;
        if (Read_Loko_Details(can, Start, AnswId) < 0) // *** Read the loko details ***
           {
           Res = -1;
           break; // Exit do..while if to many CRC errors ocoured
           }
        Start += ReqCnt;
        }
     } while ((ReqCnt > 0 && Lok_Cnt < NumLoks && Lok_Cnt < MAX_LOK_DATA) ||         // - noch nicht alle Daten der CSx oder MS2 mit alter Sw gelesen wurden
              (Res == -1 && Retry >= 0) ||                                           // - ein CRC Fehler aufgetreten ist
              (Res == -2 && (millis() - StartReadLoko)/1000 < MAX_BOOTTIME_CENTRAL));// - ein timeout beim lesen der ersten CAN Antwort aufgetreten ist

  switch (Res)
    {
    case -1:   //          1234567890123
               Write_OLED("\14\23"                // Clear screen, font 3 (Medium size font)
                 "No answer or\n"
                 "to much CRC\n"
                 "errors ;-(\21");                // \21 = small font
               break;
    case -2:   //          1234567890123
    case -5:   Write_OLED("\14\23"                // \23 = font 3 (Medium size font)
                          "\nAborted ;-(\n\21");  // \21 = small font
               break;
    default:   if (Res >= 0)//12345678901234567890
                   Write_OLED("\n"
                              "%i Lokos in %is\n", Lok_Cnt, (millis() - StartReadLoko)/1000);
    }
  if (Res < 0) Lok_Cnt = 0;

  Set_Status_LED(0);
  //Write_Lok_Cnt_to_EEPROM(Lok_Cnt);

  Print_Lok_Data(2);

  EEPROM_Loko_Write();

  DisplayStart_in_xx_ms(60*1000);
  return 1;
}

#if CREATE_NOT_FOND_LOCOS


//--------------------------------------------------
uint8_t Add_Detected_Loco_to_EEPROM(uint16_t Z21Adr)
//--------------------------------------------------
// Is called if a unknown loco is detected in Find_Index_from_Adr()
{
  if (Lok_Cnt >= MAX_LOK_DATA) return 0;

  // Die Adresse kommt von der Z21 App.
  // Standard Bereiche:
  //    1 - 4000 (MFX), DCC oder MM
  // 4001 - 8000 DCC    Adr: Z21Adr - 4000  uid: 0xC000 + Adr
  // 8001 - 9000 MFX    Adr: ?              uid: 0x4000 + Z21Adr - 8000
  // 9001 - 9255 MM     Adr: Z21Adr - 9000  uid: Adr
  //
  // Dummerweise können die Bereiche konfiguriert werden
  // Ich mache es jetzt erst mal für den Fall, das
#if EXT_ADR_RANGE_1_TYP != EA_TYP_MFX || EXT_ADR_RANGE_2_TYP != EA_TYP_DCC || EXT_ADR_RANGE_3_TYP != EA_TYP_SID  || EXT_ADR_RANGE_4_TYP  != EA_TYP_MM
   #error "Currently not supported adress range combination!"
#endif
  const char *TypP;
  if (     Z21Adr < Z21_ADR_MM_START)     TypP = "DCC";
  else if (Z21Adr < Z21_ADR_MFX_START)    TypP = "MM";
  else                                    TypP = "MFX";
  Dprintf("Add_Detected_Loco_to_EEPROM(%i) uid %04X\n", Z21Adr, Z21Adr_to_uid(Z21Adr)); // Debug

  /*
  uint16_t Adr, uid;
  if (     Z21Adr <= EXT_ADR_RANGE_END_1)  { Adr = Z21Adr;                     uid = 0xC000 + Adr; TypP = "DCC"; } // <= 4000: Map as DCC
  else if (Z21Adr <= EXT_ADR_RANGE_END_2)  { Adr = Z21Adr-EXT_ADR_RANGE_END_1; uid = 0xC000 + Adr; TypP = "DCC"; } // <= 8000: Map as DCC
  else if (Z21Adr <= EXT_ADR_RANGE_END_3)  { Adr = Z21Adr-EXT_ADR_RANGE_END_2; uid = 0x4000 + Adr; TypP = "MFX"; } // <= 9000: Map as MFX
  else                                     { Adr = Z21Adr-EXT_ADR_RANGE_END_3; uid =          Adr; TypP = "MM";  } // <= 9255: Map as MM
  Dprintf("Add_Detected_Loco_to_EEPROM(%i) Adr %i uid %04X\n", Z21Adr, Adr, uid); // Debug
  */

  char Name[20], Sufix = 'A';
  do // Check if the name already exists
    {
    if (Sufix == 'A')
         sprintf(Name, "New %s %i",    TypP, Z21Adr);
    else sprintf(Name, "New %s %i %c", TypP, Z21Adr, Sufix);
    Sufix++;
    if (Sufix > 'Z') { Dprintf("Error Sufix > 'Z'\n"); return 0; }
    } while (Find_Lokname_in_Lok_Data(Name) != 0xFFFF);

  Write_Lok_Name_to_EEPROM(Lok_Cnt, Name);
  uint16_t LokNr = Lok_Cnt++;

  Write_Lok_DCC_to_EEPROM(LokNr, Z21Adr_to_DCC(Z21Adr));
  Write_Lok_Z21Adr_to_EEPROM(LokNr, Z21Adr);

  uint8_t FktArr[32];
  for (uint8_t i = 0; i < 32; i++)
    FktArr[i] = 50+i;
  Write_FktTyp_to_EEPROM(LokNr, FktArr);
  EEPROM_Loko_Write();
  return 1;
}
#endif // CREATE_NOT_FOND_LOCOS

#endif // USE_CAN
