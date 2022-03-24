#ifndef __EEPROM_Read_Write_H__
#define __EEPROM_Read_Write_H__
/*
 ToDo: Der Text stimm nicht mehr
       Mit der Preferences Bibliothek kann man mehr als 4096 Byte speichern

 https://unsinnsbasis.de/esp32-preferences/

 Bisher wurden die Lok Daten immer sofort im EEPROM abgelegt                                                // 11.02.22:
 Das habe ich so gemacht weil beim Arduino nicht genügend RAM zur verfügung steht.

 Beim ESP existiert kein EEPROM. Die Daten werden im Flash gespeichet. Da man hier nicht einzelne Bytes
 schreiben kann werden die Daten im RAM zwischengspeichert.
 Das bedeutet, dass man im RAM genau so viel Speicher braucht wie man im EEPROM Ablegen will.
 Wenn ich das richtig sehe kan man beim ESP bis zu 4096 Bytes als "EEPROM" nutzen.
 Zur Speicherung der Lokomotiven wird das aber knapp, wenn man 100 Lokomotiven Speichern will.
 Eine Lok belegt:
   uid;                 2
   Adresse;             2
   Name[LOK_NAME_LEN];  16
   FktTyp[32];          32
                        52
 Bei 100 Lokomotiven wären das 5200 Bytes. Das passt nicht in den verfügbaren Speicher.
 Darum sollen die Daten komprimiert werden. Ich vermute dass man dann nur noch etwa die
 Hälfte des Speichers benötigt. Allerdings kann man dann nicht mehr so einfach eine
 maximal mögliche Anzahl an möglichen Lokomotiven angeben.

 Änderungen:
 - Speichern der Daten in einer Struktur im RAM
   Die bestehenden Routinen greifen darauf zu.                                                                OK
 - Der commit Befehl schreibt die Daten in das simulierte EEPROM                                              OK
 - Beim Start werden die Daten in den RAM kopiert                                                             OK

*/


#include "Compr_Lok_Data.h"

void Write_Ma_Lok_Nr_Arr_to_EEPROM(); // Forward definition
void Write_Verstr_EEPROM();
void EEPROM_Loko_Write();

//--------------------------------------------------
void Check_and_Correct_Lok_Nr_Read(uint16_t &Lok_Nr)
//--------------------------------------------------
{
  #if 1 // If the program runs stabile this check could be disabled
    if (Lok_Nr >= Lok_Cnt)
       {
       Dprintf("ERROR: Wrong Lok_Nr: %i! => Set to 0\n", Lok_Nr);
       Lok_Nr = 0;
       }
  #endif
}

//---------------------------------------------------
void Check_and_Correct_Lok_Nr_Write(uint16_t &Lok_Nr)
//---------------------------------------------------
{
  #if 1 // If the program runs stabile this check could be disabled
    if (Lok_Nr >= MAX_LOK_DATA)
       {
       Dprintf("ERROR: Wrong Lok_Nr: %i! => Set to %i\n", Lok_Nr, MAX_LOK_DATA-1);
       Lok_Nr = MAX_LOK_DATA-1;
       }
  #endif
}



//------------------------------------
uint8_t Read_Lok_Cnt_from_EEPROM(void)
//------------------------------------
{
  #ifdef DEBUG_LOK2EEPROM_R
    Dprintf("Read Lok_Cnt_EEPROM=%i\n",All_EEPROM_Data.Lok_Cnt);
  #endif
  return All_EEPROM_Data.Lok_Cnt;
}

//----------------------------------------
void Write_Lok_Cnt_to_EEPROM(uint16_t Cnt)
//----------------------------------------
{
  All_EEPROM_Data.Lok_Cnt = Cnt;
  #ifdef DEBUG_LOK2EEPROM_W
    Dprintf("Lok_Cnt_EEPROM="); Dprintf("%i\n",Cnt);
  #endif
}

//----------------------------------------------------------------------
char *Read_Lok_Name_from_EEPROM(uint16_t Nr, char Buffer[LOK_NAME_LEN+1])
//----------------------------------------------------------------------
{
  Check_and_Correct_Lok_Nr_Read(Nr);
  memset(Buffer, 0, LOK_NAME_LEN+1);
  if (Nr < Lok_Cnt)
       memcpy(Buffer, All_EEPROM_Data.Const_Lok_Data[Nr].Name, sizeof(All_EEPROM_Data.Const_Lok_Data[0].Name));
  else *Buffer = '-';
  #ifdef DEBUG_LOK2EEPROM_R
    Dprintf("Read Lok_Name[");
    Dprintf("%i] '%s'\n", Nr, Buffer);
  #endif
  return Buffer;
}

//---------------------------------------------------------
void Write_Lok_Name_to_EEPROM(uint16_t Nr, const char *Name)
//---------------------------------------------------------
{
  Check_and_Correct_Lok_Nr_Write(Nr);
  #ifdef DEBUG_LOK2EEPROM_W
    Dprintf("Write_Lok_Name_to_EEPROM[");
    Dprintf("%i]='%s'\n",Nr, Name);
  #endif
  memset(All_EEPROM_Data.Const_Lok_Data[Nr].Name, 0, sizeof(All_EEPROM_Data.Const_Lok_Data[0].Name)); // fill the unused chars with 0 to get a better compression
  strncpy(All_EEPROM_Data.Const_Lok_Data[Nr].Name, Name, sizeof(All_EEPROM_Data.Const_Lok_Data[0].Name));
}
//-----------------------------------------------
uint16_t Read_Lok_Z21Adr_from_EEPROM(uint16_t Nr)
//-----------------------------------------------
{
  Check_and_Correct_Lok_Nr_Read(Nr);
  uint16_t ret = All_EEPROM_Data.Const_Lok_Data[Nr].Z21Adr;
  #ifdef DEBUG_LOK2EEPROM_R
    Dprintf("Read_Lok_Z21Adr_from_EEPROM[%i]=%X\n",Nr, ret);
  #endif
  return ret;
}

//-----------------------------------------------------------
void Write_Lok_Z21Adr_to_EEPROM(uint16_t Nr, uint16_t Z21Adr)
//-----------------------------------------------------------
{
  Check_and_Correct_Lok_Nr_Write(Nr);
  All_EEPROM_Data.Const_Lok_Data[Nr].Z21Adr = Z21Adr;
  #ifdef DEBUG_LOK2EEPROM_W
    Dprintf("Write_Lok_Z21Adr_to_EEPROM[%i]=%X\n",Nr, Z21Adr);
  #endif
}

//-----------------------------------------------------
void Write_Lok_DCC_to_EEPROM(uint16_t Nr, uint16_t DCC)
//-----------------------------------------------------
{
  Check_and_Correct_Lok_Nr_Write(Nr);
  All_EEPROM_Data.Const_Lok_Data[Nr].DCC = DCC;
  #ifdef DEBUG_LOK2EEPROM_W
    Dprintf("Write_Lok_DCC_to_EEPROM[%i]=%X\n",Nr, DCC);
  #endif
}

//--------------------------------------------
uint16_t Read_Lok_DCC_from_EEPROM(uint16_t Nr)
//--------------------------------------------
{
  Check_and_Correct_Lok_Nr_Read(Nr);
  uint16_t DCC = All_EEPROM_Data.Const_Lok_Data[Nr].DCC;
  #ifdef DEBUG_LOK2EEPROM_R
    Dprintf("Read_Lok_DCC_from_EEPROM[%i]=%X\n",Nr, DCC);
  #endif
  return DCC;
}
/*                                                                                                            18.03.22:
//-------------------------------------------
uint16_t Read_Lok_uid_from_EEPROM(uint16_t Nr)
//-------------------------------------------
{
  Check_and_Correct_Lok_Nr_Read(Nr);
  uint16_t ret = All_EEPROM_Data.Const_Lok_Data[Nr].uid;
  #ifdef DEBUG_LOK2EEPROM_R
    Dprintf("Read_Lok_uid_from_EEPROM[%i]=%X\n",Nr, ret);
  #endif
  return ret;
}

//-----------------------------------------------------
void Write_Lok_uid_to_EEPROM(uint16_t Nr, uint16_t uid)
//-----------------------------------------------------
{
  Check_and_Correct_Lok_Nr_Write(Nr);
  All_EEPROM_Data.Const_Lok_Data[Nr].uid = uid;
  #ifdef DEBUG_LOK2EEPROM_W
    Dprintf("Write_Lok_uid_to_EEPROM[%i]=%X\n",Nr, uid);
  #endif
}
//---------------------------------------------------------
void Write_Lok_Adr_to_EEPROM(uint16_t Nr, uint16_t Adresse)
//---------------------------------------------------------
{
  Check_and_Correct_Lok_Nr_Write(Nr);
  All_EEPROM_Data.Const_Lok_Data[Nr].Adresse = Adresse;
  #ifdef DEBUG_LOK2EEPROM_W
    Dprintf("Write_Lok_Adr_to_EEPROM[%i]=%X\n",Nr, Adresse);
  #endif
}

//--------------------------------------------
uint16_t Read_Lok_Adr_from_EEPROM(uint16_t Nr)
//--------------------------------------------
{
  Check_and_Correct_Lok_Nr_Read(Nr);
  uint16_t Adresse = All_EEPROM_Data.Const_Lok_Data[Nr].Adresse;
  #ifdef DEBUG_LOK2EEPROM_R
    Dprintf("Read_Lok_Adr_from_EEPROM[%i]=%X\n",Nr, Adresse);
  #endif
  return Adresse;
}
*/
//----------------------------------------------------------
void Write_FktTyp_to_EEPROM(uint16_t Nr, uint8_t FktTyp[32])
//----------------------------------------------------------
{
  Check_and_Correct_Lok_Nr_Write(Nr);
  memcpy(All_EEPROM_Data.Const_Lok_Data[Nr].FktTyp, FktTyp, sizeof(All_EEPROM_Data.Const_Lok_Data[0].FktTyp));
}

//-----------------------------------------------------------
void Read_FktTyp_from_EEPROM(uint16_t Nr, uint8_t FktTyp[32])
//-----------------------------------------------------------
{
  Check_and_Correct_Lok_Nr_Read(Nr);
  memcpy(FktTyp, All_EEPROM_Data.Const_Lok_Data[Nr].FktTyp, sizeof(All_EEPROM_Data.Const_Lok_Data[0].FktTyp));
}

//--------------------------
void EEPROM_Loko_Clear_All()
//--------------------------
{
  memset(&All_EEPROM_Data, 0, sizeof(All_EEPROM_Data));
}

//------------------------
void EEPROM_Set_Defaults()
//------------------------
{
  //Dprintf("EEPROM_Set_Defaults()\n");
  Lok_Cnt = 0;
  EEPROM_Loko_Clear_All();
  Write_Verstr_EEPROM();
  EEPROM_Loko_Write();
  memset(Ma_Lok_Nr, 0, sizeof(Ma_Lok_Nr));
  Write_Ma_Lok_Nr_Arr_to_EEPROM();
}


#if defined(ESP8266)
  //-----------------------
  void Debug_Print_EEPROM()
  //-----------------------
  {
    int Cnt = 50;
    for (uint16_t j = 0; j < 5; j++)
      {
      uint16_t Offs = j*Cnt;
      for (uint16_t i = 0; i < Cnt; i++)
        {
        uint8_t b = FIXSTORAGE.read(i+VERSTR_EEPROM+Offs);
        if (b >= 32)
             Dprintf("%c ", b);
        else Dprintf(". ");
        }
      Dprintf("\n\n");
      for (uint16_t i = 0; i < Cnt; i++)
        {
        uint8_t b = FIXSTORAGE.read(i+VERSTR_EEPROM+Offs);
        Dprintf("%02X", b);
        }
      Dprintf("\n");
      }
  }
#endif

#if defined(ESP32)
#include <Preferences.h>
#define LOCO_NAMESPACE "LOCO_CFG"

   //------------------------
   void Err_Msg_prefs_begin()
   //------------------------
   {
     Dprintf("Error opening the NVS-Namespace\n");
   }

   //---------------------------
   uint8_t Check_Verstr_EEPROM()
   //---------------------------
   // Return 1: If the version string in the EEPROM is correct
   {
     Dprintf("Pref Check_Verstr_EEPROM\n");
     char VerStr[sizeof(Exp_VerStr_EEPROM)];
     Preferences prefs;
     if (!prefs.begin(LOCO_NAMESPACE, true)) Err_Msg_prefs_begin();
     if (!prefs.getString("Version", VerStr, sizeof(VerStr)))
        {
        Dprintf("Error reading the version from EEPROM\n");
        prefs.clear();
        }
     prefs.end();

     uint8_t Res = (strcmp(Exp_VerStr_EEPROM, VerStr) == 0);
     #ifdef DEBUG_LOK2EEPROM_R
       Dprintf("VERSTR_EEPROM: '%s': %s\n", VerStr, Res?"OK":"Err");
     #endif
     return Res;
   }

   //------------------------
   void Write_Verstr_EEPROM()
   //------------------------
   {
     Preferences prefs;
     if (!prefs.begin(LOCO_NAMESPACE)) Err_Msg_prefs_begin();
     if (!prefs.putString("Version", Exp_VerStr_EEPROM))
        {
        Dprintf("Error writing the version to EEPROM\n");
        }
     prefs.end();
   }

   //-----------------------------------
   void Read_Ma_Lok_Nr_Arr_from_EEPROM()
   //-----------------------------------
   {
     Preferences prefs;
     if (!prefs.begin(LOCO_NAMESPACE)) Err_Msg_prefs_begin();
     if (!prefs.getBytes("Ma_Lok_Nr", &Ma_Lok_Nr, sizeof(Ma_Lok_Nr)))
             Dprintf("ERROR reading Ma_Lok_Nr[] from EEPROM\n");
     prefs.end();
   }

   //----------------------------------
   void Write_Ma_Lok_Nr_Arr_to_EEPROM()
   //----------------------------------
   {
     Preferences prefs;
     if (!prefs.begin(LOCO_NAMESPACE)) Err_Msg_prefs_begin();
     if (!prefs.putBytes("Ma_Lok_Nr", &Ma_Lok_Nr, sizeof(Ma_Lok_Nr)))
             Dprintf("ERROR writing Ma_Lok_Nr[] to EEPROM\n");
     prefs.end();
   }

   //------------------------------------------------
   uint16_t Read_Ma_Lok_Nr_from_EEPROM(uint8_t MaeNr)
   //------------------------------------------------
   {
     if (MaeNr < 4)
          return Ma_Lok_Nr[MaeNr];
     else {
          Dprintf("ERROR wrong MaeNr %i in Read_Ma_Lok_Nr_from_EEPROM\n", MaeNr);
          return 0;
          }
   }

   //-----------------------------------------------------------
   void Write_Ma_Lok_Nr_to_EEPROM(uint8_t MaeNr, uint16_t LokNr)
   //-----------------------------------------------------------
   {
     if (MaeNr >= 4)
        {
        Dprintf("ERROR wrong MaeNr %i in Write_Ma_Lok_Nr_to_EEPROM\n", MaeNr);
        return ;
        }
     Ma_Lok_Nr[MaeNr] = LokNr;
     Write_Ma_Lok_Nr_Arr_to_EEPROM();
   }



   //-------------------------
   void Read_All_EEPROM_Data()
   //-------------------------
   {
     Dprintf("Pref EEPROM_Loco_setup\n");
     Preferences prefs;
     if (!prefs.begin(LOCO_NAMESPACE, true)) Err_Msg_prefs_begin();
     uint8_t *CompBuff = (uint8_t*)malloc(MAX_LOK_DATA_BYTES);
     if (prefs.getBytes("Locos", CompBuff, MAX_LOK_DATA_BYTES) <  5)
        {
        Dprintf("Error reading Loco data from EEPROM => Setting defaults\n");
        EEPROM_Set_Defaults();
        }
     Decompr_Lok_Data(CompBuff, All_EEPROM_Data);
     free(CompBuff);
     prefs.end();
   }

   //----------------------
   void EEPROM_Loko_Write()
   //----------------------
   {
     Dprintf("Pref EEPROM_Loko_Write()\n");
     Write_Lok_Cnt_to_EEPROM(Lok_Cnt);                                                                        // 15.03.22:
     Preferences prefs;
     if (!prefs.begin(LOCO_NAMESPACE)) Err_Msg_prefs_begin();

     prefs.remove("Locos");

     uint8_t *CompBuff = NULL;
     int32_t UsedSize = Compr_Lok_Data(All_EEPROM_Data, CompBuff, MAX_LOK_DATA_BYTES);
     if (UsedSize > 0)
        {
        if (!prefs.putBytes("Locos", CompBuff, UsedSize))
           {
           Dprintf("Error writing Loco data to EEPROM. (freeEntries:%li)\n", prefs.freeEntries());
           }
        }
     free(CompBuff);
     prefs.end();

   }

#else // not ESP32
   //---------------------------
   uint8_t Check_Verstr_EEPROM()
   //---------------------------
   // Return 1: If the version string in the EEPROM is correct
   {
   //  std::string VerStr;
   //    VerStr = EEPROMread(sizeof(Exp_VerStr_EEPROM), Exp_VerStr_EEPROM);
     char VerStr[sizeof(Exp_VerStr_EEPROM)];
     FIXSTORAGE.get(VERSTR_EEPROM, VerStr);


     uint8_t Res = (strcmp(Exp_VerStr_EEPROM, VerStr) == 0);
     #ifdef DEBUG_LOK2EEPROM_R
       Dprintf("VERSTR_EEPROM: '%s': %s\n", VerStr, Res?"OK":"Err");
     #endif
     return Res;
   }

   //------------------------
   void Write_Verstr_EEPROM()
   //------------------------
   {
     FIXSTORAGE.put(VERSTR_EEPROM, Exp_VerStr_EEPROM);
   }

   //-----------------------------------
   void Read_Ma_Lok_Nr_Arr_from_EEPROM()
   //-----------------------------------
   {
     //Dprintf("Read_Ma_Lok_Nr_Arr_from_EEPROM\n");
     uint8_t *p = (uint8_t*)&Ma_Lok_Nr;
     for (uint16_t i = MA_LOK_NR_EEPROM; i < MA_LOK_NR_EEPROM + sizeof(Ma_Lok_Nr); i++, p++)
         *p = FIXSTORAGE.read(i);
   }

   //----------------------------------
   void Write_Ma_Lok_Nr_Arr_to_EEPROM()
   //----------------------------------
   {
     //Dprintf("Write_Ma_Lok_Nr_Arr_to_EEPROM\n");
     uint8_t *p = (uint8_t*)&Ma_Lok_Nr;
     for (uint16_t i = MA_LOK_NR_EEPROM; i < MA_LOK_NR_EEPROM + sizeof(Ma_Lok_Nr); i++, p++)
         FIXSTORAGE.FIXMODE(i, *p);
     FIXSTORAGE.commit();
   }

   //------------------------------------------------
   uint16_t Read_Ma_Lok_Nr_from_EEPROM(uint8_t MaeNr)
   //------------------------------------------------
   {
     if (MaeNr < 4)
          {
          //Dprintf("Read_Ma_Lok_Nr_to_EEPROM[%i]=%i\n", MaeNr, Ma_Lok_Nr[MaeNr]); // Debug
          return Ma_Lok_Nr[MaeNr];
          }
     else {
          Dprintf("ERROR wrong MaeNr %i in Read_Ma_Lok_Nr_from_EEPROM\n", MaeNr);
          return 0;
          }
   }

   //-----------------------------------------------------------
   void Write_Ma_Lok_Nr_to_EEPROM(uint8_t MaeNr, uint16_t LokNr)
   //-----------------------------------------------------------
   {
     if (MaeNr >= 4)
        {
        Dprintf("ERROR wrong MaeNr %i in Write_Ma_Lok_Nr_to_EEPROM\n", MaeNr);
        return ;
        }
     Ma_Lok_Nr[MaeNr] = LokNr;
     //Dprintf("Write_Ma_Lok_Nr_to_EEPROM[%i]=%i\n", MaeNr, Ma_Lok_Nr[MaeNr]); // Debug
     Write_Ma_Lok_Nr_Arr_to_EEPROM();
   }

   //-------------------------
   void Read_All_EEPROM_Data()
   //-------------------------
   {
     Dprintf("Read_All_EEPROM_Data\n"); delay(100);
     uint8_t *CompBuff = (uint8_t*)malloc(MAX_LOK_DATA_BYTES);
     uint8_t *p = CompBuff;
     int32_t End = ALL_EEPROM_DATA + MAX_LOK_DATA_BYTES;
     for (uint16_t i = ALL_EEPROM_DATA; i < End; i++, p++)
         *p = FIXSTORAGE.read(i);

     Decompr_Lok_Data(CompBuff, All_EEPROM_Data);
     free(CompBuff);
     //Dprintf("EEPROM_Loco_setup Lok_Cnt:%i\n", Read_Lok_Cnt_from_EEPROM());
   }

   //----------------------
   void EEPROM_Loko_Write()
   //----------------------
   {
     Dprintf("EEPROM_Loko_Write()\n");
     Write_Lok_Cnt_to_EEPROM(Lok_Cnt);                                                                        // 15.03.22:
     uint8_t *CompBuff = NULL;
     int32_t UsedSize = Compr_Lok_Data(All_EEPROM_Data, CompBuff, MAX_LOK_DATA_BYTES);
     if (UsedSize > 0)
        {
        uint8_t *p = CompBuff;
        int32_t End = ALL_EEPROM_DATA + UsedSize;
        for (int32_t i = ALL_EEPROM_DATA; i < End; i++, p++)
            FIXSTORAGE.FIXMODE(i, *p);
        }
     free(CompBuff);
     FIXSTORAGE.commit();
   }
#endif // not ESP32

//----------------------
void EEPROM_Loco_setup()
//----------------------
{
  // ToDo: Versionsprüfung hier
  // - Default Werte auch für Write_Ma_Lok_Nr_Arr_to_EEPROM()
  if (!Check_Verstr_EEPROM())
       EEPROM_Set_Defaults();
  else {
       Read_All_EEPROM_Data();
       Read_Ma_Lok_Nr_Arr_from_EEPROM();
       }
  //Test_Compr_Lok_Data(All_EEPROM_Data); // Test
}



#endif // __EEPROM_Read_Write_H__
