#if defined(USE_CAN)
/*
 Read the Märklin Config files over the CAN Bus
 The files are read into a buffer which is allocated with malloc()
 => There must be enough memory available
 Some of the files are compressed with LZib. Here a huge menory is used.


 The files are checked with a crc
*/

#if USE_LIBZ
  #include "miniz.h"     // https://github.com/richgel999/miniz/releases
#endif


#ifndef Dprintf_Mae_Conf
  #if  1 // Set to 0 to disable the messages or define Dprintf_Mae_Conf outside
     #define Dprintf_Mae_Conf(Format, ...) Dprintf(Format, ##__VA_ARGS__)
  #else
     #define Dprintf_Mae_Conf(Format, ...)  {}
  #endif
#endif

//----------------------------------------------
uint32_t ReadUIntN(byte *rxBuf, uint8_t ByteCnt)
//----------------------------------------------
{
  uint32_t res = 0;
  for (uint8_t i = 0; i < ByteCnt; i++, rxBuf++)
    res = (res << 8) + *rxBuf;
  return res;
}

#define GEN_ANSWER_IDLIST  0  // Check which message ID is used for the different requests
                              // This is important to set the filters
                              // Some answers are send back with the hash of the requesting
                              // unit to prevent message collisions.
                              // Broadcast messages are send with the hash of the sender

#if GEN_ANSWER_IDLIST
  // Die MS2 Antwortet nur auf die Anfragen 1 und 18. mit ihrem eigenen Hash
  // Die CS2 verwendet bei diesen Anfragen den Hash des Fragenden
  // Das ist unabhängig davon ob eine MS2 an die CS2 angeschlossen ist

  std::string Answer_IDList;
  //-----------------------------------------------------
  void Add_to_Answer_IDList(unsigned long rxId, byte len)
  //-----------------------------------------------------
  {
    char s[30];
    sprintf(s, "%08lX %i   ", rxId & 0x7FFFFFFF, len);
    Answer_IDList += s;
  }
#else
  void Add_to_Answer_IDList(unsigned long rxId, byte len) {}
#endif

uint32_t Start_Spy = 0;

//------------------------------------------------------------------------------------
void Add_Message_to_Hex_Buffer(unsigned long rxId, byte len, byte rxBuf[8], char *hex)
//------------------------------------------------------------------------------------
{
  sprintf(hex, "%6li %08lX %i:", millis() - Start_Spy, rxId & 0x1FFFFFFF, len);
  for (uint8_t i = 0; i < len; i++)
    sprintf(hex+strlen(hex), " %02X", rxBuf[i]);

  if (len == 8)
     {
     strcat(hex, " '");
     char *p = hex + strlen(hex);
     for (uint8_t i = 0; i < 8; i++)
       {
       if (rxBuf[i] == '\0')                       *p = '~';
       else if (rxBuf[i] != '\n' && rxBuf[i] < 32) *p = '.';
       else                                        *p = (char)rxBuf[i];
       p++;
       }
     *p = '\0';
     strcat(hex, "'");
     }
  strcat(hex, "\n");
}

uint8_t Check_Abort_Button_or_SerialChar(uint8_t &Old_Button); // Forward definition

//------------------------------------------------------------------------------------------
uint32_t Wait_for_CAN_Message(CAN_CLASS *can_r, uint8_t &len, byte *rxBuf, uint32_t Wait_ms)
//------------------------------------------------------------------------------------------
{
  //Dprintf("Wait_for_CAN_Message\n");
  uint8_t Old_Button = 0; // Don't abort if the button is always pressed (Not/wrong connected, or stuck)
  unsigned long rxId;
  uint32_t EndTime = millis() + Wait_ms;
  while (millis() < EndTime)
         {

         if (can_r->checkReceive() == CAN_MSGAVAIL)
            {
            if (can_r->readMsgBuf(&rxId, &len, rxBuf) == CAN_OK)
               {
               //Dprintf("Received Msg 0x%08lX %i\n", rxId, len);
               return rxId;
               }
            }
         WDT_RESET();
         switch (Check_Abort_Button_or_SerialChar(Old_Button))
            {
            case 1: return 0xFFFFFFFE; // Abort by pressing the button
            case 2: return 0xFFFFFFFD; // Abort by pressing serial character
            }
         }
  //Dprintf("No CAN message received from CAN\n");
  return 0xFFFFFFFF;
}

//-------------------------------------------------------------------------------------------------
uint8_t ReadExpectedBytesAndCRC(CAN_CLASS can, uint32_t &Bytes, uint16_t &crc, unsigned long &rxId)
//-------------------------------------------------------------------------------------------------
// CAN Filters have to be defined to receive only the Config data stream messagec 0x0042
// Return:
//   0: Timeout (5 Sek)
//   6: Bei dem Datenstroms wird zur Kollisionsauflösung der Hash des Empfängers verwendet.
//   7: Datenstrom als Broadcast einer Konfigänderung. Es wird Hash des Senders verwendet.
// 0xE: Abaoted by button
{
  byte rxBuf[8], len = 0;
  rxId = Wait_for_CAN_Message(&can, len, rxBuf, 5000);
  if (rxId != 0xFFFFFFFF &&       // In der ersten Message steht die Anzahl zu erwartenden Daten
      (len == 6 || len == 7))     // Answer time MS2 Hardware 2.1 ~1.1ms
       {
       Bytes =  ReadUIntN(rxBuf,   4);
       crc   =  ReadUIntN(rxBuf+4, 2);
       Add_to_Answer_IDList(rxId, len);
       return len;
       }
  // Error
  Serial.print(F("Error ReadExpectedBytesAndCRC "));
  if (rxId == 0xFFFFFFFF)
        Serial.println(F("Timeout"));
  else if (rxId == 0xFFFFFFFE)
        {
        Serial.println(F("Aborted by button"));
        return 0xE;
        }
  else if (rxId == 0xFFFFFFFD)
        {
        Serial.println(F("Aborted by serial command"));
        return 0xE;
        }
  else  {
        Serial.println(F("Wrong message receicved"));
        char hexStr[100];
        Add_Message_to_Hex_Buffer(rxId, len, rxBuf, hexStr);
        Serial.println(hexStr);
        }

  return 0;
}

//-----------------------------------------------------
uint16_t updateCRC(uint16_t CRC_acc, uint8_t CRC_input)
//-----------------------------------------------------
{
  #define POLY 0x1021
  // Create the CRC "dividend" for polynomial arithmetic (binary arithmetic with no carries)
  CRC_acc = CRC_acc ^ (CRC_input << 8);
  // "Divide" the poly into the dividend using CRC XOR subtraction CRC_acc holds the
  // "remainder" of each divide. Only complete this division for 8 bits since input is 1 byte
  for (int i = 0; i < 8; i++) {
    // Check if the MSB is set (if MSB is 1, then the POLY can "divide" into the "dividend")
    if ((CRC_acc & 0x8000) == 0x8000) {
      // if so, shift the CRC value, and XOR "subtract" the poly
      CRC_acc = CRC_acc << 1;
      CRC_acc ^= POLY;
    }
    else {
      // if not, just shift the CRC value
      CRC_acc = CRC_acc << 1;
    }
  }
  // Return the final remainder (CRC value)
  return CRC_acc;
}

//-----------------------------------------------------------------------------------------------------------------------------
int8_t Read_Config_File_from_CAN_Sub(CAN_CLASS &can, const char *ConfName, byte* &Buffer, uint32_t &size, unsigned long AnswId)
//-----------------------------------------------------------------------------------------------------------------------------
// Attention: This function is time critical because the CAN messages are sent very fast (0.5ms period).
//
// Receive a config file from the CAN bus.
// ConfName defines the requested data (See "cs2CAN-Protokoll-2_0.pdf" page 46)
// Example:
//   "lokliste"
//   "loknamen0 10"
//   "lokinfo~038 193-9 DB"             A '~' is replaced by '\0'
//
// The receicved data are crc checked
// The Buffer must be freed outside with
//   free(buffer);
// Return:
//    1: OK
//  < 0: Error
//   -1: CRC Error
//   -2: Timeout beim lesen der Länge und CRC  (5 Sek)
//   -3: Timeout beim lesen des Datenblocks    (TIMEOUT_CONF_LINE = 500 ms)
//   -4: Nicht genügend Speicher vorhanden
//   -5: Aborted by button
// The function waits up to 5 seconds if no data is received.
{
  // Message Filter:
  // ~~~~~~~~~~~~~~~
  // Die Antworten kommen auf einer Message welche mit 0042 beginnt (Config Data Stream).
  // Bei der MS2 sind die folgenden 4 bytes der Message die Kennung der MS2.
  // Bei CS2/3 wird die Kennung der Anfrage benutzt (siehe #define Hash 0302).
  // Das ist aber nicht bei jeder Anfrage so. Die Config Files 9-17
  // ("loks", "mags"  , "gbs", "gbs-#", "fs", "lokstat, "magstat", "gbsstat", "fsstat")
  // Werden von der CS2 auf ihre eigenen Message ID beantwortet.
  // => Die Lokanfragen werden mit der AnswId 0x0303 = Hash gemacht und der Filter so gesetzt,
  //    dass nur diese Messages durchkommen.
  // Bei den Anfragen welche zu Testzwecken von Serial_commands.h kommen wird der Filter so gesetzt,
  // dass alle Messages duchkommen.
  // Problematisch wird es wenn zwei MS2 an einer Gleisbox hängen. Die mit der niedrigeren
  // Seriennummer wird zum Master. Aber wie finde ich heraus welche das ist?
  // => Das Konzept mit dem Message Filter geht nicht mit zwei MS2.
  // Es aber benötigt wenn eine CS2 und eine MS2 angeschlossen sind. Da die CS2 auf der MsgId
  // des fragenden antwortet kommt es nicht zu kollisionen mit den Config Anfragen der MS2
  if (AnswId)
       {
       AnswId = (AnswId & 0x0000FFFF) | 0x00420000;
       Set_Message_Filter_to_single_Message(&can, 0x00FFFFFF, AnswId, 1);
       }
  else Set_Message_Filter_to_single_Message(&can, 0x00FF0000, 0x00420000, 1);   // Without AnswId

  uint8_t len;
  byte Buf[8];
  const char *p = ConfName;
  while (*p) // Send the Request messages
    {
    for (uint8_t i = 0; i < 8; i++, p++)
        {
        if (*p == '~')
             Buf[i] = 0;
        else Buf[i] = *p;
        if (*p == 0) break;
        }
    can.sendMsgBuf(IdReq, 1, 8, Buf);
    if (*p) ContProc_Delay(50);
    }

  // *** Receiver the data ***
  uint32_t ExpectedBytes, ReceivedBytes = 0;
  uint16_t ExpectedCRC;
  uint8_t Len;
  unsigned long FirstReceivedId;
  Len = ReadExpectedBytesAndCRC(can, ExpectedBytes, ExpectedCRC, FirstReceivedId);
  if (Len == 0)   return -2;   // Timeout beim lesen der Länge und CRC
  if (Len == 0xE) return -5;   // Aborted by button

  Buffer = (byte*)malloc(ExpectedBytes+7+20); // 20 reserve bytes
  if (Buffer == NULL) { Dprintf_Mae_Conf("Error: Not enough memory\n");  return -4; } // Nicht genügend Speicher vorhanden

  unsigned long EndTime = millis() + TIMEOUT_CONF_LINE, rxId;
  byte *dp = Buffer;
  uint8_t First = 1;
  while (ReceivedBytes < ExpectedBytes && millis() < EndTime )
      {
      if (can.checkReceive() == CAN_MSGAVAIL)
           {
           if (can.readMsgBuf(&rxId, &len, Buf) == CAN_OK)
              {
              if (len == 8 && rxId == FirstReceivedId) // Don't know how to figure out the master => We use ID from the first received message
                 {
                 if (First) { Add_to_Answer_IDList(rxId, len); First = 0;}
                 EndTime = millis() + TIMEOUT_CONF_LINE;
                 memcpy(dp, Buf, 8);
                 dp += 8;
                 ReceivedBytes += 8;
                 }
              }
           }
      WDT_RESET();
      }
  if (ReceivedBytes < ExpectedBytes)  { Dprintf_Mae_Conf("Timeout\n");   return -3; }

  // *** Check CRC ***
  uint32_t CheckBytes;
  if (Len == 6)
       {
       CheckBytes = (ExpectedBytes+7) / 8;  // Messages with 6 bytes check all bytes in
       CheckBytes *= 8;                     // the CAN messages, not only the used bytes
       }                                    // Feature or bug?
  else CheckBytes = ExpectedBytes;
  uint16_t CRC_acc = 0xFFFF;
  for (uint32_t i = 0; i < CheckBytes; i++)
    {
    CRC_acc = updateCRC(CRC_acc, Buffer[i]);
    }

  //Dprintf_Mae_Conf("ExpectedCRC %04X, CRC %04X\n", ExpectedCRC, CRC_acc);
  if (CRC_acc != ExpectedCRC)  { Dprintf_Mae_Conf("CRC Error\n");        return -1; }

  size = ExpectedBytes;
  Buffer[size] = '\0'; // Just in case the data don't contain an end
  return 1;
}


//-----------------------------------------
uint8_t Is_Bin(byte *Buffer, uint32_t size)
//-----------------------------------------
{
  uint8_t Bin = 0;
  for (uint32_t i = 0; i < size-1; i++)
      {
      if (Buffer[i] > 0 && Buffer[i] < 32 && Buffer[i] != 10 && Buffer[i] != 13)
         {
         Bin = 1;
         break;
         }
      }
  return Bin;
}

#if 0 // Stream based function
// Mit Dieser Funktion sollen die ZLIB Daten konvertiert werden ohne dass man den ganzen                      // 23.02.22:
// Speicher für den Dekomprimierten Block benötigt.
// Die Daten sollen Blockweise aus der Funktion kommen....
// Aber das Funktioniert noch nicht.
// Aus Irgend einem Grund geht auch die alte Funktion (unten) nicht mehr
// Am 26.1.22 hat es noch funktioniet. Dann wurde die Datei "Read loks von CS2.txt" erzeugt.
// => Untersuchen
//
// Die Funktion basiert auf "example3.c" von miniz-2.2.0
//-------------------------------------------------------
int8_t UnZip_if_Compressed(byte* &Buffer, uint32_t &size)
//-------------------------------------------------------
// Return:
//   0: Unable to decompress: Not ZLib compressed
//   1: ASCII file
//   2: decompressed ZLib file
{
#if USE_LIBZ
  typedef unsigned int uint;

  if (Is_Bin(Buffer, size))
       {
       #define BUF_SIZE 1024
       byte *s_outbuf = (byte*)malloc(BUF_SIZE+1);
       if (s_outbuf == NULL ) { Dprintf_Mae_Conf("Error allocating %li bytes for the uncompressed buffer\n", BUF_SIZE); return 0; }
       z_stream stream;
       // Init the z_stream
       mz_ulong Offset = 4; // Die ersten 4 Bytes im Block sind die unkompimierte Größe
       memset(&stream, 0, sizeof(stream));
       stream.next_in   = Buffer+Offset;
       stream.avail_in  = size - Offset;      // The whole input block
       stream.next_out  = s_outbuf;
       stream.avail_out = BUF_SIZE;

       int Res = inflateInit(&stream);
       if (Res) { Dprintf("inflateInit() failed %i!\n", Res); free(s_outbuf); return 0;}

       for ( ; ; )
         {
         int status = inflate(&stream, Z_SYNC_FLUSH);
         if ((status == Z_STREAM_END) || (!stream.avail_out))
            {
            // Output buffer is full, or decompression is done, so write buffer to output file.
            uint n = BUF_SIZE - stream.avail_out;
            s_outbuf[n] = '\0'; // We have allocated one additional byte
            Serial.print((char*)s_outbuf);
            stream.next_out = s_outbuf;
            stream.avail_out = BUF_SIZE;
            }

         if (status == Z_STREAM_END)
              break;
         else if (status != Z_OK) { Dprintf("inflate() failed with status %i!\n", status); free(s_outbuf); return 0; }
         } // end for

       free(s_outbuf);
       if (inflateEnd(&stream) != Z_OK) { Dprintf("inflateEnd() failed!\n"); return 0; }
       }

#endif // USE_LIBZ
  return 0;
}

#else // Old function which allocates the whole decompression memory at once
//-------------------------------------------------------
int8_t UnZip_if_Compressed(byte* &Buffer, uint32_t &size)
//-------------------------------------------------------
// Return:
//   0: Unable to decompress: Not ZLib compressed
//   1: ASCII file
//   2: decompressed ZLib file
{
#if USE_LIBZ
  if (Is_Bin(Buffer, size))
       {
       uint8_t *pUncomp;
       mz_ulong uncomp_len;
       mz_ulong uncomp_Buf_Size = ReadUIntN(Buffer, 4) + 1; // Die ersten 4 Bytes im Block sind die unkompimierte Größe
       if (uncomp_Buf_Size > 1000 * size) Dprintf_Mae_Conf("Looks like a uncompressed binary file \n");
       //Dprintf_Mae_Conf("Uncomp Size:%li\n", ReadUIntN(Buffer, 4)); delay(10); // Debug
       pUncomp = (mz_uint8 *)malloc((size_t)uncomp_Buf_Size+20); // 20 Reserve bytes
       if (pUncomp == NULL)
            {
            Dprintf_Mae_Conf("Error allocating %li bytes for the uncompressed buffer\n", uncomp_Buf_Size);
            return 0;
            }
       else {
            mz_ulong Offset = 4;
            int Res = uncompress(pUncomp, &uncomp_len, Buffer+Offset, size-Offset);
            if (Res == Z_OK)
                 {
                 Dprintf_Mae_Conf("-- Uncompressed the buffer %li -> %li --\n\n", size, uncomp_len + 1);
                 pUncomp[uncomp_len] = '\0';
                 byte *Tmp = Buffer;
                 Buffer = pUncomp;
                 free(Tmp);
                 size = uncomp_len + 1; // +1 Tailing '\0'
                 return 2;
                 }
            else {
                 Dprintf_Mae_Conf("Error uncompressing: ");
                 if (Res == -4)
                      Dprintf_Mae_Conf("Out of mem\n");
                 else Dprintf_Mae_Conf("%i\n", Res);
                 free(pUncomp);
                 return 0;
                 }
            }
       }

#endif // USE_LIBZ
  return 1;
}
#endif

//-------------------------------------------------------------------------------------------------------------------------
int8_t Read_Config_File_from_CAN(CAN_CLASS &can, const char *ConfName, byte* &Buffer, uint32_t &size, unsigned long AnswId)
//-------------------------------------------------------------------------------------------------------------------------
// Return:
//  < 0: in case of an error
//   -1: CRC Error
//   -2: Timeout beim lesen der Länge und CRC  (5 Sek)
//   -3: Timeout beim lesen des Datenblocks    (TIMEOUT_CONF_LINE = 500 ms)
//   -4: Nicht genügend Speicher vorhanden
//   -5: Abbuch duch Tastendruck
//    0: if an binary file was read which could not be decompressed
//    1: if an ascii file read
//    2: if an ZLib file was read and decompressed to ASCII
{
  Set_Message_Filter_to_single_Message(&can, 0x00FF0000, 0x00420000, 1);
  Buffer = NULL;

  int8_t Res = Read_Config_File_from_CAN_Sub(can, ConfName, Buffer, size, AnswId);

  Set_Message_Filter_to_single_Message(&can, 0, 0, 0); // Disable the filter again

  if (Res > 0) Res = UnZip_if_Compressed(Buffer, size);

  if (Res < 0 && Buffer != NULL) // Free the Buffer in case of an error
       {
       free(Buffer);
       Buffer = NULL;
       }

  return Res;
}

//--------------------------------------------------------
void SerialPrintAndSumHexByte(uint8_t Byte, uint16_t &Sum)
//--------------------------------------------------------
{
  if (Byte <= 0x0F) Serial.print('0');
  Serial.print(Byte,HEX);
  Sum += Byte;
}


//---------------------------------------------------------------
void Print_Config_File_ASCII_or_HEX(byte* &Buffer, uint32_t size)
//---------------------------------------------------------------
// Intel Hex Format: https://de.wikipedia.org/wiki/Intel_HEX
{

  if (Is_Bin(Buffer, size))
       {
       // Generate an intel hex file which could be converted outside
       uint8_t BytesPerLine = 0;
       uint16_t Sum;
       uint16_t Adr = 0;
       for (uint32_t i = 0; i < size; i++)
         {
         if (BytesPerLine == 0)
            {
            Sum = 0;
            Serial.print(':');
            SerialPrintAndSumHexByte(size - i > 16?0x10:size-i, Sum); // Byte count
            SerialPrintAndSumHexByte(Adr>>8,   Sum); // Adr
            SerialPrintAndSumHexByte(Adr&0xFF, Sum); // Adr
            SerialPrintAndSumHexByte(0x00,     Sum); // Typ
            Adr+=16;
            }
         SerialPrintAndSumHexByte(Buffer[i], Sum);
         if (++BytesPerLine >= 16 || i+1 == size)
            {
            uint8_t CheckSum = Sum & 0xFF;
            SerialPrintAndSumHexByte(~CheckSum + 1, Sum);
            BytesPerLine = 0;
            Serial.print('\n');
            }
         }
       Serial.println(":00000001FF");
       }
  else Serial.println((char*)Buffer);
}

//-------------------------------- SPY_CAN_CONFIG_FILE ------------------
#if SPY_CAN_CONFIG_FILE

// ToDo: Irgend was stimmt noch nicht. Wenn längere Blöcke empfangen werden, dann stürtzt die Kiste ab
//       Das tritt dann auf, wenn man éinige mahle in der Lokliste blättert
//       Dem Programm geht der Speicher aus:
//           Memory allocation of 20225 bytes failed at 0x40227ffb: std::__cxx11::basic_string  , std::allocator  >::_M_create(unsigned int&, unsigned int) at /workdir/arena.x86_64/gcc-gnu/xtensa-lx106-elf/libstdc++-v3/include/ext/new_allocator.h line 103
//           Add_Message_to_Spy_Buffer(unsigned long, unsigned char, unsigned char*)

#ifndef DebugBaud
  #error "DebugBaud must be defined if SPY_CAN_CONFIG_FILE is used"
#endif

std::string Spy_Buffer;
std::string Spy_Hex_Buffer;
uint8_t Spy_Enabled = SPY_CAN_CONFIG_FILE -1;  // If SPY_CAN_CONFIG_FILE = 2 the spying is enabled from the start
                                               // Otherwise the "spy" command has to be send on the serial port

#define INDENT  "\t\t\t\t\t   "


//-----------------------------------------------------------------------
void Add_Message_to_Spy_Buffer(unsigned long rxId, byte len, byte *rxBuf)
//-----------------------------------------------------------------------
{
  // 12345 0042131C 8: 0A 20 2E 77 65 72 74 3D   // Len:41
  char s[20];
  memset(s, 0, sizeof(s));
  char *p;
  if (len == 8) // Decode only 8 bytes long messages to ASCII
     {
     p = s;
     for (uint8_t i = 0; i < 8; i++)
       {
       if (rxBuf[i] == '\0')                       *p = '~';
       else if (rxBuf[i] != '\n' && rxBuf[i] < 32) *p = '.';
       else                                        *p = (char)rxBuf[i];
       p++;
       }
     Spy_Buffer += s;
     }

  #if SPY_CAN_CONFIG_FILE_HEX
    char hex[100];
    Add_Message_to_Hex_Buffer(rxId, len, rxBuf);
    Spy_Hex_Buffer += hex;
  #endif
}

//-------------------------------------------------------------------------
void Add_Msg_Id_to_SpyBuffer(unsigned long rxId, byte len, const char *Msg)
//-------------------------------------------------------------------------
{
  Spy_Buffer += Msg;
  char s[50];
  sprintf(s, "%08lX len:%i received\n", rxId & 0x7FFFFFFF, len);
  Spy_Buffer += s;
  Spy_Hex_Buffer += Msg;
  Spy_Hex_Buffer += '\n';
}

#define SPY_READ_REQUEST  1
#define SPY_READ_ANSWER   2


//-------------------------------------------------------------------
void Start_Spy_Config_File(unsigned long rxId, byte len, byte *rxBuf)
//-------------------------------------------------------------------
// Is called if a request Message 0x40 is received in the CAN_Server()
{
  if (!Spy_Enabled) return ;
  Start_Spy = millis();
//Set_Message_Filter_to_single_Message(&can, 0x00FC0000, 0x00420000, 0);  // Read 0x40 - 0x43
//  Set_Message_Filter_to_single_Message(&can, 0x003C0000, 0x00420000, 0);  // Read 0x40 - 0x43,  0x80 - 0x83, 0xC0 - 0xC3
  Spy_Buffer = "";
  Spy_Hex_Buffer = "";
  Add_Msg_Id_to_SpyBuffer(rxId, len, INDENT "Request Message:");
  Add_Message_to_Spy_Buffer(rxId, len, rxBuf);
  uint8_t Msg_Typ = (rxId >> 16) & 0xFF;
  if (Msg_Typ != 0x84) // Debug messages are sent very fast => No OLED display possible
     {         //123456789123
      Write_OLED("\23\14"
                 "Spying CAN\n"
                 "to RS232\n"
                 "Please wait\n\21"
                 "(Baud %i)", DebugBaud);
     }
  uint32_t TimeOut = millis() + 2000;
  uint8_t Mode = SPY_READ_REQUEST;
  long unsigned BlockSize;
  uint16_t ExpCRC;
  uint16_t MsgCnt = 1;
  while (millis() < TimeOut)
    {
    if (CAN_is_ok && (can.checkReceive() == CAN_MSGAVAIL))
       {
       if (can.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK)
          {
          //if (rxId & 0x80000000) Dprintf("Msg %08X %i:", rxId & 0x7FFFFFFF, len); else Dprintf("Msg     %04X %i:", rxId); for (uint8_t i = 0; i < len; i++) Dprintf(" %02X", rxBuf[i]); Dprintf("\n"); // Debug
          TimeOut = millis() + 2000;
          MsgCnt++;
          Msg_Typ = (rxId >> 16) & 0xFF;
          switch (Msg_Typ)
            {
            case 0x40:  // Request Message
                        if (Mode != SPY_READ_REQUEST)
                            {
                            Add_Msg_Id_to_SpyBuffer(rxId, len, "\n\n" INDENT "Reqest Message:");  // New Block
                            Mode = SPY_READ_REQUEST;
                            }
                        Add_Message_to_Spy_Buffer(rxId, len, rxBuf); // Add message to
                        break;
            case 0x42:  // Answer Message
                        if (Mode != SPY_READ_ANSWER)
                            {
                            Add_Msg_Id_to_SpyBuffer(rxId, len, "\n\n" INDENT "Answer Message:");  // New Block
                            Mode = SPY_READ_ANSWER;
                            }
                        if (len == 6 || len == 7) // Message with BlockSize and CRC
                             {
                             BlockSize =  ReadUIntN(rxBuf,   4);
                             ExpCRC    =  ReadUIntN(rxBuf+4, 2);
                             char s[100];
                             sprintf(s, INDENT "BlockSize: %lu, CRC %04X\n", BlockSize, ExpCRC);
                             Spy_Buffer += s;
                             Add_Message_to_Spy_Buffer(rxId, len, rxBuf);
                             }
                        else Add_Message_to_Spy_Buffer(rxId, len, rxBuf); // Normal Message
                        break;
            case 0x84:  // Debug outputs ?
                        Add_Message_to_Spy_Buffer(rxId, len, rxBuf); // Normal Message
                        break;
            }
          }
       }
    WDT_RESET();
    }
  #if SPY_CAN_CONFIG_FILE_HEX
    Serial.println(Spy_Hex_Buffer.c_str());
    Serial.println("------------------------- ASCII -------------------\n");
  #endif
  Serial.println(Spy_Buffer.c_str());
  Dprintf("\nSpy End duration %li ms\n\n", millis() - Start_Spy);
  Write_OLED("\23\14"
             "End Spying\n"
             "Received %i\n"
             "messages\n\21", MsgCnt);
  Set_Message_Filter_to_single_Message(&can, 0, 0, 0); // Disable the filter again
}
#endif // SPY_CAN_CONFIG_FILE


#endif // USE_CAN
