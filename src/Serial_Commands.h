#if USE_SERIAL_COMMANDS
/*
Modul to read in serial commands for tests
*/
const char CmdHelp[] PROGMEM =
    "Commands:\n"
#if defined(USE_CAN)
    "r <name>    Read config data from the CAN: \n"
    "             r loknamen0 20   or   r lokliste\n"
    "             r lokinfo~Rolf~~~~~\n"
    "             :\n"
    "r <Nr>      Read config data entry according to page 46 'cs2CAN-Protokoll-2_0.pdf'\n"
    "read        Read all loko data\n"
    "list        List al loko data (list 0 = raw)\n"
    "fkt         Define the loco funktions (Name<TAB>nr nr ...)\n"
    "ren         Rename a loko (OldName<TAB>NewName)\n"
    #if TEST_LOCO_SELECTION
    "adr Nr a    Set the adress for a loco for tests (uid for MM and DCC is corrected)\n"
    "uid Nr u    Set the uid for a loco for tests\n"
    "t z21Adr    Test the detection of a z21 adress\n"
    #endif
    #if SPY_CAN_CONFIG_FILE
    "spy         Enable/Disable config file spying\n"
    #endif
#endif
    "ram         print the avilable memory\n"
    "restart     Restart the program\n"
    "clear       Clear the EEPROM\n"
    "A,A1,A2,A3,A4,A5  Show animation\n"
  #if defined(ESP32)
    "nvr         Info about the nvr\n"
  #endif
  #if defined(ESP8266)
    "EE          print the first bytes of te EEPROM\n"
  #endif
    ;

#define MAX_MESSAGE_LENGTH  200  // Must be long enough to hold the "fkt" commands

#if defined(USE_CAN)

#if TEST_LOCO_SELECTION
  //-----------------------------------
  void Change_Loco_Adr(const char *Par)
  //-----------------------------------
  {
    int LNr, Adr;
    if (sscanf(Par, "%i %i", &LNr, &Adr) == 2)
       {
       Write_Lok_Adr_to_EEPROM(LNr, Adr);
       uint16_t uid = Read_Lok_uid_from_EEPROM(LNr);
       uint16_t NewUid = uid;
       if (     uid <= 0x07FF && Adr <= 0x07FF) NewUid = Adr;
       else if (uid >= 0x7FFF)                  NewUid = 0xC000 + Adr;
       if (NewUid != uid) Write_Lok_uid_to_EEPROM(LNr, NewUid);
       Print_Lok_Data();
       EEPROM_Loko_Write();
       }
  }

  //-----------------------------------
  void Change_Loco_uid(const char *Par)
  //-----------------------------------
  {
    int LNr, uid;
    if (sscanf(Par, "%i %i", &LNr, &uid) == 2)
       {
       Write_Lok_uid_to_EEPROM(LNr, uid);
       Print_Lok_Data();
       EEPROM_Loko_Write();
       }
  }

  //-----------------------------------
  void Test_z21_Adress(const char *Par)
  //-----------------------------------
  {
    int z21_Adr;
    if (sscanf(Par, "%i", &z21_Adr) == 1)
       {
       #if 1 // It's already printed in CAN_Read_Lok_Config.h at the moment => just call the Find function
          Find_Index_from_Adr(z21_Adr);
       #else
          uint16_t Ix = Find_Index_from_Adr(z21_Adr);
          char Name[LOK_NAME_LEN+1];
          if (Ix != 0xFFFF)
                Dprintf("Adr:%4i uid:%04X %s\n", z21_Adr, Read_Lok_uid_from_EEPROM(Ix), Read_Lok_Name_from_EEPROM(Ix, Name));
          else  Dprintf("Adr:%4i Not found ;-(\n", z21_Adr);
       #endif
       }
  }
#endif // TEST_LOCO_SELECTION

//------------
void Restart()
//------------
{
  #if defined(ESP8266_MCU) || defined(ESP32_MCU)
      ESP.restart();
  #else
      #error "Restart function not defined for this processor at the moment ;-("
  #endif
}


#if defined(ESP32)
#include <Preferences.h>

void Err_Msg_prefs_begin();

//-----------------------
void Get_ESP32_NVR_Info()
//-----------------------
{
  Preferences prefs;
  if (!prefs.begin("LOCO_CFG", true))
       Err_Msg_prefs_begin();
  else {
       int32_t freeEntries = prefs.freeEntries(); // Die Berechnung des freien Speichers ist unklar (https://unsinnsbasis.de/esp32-preferences/)
       Dprintf("prefs.freeEntries:%li, ~%li bytes, ~%li add lokos\n", freeEntries, freeEntries*32-4096, (freeEntries*32-4096)/sizeof(Const_Lok_Data_t));
       Dprintf("Locos size:%li\n", prefs.getBytesLength("Locos"));
       }
  prefs.end();
}
#endif


//--------------------------------
void Read_Command(const char *Cmd)
//--------------------------------
{
  byte *Buffer;
  uint32_t size, Start = millis();
  int8_t Res;
  Serial.print("? Read_Command('"); Serial.print(Cmd); Serial.println("')");
  if ((Res = Read_Config_File_from_CAN(can, Cmd, Buffer, size, 0)) >= 0)  // AnswId = 0 => All messages 0x04 are processt
       {
       uint32_t Duration = millis() - Start;
       Serial.println("\n---------------------------\n");
       Print_Config_File_ASCII_or_HEX(Buffer, size);
       Serial.println("\n---------------------------\n");
       Dprintf("Received %li Bytes Duration: %li ms\n", size, Duration);
       free(Buffer);
       }
  else Serial.println(";-(");
}
#endif // USE_CAN

#define If_Msg_Start(Cmd) if (strncmp(message, Cmd, (CmdLen = strlen(Cmd))) == 0)
#define MsgPar (message+CmdLen)

//-------------------
void SerialCmd_loop()
//-------------------
{
 uint16_t ReqAll = 0;
 // Check to see if anything is available in the serial receive buffer
 while (Serial.available() > 0 || ReqAll > 0)
   {
   static char message[MAX_MESSAGE_LENGTH]; // Create a place to hold the incoming message
   static unsigned int message_pos = 0;

   char inByte = Serial.read(); // Read the next available byte in the serial receive buffer
   if ( inByte != '\n' && inByte != '\r' && (message_pos < MAX_MESSAGE_LENGTH - 1) ) // Message coming in (check not terminating character) and guard for over message size
        {
        // Add the incoming byte to our message
        if (inByte == 8) // Del
             {
             if (message_pos > 0) message_pos--;
             }
        else {
             message[message_pos] = inByte;
             message_pos++;
             }
        }
   else if (message_pos || ReqAll > 0)
        { // Full message received...
        message[message_pos] = '\0'; // Add null character to string

        delay(20); // To receive the '\n'
        if (Serial.available() && Serial.peek() == '\n') Serial.read(); // Remove '\n' because this will be interpreted as abort when
                                                                        // reading the loco data in Check_Abort_Button_or_SerialChar()
        if (ReqAll > 0)
           {
           sprintf(message, "r %2i", ReqAll++);  // Simulate
           #if GEN_ANSWER_IDLIST
               Answer_IDList += "\n";
               Answer_IDList += message;
               Answer_IDList += ":";
           #endif
           }
        Dprintf("'%s'\n", message); // Debug
        uint16_t CmdLen = 0;
        If_Msg_Start("?")     Serial.println(CmdHelp);
#if defined(USE_CAN)
        else If_Msg_Start("r ")
            {
            int16_t Nr = atoi(MsgPar); // if the command starts with a number
            switch (Nr)                                                 // Tested
               {                                                        // MS2 CS2  Absender    Bytes Bedeutung  (~ = \0)
               case 1: Read_Command("lokinfo~Rolf~~~~~"       ); break; // OK  OK  *MS2         1120  Daten einer Lok  Anfang des Lok-Namens ("Rolf" is the name of a loko) Tailing "~" to generate 3 request messages
                                                                        //                            Die Daten von MS2 und CS2 unterscheiden sich! (Siehe "lokinfo Rolf MS2.txt" und "lokinfo Rolf CS2.txt")
               case 2: Read_Command("loknamen0 20"            ); break; // -   OK  *MS2               Namen von n Loks und Gesamtzahl  1 Datagram mit Nr. der  ersten Lok und Anzahl durch Leerzeichen getrennt Text: Lok-Nr, Lok-Name und Gesamtanzahl der oks im .cs2-Format.
               case 3: Read_Command("loknamen0 200"           ); break; // -   OK  *MS2               Namen von n Loks und Gesamtzahl  1 Datagram mit Nr. der  ersten Lok und Anzahl durch Leerzeichen getrennt Text: Lok-Nr, Lok-Name und Gesamtanzahl der oks im .cs2-Format.
               case 4: Read_Command("maginfo0 20"             ); break; // -   -    MS2               Daten von n Magnetartikeln 1 Datagram mit Adresse des ersten Magnetartikels und Anzahl durch Leerzeichen getrennt Text (Ausschnitt aus "magnetartikel.cs2")
               case 5: Read_Command("lokdb~~~~"               ); break; // -   OK   MS2               Lokdatenbank  -  Binär (MS2-Lokdb-Format)
               case 6: Read_Command("lang~~~~~"               ); break; // -   OK   MS2               Sprachendatei  -  Binär (MS2-Sprachen-Format)
               case 7: Read_Command("ldbver"                  ); break; // -   OK   MS2               Versionsinfo der  Lokdatenbank -  Text: Versionsnummer,Monat, Jahr, Anzahl der Einträge im .cs2-Format
               case 8: Read_Command("langver"                 ); break; // -   OK   MS2               Versionsinfo der Sprachendatei -  Text: Versionsnummer und Größe der Sprachendatei im .cs2-Format
               case 9: Read_Command("loks"                    ); break; // -   OK   CS2 (Slave)       Komplette Lokliste  -  Konfigurationsdatei „lokomotive.cs2" Zlib-komprimiert   (Jürgen: 4672 -> 51747)
               case 10:Read_Command("mags"                    ); break; // -   OK   CS2 (Slave)       Komplette Magnetartikel- konfiguration -  Konfigurationsdatei „magnetartikel.cs2" Zlib-komprimiert
               case 11:Read_Command("gbs"                     ); break; // -   OK   CS2 (Slave)       Komplette Gleisbildgrund-konfiguration -  Konfigurationsdatei „gleisbild.cs2" Zlib-komprimiert
               case 12:Read_Command("gbs-#"                   ); break; // -   OK   CS2               Konfiguration einer  -  Konfigurationsdatei einer Gleisbildseite  Gleisbildseite Zlib-komprimiert
               case 13:Read_Command("fs"                      ); break; // -   OK   CS2 (Slave)       Komplette Fahrstraßen- konfiguration -  Konfigurationsdatei „fahrstrassen.cs2" Zlib-komprimiert
               case 14:Read_Command("lokstat"                 ); break; // -   OK   CS2 (Slave)       Aktueller Zustand der Loks -  Statusdatei „lokomotive.sr2" Zlib-komprimiert
               case 15:Read_Command("magstat"                 ); break; // -   OK   CS2 (Slave)       Aktueller Zustand der Magnetartikel -  Statusdatei „magnetartikel.sr2" Zlib-komprimiert
               case 16:Read_Command("gbsstat"                 ); break; // -   OK   CS2 (Slave)       Aktueller Zustand einer Gleisbildseite -  Statusdatei „gleisbild.sr2" Zlib-komprimiert
               case 17:Read_Command("fsstat"                  ); break; // -   OK   CS2 (Slave)       Reserve für Fahrstrassen -  Nicht implementiert
                                                                        // Undokumentiert:
               case 18:Read_Command("lokliste"                ); break; // OK  -   *MS2         1608  (24 Lokos) Wird verwendet wenn zwei MS2 an einer Gbeisbox angeschlossen sind (SW Ver 3.112)
               case 19:Read_Command("hilfe"                   ); break; // -   OK                     Hier kommen 4 Leerzeichen, Schade...
               case 20:Read_Command("loknamen0 2"             ); break; //                            Dieser Befehl habe ich bei der alten MS2 Software (mit 11 Lokomotiven) verwendet (SW Ver 2.?)
               case 21:Read_Command("loknamen2 2"             ); break; //                            Es werden immer nur zwei Lokomotiven ausgegeben. Das habe ich auch
               case 22:Read_Command("loknamen4 2"             ); break; //                            bei der Software Version 2.7 (MS2 von Gerd) überprüfen können
               case 23:Read_Command("loknamen6 2"             ); break; //
               case 24:Read_Command("loknamen8 2"             ); break; //
               case 25:Read_Command("loknamen10 2"            ); break; //
               case 26:Read_Command("ms2-conf"                ); break; // OK                         Wenn zwei MS2 an einer Gleisbox stecken. Enthält ".magprot=0" und ".lokprot=7" ?

               case 27:Read_Command("lokinfo~038 193-9 DB~~~~"); break;
               case 28:Read_Command("lokinfo~ABC~~~~~~~~~~~~~"); break;
               case 29:Read_Command("lokinfo~ICE~~~~~~~~~~~~~"); break;
               case 0: if (*MsgPar) Read_Command(MsgPar       ); break;
               default:ReqAll = 0;
               }
            #if GEN_ANSWER_IDLIST
              if (ReqAll == 0)
                 {
                 Dprintf("\nAnswer_IDList:");
                 Serial.println(Answer_IDList.c_str());
                 Dprintf("\n");
                 Answer_IDList = "";
                 }
            #endif
            }
      #if TEST_LOCO_SELECTION
        else If_Msg_Start("adr ")    Change_Loco_Adr(MsgPar);
        else If_Msg_Start("uid ")    Change_Loco_uid(MsgPar);
        else If_Msg_Start("t ")      Test_z21_Adress(MsgPar);
      #endif
        else If_Msg_Start("a")     { ReqAll = 1; Dprintf("Automatic requesting all config files\n"); }
        else If_Msg_Start("list ")   Print_Lok_Data(atoi(MsgPar));
        else If_Msg_Start("list")    Print_Lok_Data();
        else If_Msg_Start("read")    Read_Lok_Config_from_CAN(&can);
        else If_Msg_Start("fkt ")    Define_Functions(MsgPar);
        else If_Msg_Start("ren ")    Rename_Loco(MsgPar);


    #if SPY_CAN_CONFIG_FILE
        else If_Msg_Start("spy")   { Spy_Enabled = !Spy_Enabled; Dprintf("Spying %sabled\n", Spy_Enabled ? "En" : "Dis"); }
    #endif
#endif // USE_CAN
        else If_Msg_Start("ram")     Print_RAM();
        else If_Msg_Start("restart") Restart();
        else If_Msg_Start("clear")   { EEPROM_Set_Defaults(); Set_WLAN_EEPROM_to_Default();}
     #if defined(ESP8266)
        else If_Msg_Start("EE")      Debug_Print_EEPROM();
     #endif
     #if defined(ESP32)
        else If_Msg_Start("nvr")     Get_ESP32_NVR_Info();
     #endif
<<<<<<< HEAD
        else If_Msg_Start("A4")      Haus_Ani1();
        else If_Msg_Start("A5")      Haus_Ani2();
        else If_Msg_Start("A1")      Haus_Ani();
        else If_Msg_Start("A3")      Stummilok_Ani3();
=======
	else If_Msg_Start("A4")      Haus_Ani1();
        else If_Msg_Start("A5")      Haus_Ani2();
        else If_Msg_Start("A1")      Haus_Ani();
	else If_Msg_Start("A3")      Stummilok_Ani3();
>>>>>>> ce9fea7006210508549815c3158e83fcb12ffa3e
        else If_Msg_Start("A2")      Stummilok_Ani2();
        else If_Msg_Start("A")       Stummilok_Ani();
        else Dprintf("Unknown serial command. Enter ? for help.\n");
        message_pos = 0; // Reset for the next message
        }
   }
}
#else  // USE_SERIAL_COMMANDS
  void SerialCmd_loop() {}
#endif // USE_SERIAL_COMMANDS

/*
Wenn MS2 Version 3.121 und MS2 2.7 verbunden werden, dann kommt:
achdatei: V %s~Dateisystem: V %s~%s~Can-Bus Master~MS2 Slave~CS2 Slave~Ohne Gleisbox~Fkt-Icons ?ndern~    W?hle Position f?r~    das Funktionsicon~Hauptgleis~Spannung~Temperatur~Lok programmieren~Reset ausf?hren?~Default-Einstellungen der~Lok werden ?bernommen~Der CV-Wert wird gelesen~Stop-Warnung~Achtung! Stop-Zustand~aufheben!~Mobile Station Reset~Lok-DB~Update laden auf~Schreibvorgang starten~Sie m?ssen erst eine~Lokkarte einstecken~aktiv~Trotzdem entfernen?~Ist die Bearbeitung der~Funktionsicons fertig?~Lok l?schen~Endg?ltig l?schen?~Warnmeldung~Adresse existiert schon!~Trotzdem w?hlen?~Fehler beim Lesen~der Lokkarte~Erfolg~Fehler beim Schreiben~Die mfx-Lok gab keine~R?ckmeldung~Beim Datentransfer ist ein~CRC-Fehler aufgetreten~Die Lokliste des Masters~ist leer~Timeout beim~CV-Lesen/Schreiben~Der Master gab keine~Die Datenbank~Es wurde keine~Lok erkannt~Lok vom Master~Diese Lok gibt es auf~dem Master nicht (mehr)~?nderungen werden nicht~zur Lok ?bertragen~Lesen der Mfx-Blockliste~Lesen der Mfx-Itemliste~C

*/
