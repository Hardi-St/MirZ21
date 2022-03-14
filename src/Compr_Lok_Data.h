/*
 Compression / Decompression of the All_EEPROM_Data structure
 to be able to store more locos to the EEPROM.

 The compression reduces the memory usage at the moment by 1.7 to 2.1 (Depends on the loco list)

 If the EEPROM is not large enough to store all locos some features are not stored.
 - Function types => Only the function number (F0-F31) instead of the specific icon is shown
 - Loco name      => "Lok Nr" is shown

 Estimated maximal locos    ES8266    ESP32
 Name and function:         120       320
 Name only:                 220       700
 No Name and function:      620       2000
 Max Loko in RAM            350       1000

 Unfortunately the RAM is also limmiting the maximal possible locos. The usable RAM is difficult do
 define because it is also used for the stack and for functions which use heap.

*/



// Abspeichern der Lokdaten
// ~~~~~~~~~~~~~~~~~~~~~~~~
// Ich habe keine Bibliothek gefunden mit der man die Daten vernünftig komprimieren kann.
// - "miniz" braucht beim Komprimieren so viel speicher, das der ESP32 abstützt
// - "shoco" bringt nichts:   https://github.com/Ed-von-Schleck/shoco  https://ed-von-schleck.github.io/shoco/
// - "smaz"  scheint so ähnlich zu sein => Nicht getestet: https://github.com/antirez/smaz
// => Es werden einfach nur die Bytes gespeichert die benötigt werden
//    - Nur so viele Bytes wie der Lokname lang ist ohne abschließende '\0'
//      Die Länge-1 wird in einen 1/2 Byte gespeichert.
//    - Nur dei tatsächlich belegten Funktionstypen bis zur letzten benutzten Funktion.
//      Die Anzahl der Funktionen wird in einem zusätzlichen Byte gespeichert.
//    - Die Adresse wird nur bei MFX gespeichert. Bei MM und DCC kann sie aus der uid generiert werden
//    - Wenn mehr Lokomotiven vorhanden sind als in den Speicher passen, dann werden Daten weggelassen
//      - Zunächst werden Funktionstypen weggelassen
//      - Anschließend Loknamen
//
// Bytes      Beschreibung
// -----      ------------
// 2             Lok_Cnt
// Lok_Cnt/2     4 bit für die Länge-1 der Loknamen, Die Länge 0 ist nicht erlaubt.
// Daten pro Lok:
// 2             uid
// (2)           Adresse wenn MFX (0x4000 <= uid <= 0x7FFF)
// 1             Anzahl der Funktionen  0-32.
//               100 - 132 wenn nicht genügend Speicher vorhanden ist zum Speichern der einzelnen
//               Funktionen. In der Anzeige wird dann kein Symbol sondern nur F0..F32 angezeigt
//               und der Block mit den Funktionstypen entfällt.
//
// 1-16*Lok_Cnt  Name aller Lokomotiven in einem Block ohne Trennzeichen. Die Position wird aus der
//               Tabelle oben ermittelt. Die Namen werden als ein Block geschrieben damit sie später evtl.
//               mit Huffman comprimiert werden können. Außerdem ist es dadurch möglich, dann die Namen
//               nur dann geschrieben werden wenn der Speicher ausreicht.
// 0-32*Lok_Cnt  Typ der Funktionen aller Lokomotiven. Auch hier werden die Daten in einem Block geschrieben.
//
// Leider wird der Speicherbedarf der Loknamen nicht verringert, indem die Loknamen mit "shoco" komprimiert
// werden. Auf der Web Seite gibt es eine Testeingabe.
//   https://ed-von-schleck.github.io/shoco/
// Diese zeigt aber keine besonders guten Ergebnisse. Insbesondere bei Großbuchstaben.
//  "R1020 003-8 ÖBB" => -12%
//  "BR 194 178-0 DB" => 0%
//  "ICE Wagenlicht"  => 21%
//  "Cora"            => 25%
//  Alle Namen        => 1%    "PERCYICE 2BR03  1022302924 047 DBBR89066110 487-6 DB AGE44v200 060BR 41 029VT-95 Schienenbu185 401-7 DB AGE 141 207-1DB AG 245 006BR 80KITTEL CIDT8 DBE 141 211-3038 193-9 DBT3 Waldkirch2016 021-5 ÖBBE 103 113-7BR81 006335 183-0R1020 003-8 ÖBBKLVM 3087BR 94 713, DBCargo 364 754-2V60 1009BR 86 173BR 94 713, DB siDHG 500232 103-2 RDX051 013-1BR 120 139-1GlobalTrans 16212 FeuerwehrBR 194 178-0 DBDraisine KLV 12VT 98103 113-7E 69 02140 396-3 DB185 409-0 D-RDXBR81 002E 69 02 DB ZugE194 091-5CoraV100 212 349-5 bBR 216 219-6BR89066 54723V 60 1179DHG 500 Nr. 11RolfML 00389 006 Fabi 1646KRAN WEISSBR 24 014MONDOLINOE 04 01ICE WagenlichtDcc-TempIcon Test 1Icon Test 2Icon Test 3Icon Test 4Uml ÄÖÜäöü"
//
// Mit Huffman währe die Kompression deutlich besser.
// Auf dieser Seite kann man das testen:  https://www.dcode.fr/huffman-tree-compression
// Mit allen Namen wird eine Kompressionsrate von 62% errechnet
// Original    5568 => Compressed  3484

// Momentan werden die Daten nur im EEPROM in diesem Format gespeichert. Im RAM liegen sie in der Struktur
// All_EEPROM_Data_t. Dadurch wird recht viel RAM benötigt. Die Daten in der All_EEPROM_Data_t Struktur
// benötigen bei meinen beiden Testdatensätzen etwa doppelt so viel Speicher wie die EEPROM Daten.
// Zum Schreiben/Lesen wird noch eine Koppie der Daten des EEPROMs im RAM verwendet. Dadurch wird der
// 1.5 fache RAM benötigt.
//
// Evtl. wäre es sinnvoll wenn die Daten nur in dem EEPROM Format vorgehalten werden. Das Programm greift
// nur über die Funktionen im Modul "EEPROM_Read_Write.h" auf die Daten zu. Man könnte die Funktionen
// entsprechend anpassen so dass sie nur noch auf das EEPROM Format zugreifen. Das wird allerdings etwas
// langsamer werden weil man nicht direkt auf eine bestimmten Namen oder eine Funktion zugreifen kann.
// Bein schreiben der Lokdaten wird es noch komplizierter. Hier müssen Daten verschoben werden wenn eine
// Lok hinzu kommt. Aber dass kann man mit memmove machen.
//
// Dadurch wird die Anzahl der vollständig speicherbaren Lokomotiven aber nicht größer als das durch
// die Größe des EEPROMs vorgegeben ist (Siehe oben).


//-------------------------------------
uint8_t Get_Lok_Name_Len(const char *s)
//-------------------------------------
// Return the length-1: 0..15
{
  for (uint8_t i = 1; i < LOK_NAME_LEN; i++) // We assume that the first character in not '\0'
    {
    if (s[i] == '\0') return i-1;
    }
  return 15;
}

//------------------------------------------
void Store_uint16(uint8_t* &p, uint16_t Val)
//------------------------------------------
{
  *(p++) = Val & 0xFF;
  *(p++) = Val >> 8;
}

//-------------------------------
uint16_t Read_uint16(uint8_t* &p)
//-------------------------------
{
  uint16_t Val;
  Val = *(p++);
  Val |= (*(p++)) << 8;
  return Val;
}

//--------------------------------------------
uint8_t Get_used_Functions(uint8_t FktTyp[32])
//--------------------------------------------
// Returns the number of used funktion
//  0, if no function is used
{
  int8_t UsedFkt = 0;
  for (uint8_t i = 0; i < 32; i++)
    {
    if ((FktTyp[i] & 0x7F) > 0) UsedFkt = i + 1;
    }
   return UsedFkt;
}

//--------------------------------------------------------------
uint8_t Get_Lok_Name_Len_from_Buff(uint8_t *Buff, uint16_t LokNr)
//--------------------------------------------------------------
{
  uint8_t  Len;
  uint16_t ByteNr = LokNr / 2;
  if (LokNr % 2)
        Len = Buff[ByteNr] >> 4;
  else  Len = Buff[ByteNr] & 0x0F;
  Len++;                                                                                // Dprintf("Namelen(%i)=%i\n", LokNr, Len);
  return Len;
}

//----------------------------------------------------------------------
void Set_Lok_Name_Len_to_Buff(uint8_t *Buff, uint16_t LokNr, uint8_t Len)
//----------------------------------------------------------------------
// Attention: The function assumes that the prior stored length is 0!
//            => Don't call the function several times to change the data
{
  uint16_t ByteNr = LokNr / 2;
  if (LokNr % 2)
        Buff[ByteNr] |= Len<<4;
  else  Buff[ByteNr] |= Len;
}


//-----------------------------------------------------------
int8_t Decompr_Lok_Data(uint8_t* &Buff, All_EEPROM_Data_t &e)
//-----------------------------------------------------------
{                                                                                       // Dprintf("Decompr_Lok_Data\n");
  uint8_t *p = Buff;
  memset(&e, 0, sizeof(All_EEPROM_Data_t));
  e.Lok_Cnt =Read_uint16(p);                                                            // Dprintf("Lok_Cnt %i\n", e.Lok_Cnt);
  uint8_t *Lok_Name_Len_Start = p;

  uint8_t **Used_Func_Arr = (uint8_t**)malloc(sizeof(uint8_t*) * e.Lok_Cnt);
  if (Used_Func_Arr == NULL) { Dprintf("Error allocating Used_Func_Arr Lok_Cnt:%i\n", e.Lok_Cnt); return -1; }
  p+= (e.Lok_Cnt+1)/2;                                                                  // Dprintf("Start uid %i\n", p-Buff);

  for (uint16_t i = 0; i < e.Lok_Cnt; i++)
    {
    Const_Lok_Data_t *Ld = &e.Const_Lok_Data[i];
    uint16_t uid = Ld->uid = Read_uint16(p);                                            // Dprintf("Uid(%i)=%04X\n", i, uid);
    if (uid >= 0x4000 && uid <= 0x7FFF)
         Ld->Adresse = Read_uint16(p);
    else {
         if (uid <= 0x3FF)
               Ld->Adresse = uid;
         else  Ld->Adresse = uid - 0xC000;
         }
    Used_Func_Arr[i] = p++;
    }

  for (uint16_t i = 0; i < e.Lok_Cnt; i++)
    {
    Const_Lok_Data_t *Ld = &e.Const_Lok_Data[i];
    uint8_t Name_Len = Get_Lok_Name_Len_from_Buff(Lok_Name_Len_Start, i);               // Dprintf("Len=%3i  ", Name_Len);
    memset(Ld->Name, 0, LOK_NAME_LEN);
    if (Name_Len == 1 && *p == '!') // Lok name not stored (Not enough EEPROM)
         {
         p++;
         sprintf(Ld->Name, "Lok %i", i);
         }
    else {
         for (uint8_t j = 0; j < Name_Len; j++)
             Ld->Name[j] = *(p++);
         }
                                                                                        // { char Name[LOK_NAME_LEN+1]; memcpy(Name, Ld->Name, LOK_NAME_LEN); Name[LOK_NAME_LEN] = '\0'; Dprintf("%3i: Name='%s'\n", i, Name);}
    }

  for (uint16_t i = 0; i < e.Lok_Cnt; i++)
    {
    Const_Lok_Data_t *Ld = &e.Const_Lok_Data[i];
    uint8_t UsedFkt = *Used_Func_Arr[i];                                                // Dprintf("UsedFkt(%i)=%i\n", i, UsedFkt);
    if (UsedFkt < 100) // 100..130 is used if not enough memory is available
         {             // to store the functions. In this case the F0..F32 is shown
         for (uint8_t j = 0; j < UsedFkt; j++)
             Ld->FktTyp[j] = *(p++);
         }
    else {
         UsedFkt -= 100;
         for (uint8_t j = 0; j < UsedFkt; j++)
             Ld->FktTyp[j] = 50+j;
         }
    }

  free(Used_Func_Arr);
  return 1;
}

// Wenn der Speicher im EEPROM nicht reicht, dann sollen zunächst
// die Funktionstypen einiger Lokomotiven weggelassen werden.
// Beim Laden der Daten wird dann das F0-F31 Symbol verwendet. Im Speicher
// wird bei UsedFkt 100 zur Anzahl addiert.
// Wenn das nicht reicht, dann können auch noch die Namen der Lokomotiven
// weggelassen werden. Im Speicher wird das vermerkt indem die Länge-1 = 0
// ist und als Buchstabe '!' eingetragen wird.

//---------------------------------------------------------------------------------------------
uint16_t Calc_Memory_Usage(All_EEPROM_Data_t &e, uint16_t &WithOutFunc, uint16_t &WithOutNames)
//---------------------------------------------------------------------------------------------
{
  uint16_t Mem = 2;                                      // Lok_Cnt;
  uint16_t FuncMem = 0;
  uint16_t NameMem = 0;
  Mem += (e.Lok_Cnt+1)/2;                                // Length of the lok names
  for (uint16_t i = 0; i < e.Lok_Cnt; i++)
      {
      Const_Lok_Data_t *Ld = &e.Const_Lok_Data[i];
      Mem++;                                             // First char of the loco name
      uint16_t NMem = Get_Lok_Name_Len(Ld->Name);        // Lok names
      Mem += NMem;
      NameMem += NMem;
      uint16_t uid = Ld->uid;
      Mem += 2;                                          // uid
      if (uid >= 0x4000 && uid <= 0x7FFF) Mem += 2;      // Adresse
      Mem++;                                             // Used Functions
      uint8_t UsedFkt = Get_used_Functions(Ld->FktTyp);
      FuncMem += UsedFkt;
      }
  WithOutNames = Mem - NameMem;
  WithOutFunc  = Mem;
  return Mem + FuncMem;
}

//---------------------------------------------------------------------
uint16_t MaxLoco_WithoutName(All_EEPROM_Data_t &e, size_t Max_Res_Size)
//---------------------------------------------------------------------
{
  uint16_t Mem = 2;                                      // Lok_Cnt;
  uint16_t i;
  for (i = 0; i < e.Lok_Cnt; i++)
      {
      if (i % 2 == 0) Mem++;
      Const_Lok_Data_t *Ld = &e.Const_Lok_Data[i];
      Mem++;                                             // First char of the loco name
      uint16_t uid = Ld->uid;
      Mem += 2;                                          // uid
      if (uid >= 0x4000 && uid <= 0x7FFF) Mem += 2;      // Adresse
      Mem++;                                             // Used Functions
      if (Mem > Max_Res_Size) break;
      }
  return i;
}

//------------------------------------------------------------------------------
int32_t Compr_Lok_Data(All_EEPROM_Data_t &e, uint8_t* &Res, size_t Max_Res_Size)
//------------------------------------------------------------------------------
{
  Res = (uint8_t*)calloc(Max_Res_Size, 1);
  if (Res == NULL) { Dprintf("Error allocating Buffer to compress the loco data\n"); return -1; }

  uint8_t *p = Res;
  uint16_t Lok_Cnt = e.Lok_Cnt;
  uint16_t Max_Loco = MaxLoco_WithoutName(e, Max_Res_Size);
  if (Max_Loco < Lok_Cnt)
     {
     Dprintf("Not enough memory to store all %i locos ;-( Only %i locos are stored\n", e.Lok_Cnt, Max_Loco);
     Lok_Cnt = Max_Loco;
     }

  uint8_t **Used_Func_Arr = NULL;
  if (Lok_Cnt)
     {
     Used_Func_Arr = (uint8_t**)malloc(sizeof(uint8_t*) * Lok_Cnt);
     if (Used_Func_Arr == NULL) { Dprintf("Error allocating Used_Func_Arr (Lok_Cnt=%i)\n", Lok_Cnt); return -2; }
     }

  Store_uint16(p, Lok_Cnt);
  uint8_t *Lok_Name_Len_Tab = p;
  p+= (Lok_Cnt+1)/2; // At the moment all length are 0. Later they will be filled
  for (uint16_t i = 0; i < Lok_Cnt; i++)
    {
    WDT_RESET();                                                                        // Dprintf("i=%i\n", i);

    Const_Lok_Data_t *Ld = &e.Const_Lok_Data[i];
    uint16_t uid = Ld->uid; // uid
    Store_uint16(p, uid);                                                               // Dprintf("uid=%04X\n", uid);
    if (uid >= 0x4000 && uid <= 0x7FFF) Store_uint16(p, Ld->Adresse);  // Adresse
    Used_Func_Arr[i] = p;
    *(p++) = 100 + Get_used_Functions(Ld->FktTyp);  // Functions
                                                                                        // Dprintf("Used_Func_Arr[%i]=%i\n", i, (int)*Used_Func_Arr[i]);
    }

  for (uint16_t i = 0; i < Lok_Cnt; i++)    // All loco names
    {
    Const_Lok_Data_t *Ld = &e.Const_Lok_Data[i];
    uint8_t Name_Len = Get_Lok_Name_Len(Ld->Name)+1; // Name
    size_t FreeMem = Max_Res_Size - (p - Res) - (Lok_Cnt - i);                          // Dprintf("Name_Len(%2i)=%i\n", i, Name_Len);
    if (FreeMem >= Name_Len)  // Enough memory to store the first character of the remaining locos and the full name of this loco ?
         {
         for (uint8_t j = 0; j < Name_Len; j++)
             *(p++) = Ld->Name[j];
         Set_Lok_Name_Len_to_Buff(Lok_Name_Len_Tab, i, Name_Len-1);
         }
    else {                                                                              // Dprintf("Not enough memory %i to store the lok name %li\n", FreeMem, i);
         *(p++) = '!'; // Mark that the lok name is not stored. Necessary because the name length is only 4 bit => It's not possible to store the information in the length
         }
    }

  for (uint16_t i = 0; i < Lok_Cnt; i++)    // All function types
    {
    Const_Lok_Data_t *Ld = &e.Const_Lok_Data[i];
    uint8_t UsedFkt = Get_used_Functions(Ld->FktTyp);
    size_t FreeMem = Max_Res_Size - (p - Res);
    if (FreeMem >= UsedFkt) // Enough memory to store the function types?
         {                                                                              // Dprintf("Add Funct to Loco %i  FreeMem %li\n", i, FreeMem);
         for (uint8_t j = 0; j < UsedFkt; j++)
             {
             *(p++) = Ld->FktTyp[j];
             }
         *Used_Func_Arr[i] = UsedFkt;
         }
    else {                                                                              // Dprintf("Not enough memory %li to store the func types for loco %i\n", FreeMem, i);
         break;
         }
    }
   //Dprintf("Compressed size=%i NormSize=%i\n", p-Res, 1+Lok_Cnt*sizeof(Const_Lok_Data_t));
   free(Used_Func_Arr);
   return p-Res;
}


//--------------------------------------------
void Test_Compr_Lok_Data(All_EEPROM_Data_t &e)
//--------------------------------------------
{
  uint16_t WithOutFunc, WithOutNames;
  uint16_t FullMem = Calc_Memory_Usage(e, WithOutFunc, WithOutNames);
  Dprintf("EEPROM Usage for the current %i Locos: Full %i, WithOutFunc %i, WithOutNames %i\n", e.Lok_Cnt, FullMem, WithOutFunc, WithOutNames);


  size_t AlSize = 3300;
  uint8_t *Res;
  Compr_Lok_Data(e, Res, AlSize);

  All_EEPROM_Data_t *Re = (All_EEPROM_Data_t*)malloc(sizeof(All_EEPROM_Data_t));

  Decompr_Lok_Data(Res, *Re);

  for (uint16_t i = 0; i < e.Lok_Cnt; i++) // Debug
    {
    Const_Lok_Data_t *Ld = &Re->Const_Lok_Data[i];
    char Name[LOK_NAME_LEN+1]; memcpy(Name, Ld->Name, LOK_NAME_LEN); Name[LOK_NAME_LEN] = '\0'; Dprintf("%4i %04X %3i %s\n", e.Const_Lok_Data[i].Adresse, e.Const_Lok_Data[i].uid, i, Name);
    }

  Dprintf("Compare %i\n", memcmp(&e, Re, sizeof(All_EEPROM_Data_t))); // Compare

/*
  if (memcmp(&e, Re, sizeof(All_EEPROM_Data_t)))
     {
     uint8_t *p = (uint8_t*)&e; uint8_t *q = (uint8_t*)Re ;
                       for (uint16_t i = 0; i < sizeof(All_EEPROM_Data_t); i++,p++,q++) Dprintf("%c ", *p!=*q?'!':' '); Dprintf("\n");
     p = (uint8_t*)&e; for (uint16_t i = 0; i < sizeof(All_EEPROM_Data_t); i++,p++) Dprintf("%02X", *p); Dprintf("\n");
     p = (uint8_t*)Re; for (uint16_t i = 0; i < sizeof(All_EEPROM_Data_t); i++,p++) Dprintf("%02X", *p); Dprintf("\n");
     p = (uint8_t*)Re; for (uint16_t i = 0; i < sizeof(All_EEPROM_Data_t); i++,p++) Dprintf("%c ", *p >= ' ' && *p< 128?*p:'.'); Dprintf("\n");
     }
*/
  free(Res);
  free(Re);
}


/*   ---- Test of the "shoco" compression ----------

#include <shoco.h>

//---------------
void Test_Shoco()
//---------------
// Shoco bringt praktisch keine Einsparung
// Zum Dekodieren wird die Länge der komprimierten Daten benötigt.
// Das frisst den gewonnenen Speicher auf.
// Unsere Konfig: 460  => 134  Saved 31
// Jürgen:        1280 => 676  Saved 6
//
{
  uint16_t Saved = 0, Used = 0;
  for (uint16_t i = 0; i < All_EEPROM_Data.Lok_Cnt; i++)
    {
    char Comp[30], out[LOK_NAME_LEN+1];
    memset(Comp, 0x00, sizeof(Comp));
    size_t CompLen = shoco_compress(All_EEPROM_Data.Const_Lok_Data[i].Name, LOK_NAME_LEN, Comp, sizeof(Comp));
    Comp[CompLen] = 0x00; // Wenn hier was anderes reinschreibt, dann Stürtzt das Prog ab wenn keine Länge beim Dekomprimieren angegeben ist
    memset(out, 0, sizeof(out));
    size_t DcmpLen = shoco_decompress(Comp, sizeof(Comp)-1 , out, sizeof(out)-1);

    uint8_t ok = strncmp(All_EEPROM_Data.Const_Lok_Data[i].Name, out, LOK_NAME_LEN) == 0;

    char Name[LOK_NAME_LEN+1];
    memset(Name, 0, sizeof(Name));
    strncpy(Name, All_EEPROM_Data.Const_Lok_Data[i].Name, LOK_NAME_LEN);
    Dprintf("%2i -> %2i Decomp %2i %s %s\n", strlen(Name), CompLen, DcmpLen, ok?"=":"!", Name);
    Saved += strlen(Name) - CompLen;
    Used += CompLen;
    }
  Dprintf("Saved: %i Old Total %i Comp %i\n", Saved, All_EEPROM_Data.Lok_Cnt*20, Used);
}
*/
