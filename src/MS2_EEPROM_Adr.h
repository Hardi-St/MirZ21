// EEPROM adresses used in the MS2 Programm
#ifndef __MS2_EEPROM_Adr_h__
#define __MS2_EEPROM_Adr_h__


#define IR_LOC_CNT            4
#if defined(ESP32)
  #define MAX_LOK_DATA        1000    // Not enough RAM to use more locos
  #define MAX_LOK_DATA_BYTES  10000   // 10560 Bytes geht gerade noch mit der default patitionierung von 20 KiB
#else
  #define MAX_LOK_DATA        350     // Not enough RAM to use more locos
  #define MAX_LOK_DATA_BYTES  3300    // Only 4096 bytes EEPROM which are also used by the Z21 part from Phillip
#endif

int16_t Ma_Lok_Nr[4] = { 0, 1, 2, 3 };

char Exp_VerStr_EEPROM[] = "MirZ21_0.1";   // This string is written to the EEPROM to check if the EEPROM data are valid.
                                           // Change the version number if the EEPROM layout is changed.

#if defined(ESP8266)                             // The ESP32 uses the preferences library
  #define VERSTR_EEPROM               DCC_EESize // First possible start adress. Prior mem is used by Z21, ...  // 15
  #define MA_LOK_NR_EEPROM            (VERSTR_EEPROM    + sizeof(Exp_VerStr_EEPROM))
  #define ALL_EEPROM_DATA             (MA_LOK_NR_EEPROM + sizeof(Ma_Lok_Nr))
#endif

#endif // __MS2_EEPROM_Adr_h__
