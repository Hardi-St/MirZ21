#include "Beep.h"


#define DM_DISPLAY_IR_CHANNELS  0
#define DM_SHOW_MEASUREMENTS    1
#define DM_SHOW_TV_IR_LOK       2
#define DM_SHOW_TV_IR_SWITCH    3
uint8_t  DisplayMode     = DM_DISPLAY_IR_CHANNELS;

uint32_t NextScrUpd_Time = 0; // Screen update time stamp

//-------------- TV IR control ----------------------
int16_t IR_Nr = 0;
uint16_t IR_Nr_DigitNr = 0;
#define  IR_NR_TYP_INVALID     0
#define  IR_NR_TYP_SWITCHNR    1
#define  IR_NR_TYP_FUNCKEY     2
#define  IR_NR_TYP_LOK_NR      3
uint8_t  IR_Nr_Typ = IR_NR_TYP_INVALID;
uint32_t IR_NR_Timeout         = 0;
uint32_t Last_IR_Key_PressTime = 0;
int16_t  IR_Switch_Nr          = 0;           // Weichennummer von der IR Fernsteuerung (0..319)
int16_t  IR_Switch_Nr_Disp1    = 0;
uint8_t  IR_Power_Nr           = 0;
uint8_t  FktKeyCursFlashState  = 1;
uint8_t  Loco_DispMode         = 0;   // 1: Märklin IR control, 3: TV IR control, 21: z21, 33: CAN
uint8_t  Ma_FuncKeySelect      = 0;
uint8_t  Ma_FuKy_Max           = 8;
uint8_t  TV_FuncKeySelect      = 0;
uint8_t  TV_FuKy_Max           = 2;
int16_t  TV_Lok_Nr             = 0;
int16_t  Ex_Lok_Nr             = 0;
uint8_t  Act_Ma_Lok_Ix         = 0;
int16_t  Old_Ma_Lok_Ix         = 0xFFFF;
bool     Show_Temp_Display     = 0;    // Is set to 1 if a temporary display is shown.
uint32_t LastIRKeyTime         = 0;

#define PRINT_IR_DEBUG_MESSAGES
#ifdef PRINT_IR_DEBUG_MESSAGES
   // F() makro saves dynamic memory
   #define IR_Dprintf(Format, ...) Dprintf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#else
   #define IR_Dprintf(Format, ...)  {}
#endif

/*
//-----------------------------------
bool Get_SwitchPos(uint16_t SwitchNr)
//-----------------------------------
// SwitchNr 1..320
{
  SwitchData_t d;
  d.b = EEPROM.read(SWITCHDATABYTES_EEPROM + (SwitchNr/2));
  if (SwitchNr % 2)
       return d.Pos1;
  else return d.Pos0;
}

//---------------------------------------------
void Set_SwitchPos(uint16_t SwitchNr, bool Pos)
//---------------------------------------------
// SwitchNr 1..320
{
  SwitchData_t d;
  d.b = EEPROM.read(SWITCHDATABYTES_EEPROM + (SwitchNr/2));
  if (SwitchNr % 2)
       d.Pos1 = Pos;
  else d.Pos0 = Pos;
  EEPROM.update(SWITCHDATABYTES_EEPROM + (SwitchNr/2), d.b);
}

//-----------------------------------
void Set_Accessory_State(byte *rxBuf)
//-----------------------------------
// Set the Accessory state if a CAN message is received
{
  uint16_t SwitchNr =  (rxBuf[2]<<8) + rxBuf[3];
  if (SwitchNr >= 0x3800 && SwitchNr <= 0x3FFF) // DCC
     {
     SwitchNr -= 0x3800;
     if (rxBuf[5]) // Current > 0
        {
        Set_SwitchPos(SwitchNr, rxBuf[4]);
        }
     }
}

//------------------------
void Define_Switch_Chars()
//------------------------
//             128   129      130   131        132   133      134   135      Maximal 8 user defined characters are possible
//           +-----+-----+  +-----+-----+     +-----+-----+  +-----+-----+
//  b[0]     |     |     |  |     |     |     |     |     |  |     |     |
//  b[1]     |  ###|  # #|  |  # #|  ###|     |# #  |###  |  |###  |# #  |
//  b[2]     |  ###| # # |  |  # #| ### |     | # # |###  |  | ### |# #  |
//  b[3]     |  ###|# #  |  |  # #|###  |     |  # #|###  |  |  ###|# #  |
//  b[4]     |  ###| #   |  |  # #|##   |     |   # |###  |  |   ##|# #  |
//  b[5]     |  ###|#    |  |  # #|#    |     |    #|###  |  |    #|# #  |
//  b[6]     |  ###|     |  |  # #|     |     |     |###  |  |     |# #  |
//  b[7]     |  ###|     |  |  # #|     |     |     |###  |  |     |# #  |
//           +-----+-----+  +-----+-----+     +-----+-----+  +-----+-----+
{
  char b[9];
  b[0] = B10000000;
  b[8] = '\0';

  memset(b+1, B10000111, 7);
  Write_OLED("\33%c%s",128, b); // Program the character

  b[1] =    B10000101;
  b[2] =    B10001010;
  b[3] =    B10010100;
  b[4] =    B10001000;
  b[5] =    B10010000;
  b[6] =    B10000000;
  b[7] =    B10000000;
  Write_OLED("\33%c%s",129, b); // Program the character

  memset(b+1, B10000101, 7);
  Write_OLED("\33%c%s",130, b); // Program the character

  b[1] =    B10000111;
  b[2] =    B10001110;
  b[3] =    B10011100;
  b[4] =    B10011000;
  b[5] =    B10010000;
  b[6] =    B10000000;
  b[7] =    B10000000;
  Write_OLED("\33%c%s",131, b); // Program the character

  b[1] =    B10010100;
  b[2] =    B10001010;
  b[3] =    B10000101;
  b[4] =    B10000010;
  b[5] =    B10000001;
  b[6] =    B10000000;
  b[7] =    B10000000;
  Write_OLED("\33%c%s",132, b); // Program the character

  memset(b+1, B10011100, 7);
  Write_OLED("\33%c%s",133, b); // Program the character

  b[1] =    B10011100;
  b[2] =    B10001110;
  b[3] =    B10000111;
  b[4] =    B10000011;
  b[5] =    B10000001;
  b[6] =    B10000000;
  b[7] =    B10000000;
  Write_OLED("\33%c%s",134, b); // Program the character

  memset(b+1, B10010100, 7);
  Write_OLED("\33%c%s",135, b); // Program the character
}
*/

//-----------------------
void Show_Switch_On_LCD()
//-----------------------
{
// +--------------------+
// |Weichen:   +100     |
// |14 15 16 17 18 19 20|
// | 1  0  1  1  0  0  0|
// |    ^               |
// +--------------------+
/*
  Define_Switch_Chars();
*/
  Write_OLED("\1Weichen:   ");
/*
  if (IR_Switch_Nr_Disp1 < IR_Switch_Nr -6) IR_Switch_Nr_Disp1 = IR_Switch_Nr -6;
  if (IR_Switch_Nr_Disp1 > IR_Switch_Nr)    IR_Switch_Nr_Disp1 = IR_Switch_Nr;
  if (IR_Switch_Nr_Disp1 < 0) IR_Switch_Nr_Disp1 = 0;
  if (IR_Switch_Nr >= 99)
       Write_OLED("+%i00", ((IR_Switch_Nr+1)/100));
  else Write_OLED("    ");
  Write_OLED("     ");
  for (uint8_t i = 0; i < 7; i++)
    Write_OLED("%2i ", (IR_Switch_Nr_Disp1+i+1) % 100);
  Write_OLED("\10"); // Delete last char
  // Bei Märklin ist Rot = 0 = Abbiegen
  // Auf unserer Anlage ist 0 = Gerade
  // Wenn die 16:9 Taste lange festgehalten wird, dann kann die
  // Weichendarstellung zurückgesetzt werden. Dann wird man gefragt
  // ob man die Inverse oder normale Darstellung für alle Weichen benutzen
  // will. Später kann man das für jede Weiche individuell festlegen.

  char SwitchChars[] = "\202\203" // 130, 131: switch right, turn
                       "\200\201" // 128, 129: switch right, straight
                       "\206\207" // 130, 131: switch left, turn
                       "\204\205" // 128, 129: switch left, straight
                       " 0"
                       " 1"
                       " r"
                       " g";
  for (uint8_t i = 0; i < 7; i++)
      {
      SwitchData_t d;
      int16_t SwitchNr = IR_Switch_Nr_Disp1+i;
      d.b = EEPROM.read(SWITCHDATABYTES_EEPROM + (SwitchNr/2));
      uint8_t Disp, Pos;
      if (SwitchNr % 2)
           { Disp = d.Disp1; Pos = d.Pos1;}
      else { Disp = d.Disp0; Pos = d.Pos0;}
      //if (i+IR_Switch_Nr_Disp1 == IR_Switch_Nr) Dprintf("Disp=%i d.Pos0=%i d.Pos1=%i   ", Disp, d.Pos0, d.Pos1);  // Debug
      char *p = &SwitchChars[(Disp>>1)*4];
      uint8_t Inv = (Disp & 0x01) > 0;

      if (Inv ^ Pos) p+=2;
      //if (i+IR_Switch_Nr_Disp1 == IR_Switch_Nr) Dprintf("Disp=%i Inv=%i, Pos=%i Inv^Pos=%i\n", Disp, Inv, Pos, Inv ^ Pos); // Debug
      Write_OLED("%c%c ", *p, *(p+1));
      }
  Write_OLED("\10"); // Delete last char

  for (uint8_t i = 0; i < 7; i++)
      Write_OLED(" %c%c", i+IR_Switch_Nr_Disp1 == IR_Switch_Nr ? '^':' ', i<6?' ':'\0');

 */
}

//--------------------------
void Enable_Switch_Display()
//--------------------------
{
  DisplayMode = DM_SHOW_TV_IR_SWITCH;
  NextScrUpd_Time = 0;
}

//-----------------------
void Change_Switch_Disp()
//-----------------------
// Rotate the displays for the actual switch
{
/*
  int16_t SwitchNr = IR_Switch_Nr;
  SwitchData_t d;
  d.b = EEPROM.read(SWITCHDATABYTES_EEPROM + (SwitchNr/2));
  uint8_t Disp;
  if (SwitchNr % 2)
       Disp = d.Disp1;
  else Disp = d.Disp0;
  Dprintf("SwitchNr %i  Change_Switch_Disp %i ->", SwitchNr, Disp);
  Disp++;
  if (SwitchNr % 2)
       d.Disp1 = Disp & 0x07;
  else d.Disp0 = Disp & 0x07;
  Dprintf("%i\n", Disp);
  EEPROM.update(SWITCHDATABYTES_EEPROM + (SwitchNr/2), d.b);

  Enable_Switch_Display();
  Beep_Single_Key();
*/
}


//----------------------------
bool IR_YesNo(const char *Msg)
//----------------------------
{
  Beep_Single_Key();
  Write_OLED("\14%s\r", Msg);
  Write_OLED("Ja=OK, Sonst nein");
  uint32_t End = millis() + 1000;
  uint16_t Duration = 0;

  while (millis() < End) // Ignore the first keys
    Read_IR_Key_and_Flash_LED(Duration);

  End = millis() + 10000;
  while (millis() < End)
    {
    IR_Key_t key = Read_IR_Key_and_Flash_LED(Duration);
     if (key != IR_NO_KEY)
        {
        switch (key & IR_ANY_KEY)
           {
           case IR_KEY_TV_YES:  return 1;
           default:             return 0;
           }
        }
    }

  while (millis() < End) // Clear the keys
    Read_IR_Key_and_Flash_LED(Duration);
  return 0;
}

//----------------------
void Clear_switch_Disp()
//----------------------
{
/*
             // 12345678901234567890
  if (IR_YesNo("Weichen Konfig.\r"
               "zuruecksetzen?"))
     {                    // 12345678901234567890
     uint8_t Inv = IR_YesNo("Weichen Darstellung\r"
                            "invertieren?\r"
                            "(Rot=Gerade)");
     Dprintf("Inv = %i\n", Inv);
     if (Inv) Inv = B00100010;
     Beep(50);
     for (uint16_t i = 0; i < SWITCHDAT_BYTES; i++)
        {
        EEPROM.update(SWITCHDATABYTES_EEPROM + i, Inv);
        }
     }
  else Beep(300);
  Enable_Switch_Display();
*/
}

//----------------------------------------------------------
void drawCursorVLine(uint16_t x0, uint16_t y0, uint16_t len)
//----------------------------------------------------------
{
  #if 0
    display.drawVLine(x0, y0, len);
  #else // dashed line
    uint16_t ye = y0 + len;
    uint16_t DashLen = 2;
    for (uint16_t y = y0; y < ye; y+= 2*DashLen)
        {
        if (y+DashLen > ye) DashLen = ye-y;
        display.drawVLine(x0, y, DashLen);
        }
  #endif
}

//-------------------
uint16_t Get_Lok_Nr()
//-------------------
{
  uint16_t Nr;
  switch (Loco_DispMode) // 1: Märklin IR control, 3: TV IR control, 21: z21, 33: CAN
    {
    case 1:  Nr = Read_Ma_Lok_Nr_from_EEPROM(Act_Ma_Lok_Ix);
             if (Nr >= Lok_Cnt)
                {
                Nr = Lok_Cnt -1;
                Write_Ma_Lok_Nr_to_EEPROM(Act_Ma_Lok_Ix, Nr);
                }
             break;
    case 21: // Z21
    case 33: Nr = Ex_Lok_Nr; // CAN
             if (Nr >= Lok_Cnt) Nr = Ex_Lok_Nr = Lok_Cnt  -1;
             break;
    default: Nr = TV_Lok_Nr;
             if (Nr >= Lok_Cnt) Nr = TV_Lok_Nr = Lok_Cnt  -1;
    }
  return Nr;
}

//-----------------------------------------------------------------------
int8_t Get_FuncKeys_and_Set_FuKy_Max(uint16_t Lok_Nr, uint8_t FktTyp[32])
//-----------------------------------------------------------------------
// Returns the number of the last used funktion
//  -1 if no function is used
// In Addition the variable TV_FuKy_Max and Ma_FuKy_Max are set
{
  Read_FktTyp_from_EEPROM(Lok_Nr, FktTyp);
  int8_t LastUsedFkt = -1;
  for (uint8_t i = 0; i < 32; i++)
    {
    if ((FktTyp[i] & 0x7F) > 0) LastUsedFkt = i;
    //Dprintf("Fkt[%i] = %i\n", i, FktTyp[i]);
    }

  TV_FuKy_Max = LastUsedFkt / 12;

  if (LastUsedFkt > 23)
       Ma_FuKy_Max = 6 + (LastUsedFkt-24) / 2; // The last group has 2 entries
  else Ma_FuKy_Max = LastUsedFkt / 4;
  if (Ma_FuKy_Max > 8) Ma_FuKy_Max = 8;

  return LastUsedFkt;
}

//-------------------------------
void Display_Single_Loko_on_LCD()
//-------------------------------
// +----------------+
// | 12: DCC...     |  Status line
// |Rotfuchs        |  Loco Name
// |> ####          |  Speed amd direction
// |0 1 2 3  234567 |
// |4 5 6 7  890123 |  30 Function key icons
// |8 9 0 1  456789 |
// +----------------+
{
  //*** Status line ***
  uint16_t Int_Lok_Nr = Get_Lok_Nr();
  char Name[LOK_NAME_LEN+1];
  Read_Lok_Name_from_EEPROM(Int_Lok_Nr, Name);
  char NrStr[10], TypStr[5], Adr_or_uid_Str[30] = "";
  sprintf(NrStr, "%i:", Int_Lok_Nr+1);
  uint16_t DCC = Read_Lok_DCC_from_EEPROM(Int_Lok_Nr);
  uint16_t Z21Adr = Read_Lok_Z21Adr_from_EEPROM(Int_Lok_Nr);
  uint16_t uid = Z21Adr_to_uid(Z21Adr);
  Write_OLED("\25" "\14" "\37"); // verry small font 5, cls, don't show the screen now


  char CtrlMode[10];
  switch (Loco_DispMode)
    {
    case 1:  sprintf(CtrlMode, "Mä%i", Act_Ma_Lok_Ix+1);  break;
    case 3:  strcpy(CtrlMode,  "TV");; break;
    case 21: strcpy(CtrlMode,  "z21"); break;
    default: strcpy(CtrlMode,  "CAN"); break;
    }
  display.setCursor(113,6);
  display.print(CtrlMode);
  #if 1  // Show Z21Adr
     if (uid <= 0x03ff)      strcpy(TypStr, "MM");
     else if (uid <= 0x7FFF) strcpy(TypStr, "MFX");
     else                    strcpy(TypStr, "DCC");
     sprintf(Adr_or_uid_Str, "z.%-4i d.%-4i", Z21Adr, DCC);
  #else  // Show uid if MFX
     if (uid <= 0x03ff)      { strcpy(TypStr, "MM");  }
     else if (uid <= 0x7FFF) { strcpy(TypStr, "MFX"); sprintf(Adr_or_uid_Str, "uid:%04X %-5i", uid, DCC); }
     else                    { strcpy(TypStr, "DCC"); }
     if (*Adr_or_uid_Str == '\0') sprintf(Adr_or_uid_Str, "Adr:%-5i", DCC);
  #endif
  display.setCursor(0, 6);
  Write_OLED("%-5s%-3s %s\24\37", NrStr, TypStr, Adr_or_uid_Str); // '\24' = Font 4  (Achtung keine Zeichen mit Unterlänge wie "y" verwenden)


  display.drawUTF8(0, 18, Name);  // *** Loco name ***


  // *** Speed bar ***
  {
  uint y = 20;
  uint32_t Speed = Var_Lok_Data[Int_Lok_Nr].Speed;
  if (Speed > 1000) Speed = 1000;
  Speed = (Speed*127)/1000;
  if (Speed > 0) display.drawBox(0, y, Speed, 7);

  display.setFontMode(1);   // Transparent text
  display.setDrawColor(2);  // XOR
  Display_setTextSize(5);
  char DirStr[2] = "<";
  if (Var_Lok_Data[Int_Lok_Nr].Direction != 2) *DirStr = '>';
  uint8_t x = 0, w = display.getMaxCharWidth()+1;
  do {
     for (uint8_t i = 0; i < 3; i++) display.drawStr(i+x, y+6, DirStr); // Print the "<" character 3 times to make it Bold
     x += 6;
     } while (x+w < Speed);
  display.setDrawColor(1);  // Normal
  display.setFontMode(0);   // Overwrite text
  }

  // *** Draw the function symbols ***
  uint8_t FktTyp[32];
  int8_t LastUsedFkt = Get_FuncKeys_and_Set_FuKy_Max(Int_Lok_Nr, FktTyp);
  Display_setTextSize(10);  // Select the "font_Fkt_Icons" font
  uint8_t y0 = 38;          // First y position
  uint8_t dx = 12, dy = 12; // Symbol width, heihgt +1
  uint8_t x1 = 2, lh, yl, ye, x1L = x1, Blk = 3;

  if (     LastUsedFkt <= 3)  { x1 = 8; dx = 30; y0 += 4;           Blk = 1; } // One row  in the center
  else if (LastUsedFkt <= 7)  { x1 = 8; dx = 30; y0 += 4;  dy += 6; Blk = 1; } // Two rows in the center
  else if (LastUsedFkt <= 11) { x1 = 8; dx = 30;                    Blk = 1; } // Three rows
  else if (LastUsedFkt <= 23) {         dx = 15;                    Blk = 2; } // Three rows, two columns

  uint8_t IR_FuncKeyLine = 0, IR_FuncKeyBlock = 0;
  switch (Loco_DispMode)
     {
     case 1:  // Märklin
              if (Ma_FuncKeySelect > Ma_FuKy_Max) Ma_FuncKeySelect = 0;
              if (Ma_FuKy_Max == 0) FktKeyCursFlashState = 0;  // No Cursor
              IR_FuncKeyLine  = Ma_FuncKeySelect % 3;
              IR_FuncKeyBlock = Ma_FuncKeySelect / 3;
              break;
     case 3:  // TV remote control
              if (TV_FuncKeySelect > TV_FuKy_Max) TV_FuncKeySelect = 0;
              if (TV_FuKy_Max == 0)  FktKeyCursFlashState = 0; // No Cursor
              IR_FuncKeyLine  = 0;
              IR_FuncKeyBlock = TV_FuncKeySelect;
              break;
     default: // z21 or CAN
              FktKeyCursFlashState = 0; // No Cursor
              break;
     }
  uint8_t dx4 = dx*4-1, x2 = x1+dx4+3, x3=x2+dx4+3;
  if (Blk == 1) x2 -= 8;
  if (Blk == 2) x2 -= 2;

  if (((++FktKeyCursFlashState) % 2) == 0) // Flashing
     {
     yl = y0-9 + IR_FuncKeyLine * dy;
     lh = Loco_DispMode*dy-1;
     ye = yl + lh -1;
     switch (IR_FuncKeyBlock)                    // Draw brackets
       {                                         //   _    _
       case 0: display.drawHLine(x1L-2, yl, 2);  //  | xxxx | xxxx   xx
               display.drawHLine(x2-3,  yl, 2);  //  | xxxx | xxxx   xx
               display.drawHLine(x1L-2, ye, 2);  //  | xxxx |
               display.drawHLine(x2-3,  ye, 2);  //  '-    -'
               break;                            //          _    _
       case 1: display.drawHLine(x2-2,  yl, 2);  //    xxxx | xxxx | xx
               display.drawHLine(x3-3,  yl, 2);  //    xxxx | xxxx | xx
               display.drawHLine(x2-2,  ye, 2);  //    xxxx | xxxx | xx
               display.drawHLine(x3-3,  ye, 2);  //         '-    -'
               break;                            //                 _  _
       case 2: display.drawHLine(x3-2,  yl, 2);  //    xxxx   xxxx | xx |
               display.drawHLine(125,   yl, 2);  //    xxxx   xxxx | xx |
               display.drawHLine(x3-2,  ye, 2);  //    xxxx   xxxx | xx |
               display.drawHLine(125,   ye, 2);  //                '-  -'
               break;
       }
     // Vertical lines
     if (IR_FuncKeyBlock == 0) drawCursorVLine(0,    yl+1, lh);
     if (IR_FuncKeyBlock <= 1) drawCursorVLine(x2-2, yl+1, lh);
     if (IR_FuncKeyBlock >= 1) drawCursorVLine(x3-2, yl+1, lh);
     if (IR_FuncKeyBlock == 2) drawCursorVLine(126,  yl+1, lh); // One spare column => FN 30, 31 ?
     }

  uint32_t Func = Var_Lok_Data[Int_Lok_Nr].FuncKeys; // All function states stored as bit mask
  uint32_t Mask = 1;
  uint8_t x = x1, y = y0;
  for (uint8_t i = 0; i <= 31; i++)
    {
    uint16_t AscNr = (FktTyp[i] & 0x7F) *2; // AscNr = Number of the enabled icon. The bit 0x80 is set if it's a momentary key.
    if ((Func & Mask)==0) AscNr--; // The disabled icon is located before the enabled symbol (1= First disabled icon, 2= First enabled icon)
    if (i <= 29)
         {
         if (AscNr > 0) display.drawGlyph(x, y, AscNr); // The drawStr function is interpreting some control characters like "tab"
         x += dx;
         switch (i)
             {
             case 3:  case 7:  y += dy; x = x1; break;  // Positions of the icons
             case 11:          y  = y0; x = x2; break;  // 0  1  2  3   12 13 14 15   24 25
             case 15: case 19: y += dy; x = x2; break;  // 4  5  6  7   16 17 18 19   26 27
             case 23:          y  = y0; x = x3; break;  // 8  9 10 11   20 21 22 23   28 29
             case 25: case 27: y += dy; x = x3; break;
             }
         }
    else { // Function keys 30 and 31
         if ((Func & Mask)) display.drawVLine(127, i==30?y0-dy+4:y0+4, dy-3);  // The last two funktion are only shown as line because the display is to small ;-(
         }                                                                     // That's np problen since DCC is only supporting 28 function keys
    Mask = Mask << 1;
    }
  Display_display();   // Send the data to the screen
}

//-------------------------------
void Enable_Single_Loco_Display()
//-------------------------------
{
  DisplayMode     = DM_SHOW_TV_IR_LOK;
  if (!Show_Temp_Display)
     NextScrUpd_Time = 0;
  Show_Temp_Display = 0;
}


void Set_IR_Power(int8_t Mode); // Forward definition

//----------------------------------------------------------
void Set_Ma_IR_Channel(uint8_t Channel, uint8_t EnablePower)
//----------------------------------------------------------
{
  Loco_DispMode = 1;
  Enable_Single_Loco_Display();
  Old_Ma_Lok_Ix = Act_Ma_Lok_Ix;
  Act_Ma_Lok_Ix = Channel;
  if (EnablePower && Railpower != csNormal) Set_IR_Power(1);
}

//----------------------------------------
void ChangeAct_Ma_Lok_Nr(int8_t ChangeDir)
//----------------------------------------
{
  Beep_Single_Key();
  Dprintf("ChangeAct_Ma_Lok_Nr %i\n", ChangeDir);
  if (ChangeDir > 0)
       if (Read_Ma_Lok_Nr_from_EEPROM(Act_Ma_Lok_Ix)+1 >= Lok_Cnt)
            Write_Ma_Lok_Nr_to_EEPROM(Act_Ma_Lok_Ix, 0);
       else Write_Ma_Lok_Nr_to_EEPROM(Act_Ma_Lok_Ix, Read_Ma_Lok_Nr_from_EEPROM(Act_Ma_Lok_Ix)+1);
  else {
       if (Read_Ma_Lok_Nr_from_EEPROM(Act_Ma_Lok_Ix) == 0)
            Write_Ma_Lok_Nr_to_EEPROM(Act_Ma_Lok_Ix, Lok_Cnt -1);
       else Write_Ma_Lok_Nr_to_EEPROM(Act_Ma_Lok_Ix, Read_Ma_Lok_Nr_from_EEPROM(Act_Ma_Lok_Ix)-1);
       }
}

//-----------------------------------------
void SetAct_Ma_Lok_Nr_in_Range(int8_t Mode)
//-----------------------------------------
{
  Beep_Single_Key();
  switch (Mode)
    {
    case 1:  Write_Ma_Lok_Nr_to_EEPROM(Act_Ma_Lok_Ix,  0);            break; // First loco
    case 2:  Write_Ma_Lok_Nr_to_EEPROM(Act_Ma_Lok_Ix,  Lok_Cnt / 4);  break; // 1/4
    case 3:  Write_Ma_Lok_Nr_to_EEPROM(Act_Ma_Lok_Ix,  Lok_Cnt / 2);  break; // 1/2
    default: Write_Ma_Lok_Nr_to_EEPROM(Act_Ma_Lok_Ix, (Lok_Cnt / 4) * 3);    // 3/4
    }
}

//---------------------------
void Select_Ma_Loko_Display()
//---------------------------
// Auswahl der Lok für die Märklin IR Fernsteuerung
// .-------------------------.
// |Auswahl IR Lok/Schalter  |  Kleine Schrift
// |Cora                     |
// |Lolo                     |  Aktive Lok mit Ramen
// |Blaufuchs                |
// |Rolf                     |
// '-------------------------'
// - Auswahl mit Licht Taste
// - Erste Lok mit F1, F2 = 1/4 der Liste, F3 = 1/2, F3 = 3/4
// - Abbruch Stop oder mit Taste an TV Fernbedinung
{
  Beep_Deep(50);  // Anderen Ton?
  IR_Key_Input.Clear();
  uint8_t EndDisplay = 0, UpdateDisp = 1;
  int16_t Old_Ma_Lok_Nr[4];
  uint8_t y0 = 18, dy = 14;
  for (uint8_t i = 0; i < 4; i++) Old_Ma_Lok_Nr[i] = Read_Ma_Lok_Nr_from_EEPROM(i);
  uint32_t Timeout = 5000;
  uint32_t EndTime = millis() + Timeout;
  while (!EndDisplay && millis() < EndTime)
    {
    if (UpdateDisp)
       {
       Write_OLED("\25" "\14" "\37"); // verry small font 5, cls, don't show the screen now
       display.setCursor(0, 6);
       Write_OLED("Auswahl IR Lok/Schalter\24\37");  // \24: Font 8x13, \37: Dont draw the display direct
       for (uint8_t i = 0; i < 4; i++)
           {
           char Name[LOK_NAME_LEN+1];
           uint8_t y = y0 + i*dy;
           display.drawUTF8(2, y, Read_Lok_Name_from_EEPROM(Read_Ma_Lok_Nr_from_EEPROM(i), Name));  // *** Loco name ***   11.07.22:  Old: Write_OLED(...)
           }
       display.drawRFrame(0, y0 - 12 + Act_Ma_Lok_Ix*dy, 128, 15,2);  // Rahmen um aktuelle Lok
       Write_OLED("");
       }

    uint16_t Duration = 0;
    IR_Key_t key = Read_IR_Key_and_Flash_LED(Duration);
    if (key != IR_NO_KEY)
       {
       EndTime = millis() + Timeout;
       uint8_t Same_IR_Ctrl = 0;
       UpdateDisp = 1;
       Set_Ma_IR_Channel(IR_CHANNEL(key), 0);
       switch (key & IR_ANY_KEY)
          {
          case IR_KEY_M_PRESS_MINUS:
          case IR_KEY_M_HOLD_MINUS:  ChangeAct_Ma_Lok_Nr(-1);      break;
          case IR_KEY_M_PRESS_PLUS:
          case IR_KEY_M_HOLD_PLUS:   ChangeAct_Ma_Lok_Nr(+1);      break;
          case IR_KEY_M_PRESS_F1:    SetAct_Ma_Lok_Nr_in_Range(1); break;
          case IR_KEY_M_PRESS_F2:    SetAct_Ma_Lok_Nr_in_Range(2); break;
          case IR_KEY_M_PRESS_F3:    SetAct_Ma_Lok_Nr_in_Range(3); break;
          case IR_KEY_M_PRESS_F4:    SetAct_Ma_Lok_Nr_in_Range(4); break;
          case IR_KEY_M_PRESS_LIGHT: if (Act_Ma_Lok_Ix == Old_Ma_Lok_Ix)  EndDisplay = 1; // Select. If the switch was changed just update the display
                                     Beep(50);
                                     break;
          case IR_KEY_M_PRESS_STOP:  Same_IR_Ctrl = 1; // no break
          default:                   if (key & IR_KEY_PRESSED) // Other key pressed => Abort the display
                                        {
                                        if (!Same_IR_Ctrl) // Aborted by button from TV IR control
                                           Loco_DispMode = 3;
                                        EndDisplay = 2;
                                        }
          }
       }
    else UpdateDisp = 0;
    delay(10); // Watchdog                                                                                    // ToDo: ContProc? damit sich die Z21 App nicht abmeldet
    }

  if (EndDisplay != 1) // Abort by timeout or other key
     {
     Beep_Deep(100);
     for (uint8_t i = 0; i < 4; i++) Write_Ma_Lok_Nr_to_EEPROM(i, Old_Ma_Lok_Nr[i]); // Restore the old locos
     }
  IR_Key_Input.Clear();
}


//-------------------------------
bool TV_Number_Is(uint8_t Nr_Typ)
//-------------------------------
{
  if (IR_Nr_Typ == Nr_Typ || (millis() < IR_NR_Timeout && IR_Nr_DigitNr > 0))  // At least one digit entered
       {
       IR_Nr_Typ = Nr_Typ;
       Beep(50);
       IR_NR_Timeout = millis() - 100; // Don't use the entered number a second time, but the following function should get it.
       return 1;
       }
  else { // Timeout or wrong IR_Nr_Typ
       Beep_Deep(100); // Error sound
       return 0;
       }
}

//----------------------
bool TV_Number_Entered()
//----------------------
{
  if (millis() < IR_NR_Timeout && IR_Nr_DigitNr > 0)  // At least one digit entered
       {
       Beep(50);
       IR_NR_Timeout = millis() + 100; // Don't use the entered number a second time, but the following function should get it.
       return 1;
       }
  else { // Timeout or wrong IR_Nr_Typ
       return 0;
       }
}

#define CHECK_NR_ONLY 0

//---------------------------------------------------------------------------
void IncDecOrCheck_Nr(int16_t &Nr, int8_t Mode, int16_t MinNr, int16_t MaxNr)
//---------------------------------------------------------------------------
// Mode:
//  1 = Inc
// -1 = Dec
//  0 = Only check
// In Check mode the nearest number is used in case of a range error
{
  Nr += Mode;
  if (Nr < MinNr) Nr = Mode==0 ? MinNr : MaxNr;
  if (Nr > MaxNr) Nr = Mode==0 ? MaxNr : MinNr;
}


//-----------------------------------------------------------------
void IncDecOrCheck_IR_Nr(int8_t Mode, int16_t MinNr, int16_t MaxNr)
//-----------------------------------------------------------------
// Mode:
//  1 = Inc
// -1 = Dec
//  0 = Only check
{
  IncDecOrCheck_Nr(IR_Nr, Mode, MinNr, MaxNr);
}

//-----------------------------------------------
void Set_TV_Switch(int8_t IncMode, int8_t SwMode)
//-----------------------------------------------
// IncMode:
//  1 = Inc
// -1 = Dec
//  0 = Only check
// SwMode:
//  0 = Red
//  1 = Green
// -1 = Toggle
// -2 = Nothing or 2x toggle
{
  Dprintf("Set_TV_Switch IncMode %i, SwMode %i %i %i\n", IncMode, SwMode, millis() < IR_NR_Timeout,  IR_Nr_DigitNr);
/*
  if (millis() < IR_NR_Timeout && IR_Nr_DigitNr > 0)
       {
       IR_Switch_Nr = IR_Nr-1;
       Dprintf("Set_TV_Switch %i\n", IR_Switch_Nr);
       // No Beep(50) here because it's generated in TV_Number_Entered()   Indicate that the number was read
       IR_Nr_DigitNr = 0; // Don't use the entered number a second time
       }
  else Beep_Single_Key(); // Indicate that the key was accepted without entered number

  IncDecOrCheck_Nr(IR_Switch_Nr, IncMode, 0, MAX_SWITCH-1);

  bool Pos;
  switch (SwMode)
    {
    case  0:
    case  1: Set_SwitchPos(IR_Switch_Nr, Pos = SwMode);
             break;
    case -1: Set_SwitchPos(IR_Switch_Nr, Pos = !Get_SwitchPos(IR_Switch_Nr));
             break;
    }
  Send_DCC_Accessory(IR_Switch_Nr, Pos);

  Enable_Switch_Display();

  #if defined(  PRINT_IR_DEBUG_MESSAGES) && 0
    IR_Dprintf("Switch %3i: ", IR_Switch_Nr+1);
    switch (SwMode)
      {
      case  0: IR_Dprintf("Red\n");     break;
      case  1: IR_Dprintf("Green\n");   break;
      case -1: IR_Dprintf("Toggle\n");  break;
      case -2: IR_Dprintf("Nothing\n"); break;
      default: IR_Dprintf("Unknown SwMode %i\n", SwMode);
      }
  #endif
  */
}

//----------------------------
void Set_IR_Power(int8_t Mode)
//----------------------------
// Mode -1 = Toggle
//       0 = Off
//       1 = On
{
/* Alte Funktionen der MS2_Bridge mit der die verschiedenen MS2 selektiv geschaltet werden können
   Das werde ich vielleicht später wieder einbauen.


  if (TV_Number_Entered())
       {
       IncDecOrCheck_IR_Nr(CHECK_NR_ONLY, 0, CAN_CNT);
       IR_Power_Nr = IR_Nr;
       }
  else Beep_Single_Key();
*/

  const char *Txt;
  uint8_t NewState;
  if (Mode != 0 && (Mode == 1 || Railpower != csNormal))
       {
       Txt = "On";
       NewState = csNormal;
       Beep(100);
       }
  else {
       Txt = "Off\n\21"
             "\n"
             "1. Send Halt 1 sec\n"
             "2. Turn power off";
       NewState = csTrackVoltageOff;
       Beep_Deep(300);
       }
  Write_OLED("\22\14\rPower %s", Txt);
  notifyz21_RailPower(NewState);
  globalPower(NewState);

/* Alte Funktionen der MS2_Bridge

  bool SendStop = 0;
  uint8_t First = 0;
  uint8_t Last = CAN_CNT;

  if (IR_Power_Nr == 0)
       {
       // Wenn ein Kanal an ist, dann werden alle ausgeschaltet
       // Sonst werden alle eingeschaltet
       for (uint8_t i = 0; i < CAN_CNT; i++)
          {
          if (Get_GB_Power_Enable(i))
             {
             SendStop = 1;
             break;
             }
          }
       }
  else {
       SendStop = Get_GB_Power_Enable(IR_Power_Nr-1);
       First = IR_Power_Nr-1;
       Last  = IR_Power_Nr;
       }
  if (SendStop)
       Write_OLED("Aus");
  else Write_OLED("An");
  if (First+1 != Last)
       Write_OLED(" alle");
  else Write_OLED(" %i", IR_Power_Nr);


  for (uint8_t i = First; i < Last; i++)
      {
      if (SendStop)
           {
           Clear_GB_Power_Enable(i);
           Send_Stopp_to_CAN_Nr(i);
           }
      else {
           Set_GB_Power_Enable(i);
           Send_Go_to_CAN_Nr(i);
           }
      }
 */
  NextScrUpd_Time = millis() + 3000; // Switch back to normal display in xx ms
  Show_Temp_Display = 1;
}

typedef struct
    {
    uint16_t Adr;
    uint8_t  Pos:1;
    } AccessoryList_T;

#define ACCESSORYLIST_CNT  5
AccessoryList_T AccessoryList[ACCESSORYLIST_CNT];
uint8_t used_AccListPos = 0;

#define CLEARACCLIST_PERIOD  10000   // Disable the line if the lis should not be cleared after a while
#ifdef CLEARACCLIST_PERIOD
  uint32_t ClearAccList = 0;
#endif

//----------------------------------------------------------------
void Display_Accessory(uint16_t Adr, uint8_t Pos, uint8_t Current)                                            // 14.07.22:
//----------------------------------------------------------------
{
  if (Current)
     {
     #ifdef CLEARACCLIST_PERIOD
       if (millis() > ClearAccList)
          used_AccListPos = 0;
       ClearAccList = millis() + CLEARACCLIST_PERIOD;
     #endif

     if (used_AccListPos >= ACCESSORYLIST_CNT)
        {
        memmove(AccessoryList, AccessoryList+1, sizeof(AccessoryList_T)*(ACCESSORYLIST_CNT-1));
        used_AccListPos--;
        }
     AccessoryList_T *p = &AccessoryList[used_AccListPos];
     p->Adr = Adr;
     p->Pos = Pos;
     used_AccListPos++;
     Write_OLED("\22\14Accessory\n\21\37");
     p = &AccessoryList[0];
     if (used_AccListPos == 1)
          Write_OLED("\n\n\22Adr:%i %i", p->Adr+1, p->Pos);
     else {
          for (uint8_t i = 0; i < used_AccListPos; i++, p++)
             Write_OLED(" Adr:%5i   %i%s", p->Adr+1, p->Pos, i+1 < used_AccListPos?"\n\37":"");
          }
     NextScrUpd_Time = millis() + 5000; // Switch back to normal display in xx ms
     Show_Temp_Display = 1;
     }
}



//------------------------------------
void Set_TV_Lok_Direction(int8_t Mode)
//------------------------------------
//  1 = Forward
//  2 = Backward
//  3 = Toggle direction
{
  uint16_t Int_Lok_Nr = Get_Lok_Nr();

  IR_Dprintf("Lok_Dir: %c\n", Mode==1?'>': (Mode==2 ? '<':'t'));
  if (Mode == 3)
     {
     if (Var_Lok_Data[Int_Lok_Nr].Direction == 1)
          Mode = 2;
     else Mode = 1;
     }
  if (Var_Lok_Data[Int_Lok_Nr].Direction != Mode)
     {
     Var_Lok_Data[Int_Lok_Nr].Speed = 0;
     Send_Cmd_From_IR_to_Lok_by_Ix(0x08, Int_Lok_Nr, 6, 0); // Speed);
     }
  Var_Lok_Data[Int_Lok_Nr].Direction = Mode;
  Send_Cmd_From_IR_to_Lok_by_Ix(0x0A, Int_Lok_Nr, 5, Var_Lok_Data[Int_Lok_Nr].Direction);
  Beep_Single_Key();
  Enable_Single_Loco_Display();
}


//--------------------------------
void Set_TV_Lok_Speed(int8_t Mode)
//--------------------------------
//  0 = Stop
//  1 = Increment
// -1 = Dec
{
  //IR_Dprintf("Lok_Speed %c\n", Mode == 1 ? '+' : (Mode == -1 ? '-' : '0'));
  uint16_t Int_Lok_Nr = Get_Lok_Nr();
  int16_t Speed = Var_Lok_Data[Int_Lok_Nr].Speed;
  if (Mode == 0)
       Speed = 0;
  else Speed += Mode*5;
  if (Speed>1000) Speed = 1000;
  if (Speed<0)    Speed = 0;
  Var_Lok_Data[Int_Lok_Nr].Speed = Speed;
  Send_Cmd_From_IR_to_Lok_by_Ix(0x08, Int_Lok_Nr, 6, Speed);
  if (abs(Mode) <= 1)
       Beep_Single_Key();
  else Beep_Repeat_Key();
  Enable_Single_Loco_Display();
}

//--------------------------
void Set_TV_Emergency_Stop()
//--------------------------
{
  IR_Dprintf("Emergency Stop\n");
  Set_IR_Power(0);
  //Send_Stop_to_All_Beep();
}

// Die Funktionstasten können entweder mit der Märklin IR Fernbedienung
// oder mit der TV IR Fernbedienung gesteuert werden. Bei Märklin Steuerung
// gibt es nur 4 Funktionstasten (F1-F4). Umgeschaltet wird über die Licht Taste.
// Bei der TV Fernbedienung gibt es 3 Reihen a 4 Tasten für die Funktionen.
// Die Variable Loco_DispMode schaltet zwischen den Steuerungen um.
//  1: Märklin
//  3: TV
//  21: Z21  Keinen Einfluss auf die Funktionstasten
//  33: CAN    "
// Das Programm merkt sich den Zustand für beide IR Steuerungen separat.
//  Märklin: Ma_FuncKeySelect 0..8  (Ma_FuKy_Max)
//  TV:      TV_FuncKeySelect 0..2  (TV_FuKy_Max)

//--------------------------------------------
void Switch_FuncKeySelect(uint8_t DisplayOnly)
//--------------------------------------------
{
  FktKeyCursFlashState = 1;
  NextScrUpd_Time = 0; // Update now
  uint8_t BeepEnd, BeepTyp;
  uint8_t FktTyp[32];
  Get_FuncKeys_and_Set_FuKy_Max(Get_Lok_Nr(), FktTyp);
  if (Loco_DispMode == 1)
       { // Märklin
       if (!DisplayOnly) Ma_FuncKeySelect++;
       if (Ma_FuncKeySelect > Ma_FuKy_Max) Ma_FuncKeySelect = 0;
       BeepEnd = Ma_FuncKeySelect / 3;
       BeepTyp = Ma_FuncKeySelect % 3;
       }
  else { // TV
       if (!DisplayOnly) TV_FuncKeySelect++;
       if (TV_FuncKeySelect > TV_FuKy_Max) TV_FuncKeySelect = 0;
       BeepEnd = TV_FuncKeySelect;
       BeepTyp = 2;
       }

  for (uint8_t i = 0; i <= BeepEnd; i++) // Generate the beeps
      {
      switch (BeepTyp)
        {
        case 0:  Beep_High(20); break;
        case 1:  Beep(20);      break;
        default: Beep_Deep(20);
        }
      if (i < BeepEnd) delay(150);
      }

}

//-----------------------------------------------------------------------------------
void Set_TV_FuncKeyDirect(uint8_t KeyNr, uint8_t TransLateBlock, uint8_t KeyReleased)
//-----------------------------------------------------------------------------------
{
  //IR_Dprintf("FuncKey %3i\n", KeyNr);
  //  Key transl for IR_FuncKeyBlock = 2
  //  Here only the first two function key columns are visible
  //   ______________
  //  | 0=>24  1=>25 |  2>=30  3=99
  //  | 4=>26  5=>27 |  6>=31  7=99
  //  | 8=>28  9=>29 | 10>=99 11=99
  uint8_t KeyTrans[] = { 24, 25, 30, 99, 26, 27, 31, 99, 28, 29, 99, 99};
  if (TransLateBlock) // Translate the keys
     {
     if (Loco_DispMode == 1)
          { // Märklin IR control
          if (Ma_FuncKeySelect < 6)
               KeyNr += Ma_FuncKeySelect*4;
          else KeyNr = KeyTrans[KeyNr+(Ma_FuncKeySelect-6)*4];
          }
     else { // TV IR control
          switch (TV_FuncKeySelect)
            {
            case 0: break; // No translation
            case 1: KeyNr += 12; break;
            case 2: KeyNr = KeyTrans[KeyNr];
                    break;
            }
          }
     }

  if (KeyNr == 99) return ;
  uint16_t Int_Lok_Nr = Get_Lok_Nr();

  if (KeyReleased)
    {
    uint8_t FktTyp[32];
    Read_FktTyp_from_EEPROM(Int_Lok_Nr, FktTyp);
    if ((FktTyp[KeyNr] & 0x80) == 0) return ; // No momentary function => Don't disable the function when the button is released
    }

  uint32_t Mask = 1;
  Mask = Mask << KeyNr;
  uint32_t FuncKeys = Var_Lok_Data[Int_Lok_Nr].FuncKeys;
  uint16_t Par = KeyNr*256;
  if (FuncKeys & Mask || KeyReleased)
       { FuncKeys &= ~Mask; }
  else { FuncKeys |=  Mask; Par++; }
  Var_Lok_Data[Int_Lok_Nr].FuncKeys = FuncKeys;
  Send_Cmd_From_IR_to_Lok_by_Ix(0x0C, Int_Lok_Nr, 6, Par);
  Enable_Single_Loco_Display();
}


//------------------------------------------------------------------------
void Set_Loc_Function(uint8_t Channel, uint8_t KeyNr, uint8_t KeyReleased)
//------------------------------------------------------------------------
{
  Set_Ma_IR_Channel(Channel, 1);
  Set_TV_FuncKeyDirect(KeyNr - 1, 1, KeyReleased);
  if (!KeyReleased) Beep_Single_Key();
}

//--------------------------------------------------------------
void Chg_Speed(uint8_t Channel, int8_t Direction, int8_t Repeat)
//--------------------------------------------------------------
{
  Set_Ma_IR_Channel(Channel, 1);
  Set_TV_Lok_Speed(Direction * (Repeat?5:1));
}



//---------------------------------------------------
void Set_TV_FuncKey(int8_t Mode, uint8_t KeyReleased)
//---------------------------------------------------
{
  IncDecOrCheck_IR_Nr(Mode, 0, 31);
  Set_TV_FuncKeyDirect(IR_Nr, 0, KeyReleased);
}

//--------------------------------
void Set_IR_Lok_Nr(int8_t Mode)
//--------------------------------
// Mode:
//  1: Increment
// -1: Decrement
//  0: Set from IR_Nr
{
  Loco_DispMode = 3;
  if (Mode == 0)
       TV_Lok_Nr = IR_Nr-1;
  else Beep_Single_Key();
  IncDecOrCheck_Nr(TV_Lok_Nr, Mode, 0, Lok_Cnt-1);
  IR_Dprintf("Lok Nr %3i\n", TV_Lok_Nr+1);
  Enable_Single_Loco_Display();
}

//---------------------------------------------------
void Proc_TV_IR_Keys(IR_Key_t key, uint16_t Duration)
//---------------------------------------------------
{
  uint8_t Channel = IR_CHANNEL(key);
  LastIRKeyTime = millis();

  if (Channel == 15 && (key & IR_KEY_PRESSED))                                                                // ToDo: Braucht man hier die Channel abfrage
     {
     uint32_t DeltaT = millis() - Last_IR_Key_PressTime;
     Last_IR_Key_PressTime = millis();
     // Zwischen zwei Tasten sollten 300 ms Abstand sein
     // Manchmal kommt eine Taste versehentlich zwei mal innerhalb von 206 ms
     if (DeltaT < 300) return ;
     //IR_Dprintf("%8lims ", DeltaT);  // Debug
     }
  //Debug_Print_Grundig_Keys(key);
  // Dprintf("Proc_TV_IR_Keys %i\n", key);

  uint8_t Old_Loco_DispMode;
  int8_t Mode = 0;
  switch (key & IR_ANY_KEY)
     {
     //***************** TV IR control **************************
     case IR_KEY_TV_PRESS_NR0:
     case IR_KEY_TV_PRESS_NR1:
     case IR_KEY_TV_PRESS_NR2:
     case IR_KEY_TV_PRESS_NR3:
     case IR_KEY_TV_PRESS_NR4:
     case IR_KEY_TV_PRESS_NR5:
     case IR_KEY_TV_PRESS_NR6:
     case IR_KEY_TV_PRESS_NR7:
     case IR_KEY_TV_PRESS_NR8:
     case IR_KEY_TV_PRESS_NR9:       // Read several digits and create a number
                                     {
                                     uint8_t Digit = (key & IR_ANY_KEY) - IR_KEY_TV_PRESS_NR0;
                                     if (millis() > IR_NR_Timeout) IR_Nr_DigitNr = 0;
                                     if (IR_Nr_DigitNr == 0) IR_Nr = 0;
                                     IR_Nr_DigitNr++;
                                     IR_Nr =IR_Nr * 10 + Digit;
                                     IR_NR_Timeout = millis() + 3000;
                                     IR_Nr_Typ = IR_NR_TYP_INVALID;
                                     Beep_Single_Key();
                                     }
                                     break;
     case IR_KEY_TV_SWITCH_GREEN:    Mode =  2;     // Green  =  1     No break
     case IR_KEY_TV_SWITCH_TOGGLE:   Mode -= 1;     // TOGGLE = -1     No break
     case IR_KEY_TV_SWITCH_RED:      // Set Switch  // Red    =  0
                                     Set_TV_Switch(CHECK_NR_ONLY, Mode);
                                     break;
     case IR_KEY_TV_SWITCH_CNG_DISP: if (DisplayMode == DM_SHOW_TV_IR_SWITCH) Change_Switch_Disp(); break;
     case IR_KEY_TV_SWITCH_RST_DISP: if (DisplayMode == DM_SHOW_TV_IR_SWITCH) Clear_switch_Disp();  break;
     case IR_KEY_TV_FUNCKEY:         Old_Loco_DispMode = Loco_DispMode;
                                     Loco_DispMode = 3;
                                     if (TV_Number_Entered())
                                          Set_TV_FuncKey(CHECK_NR_ONLY, 0);     // Set Function key by number
                                     else Switch_FuncKeySelect(Loco_DispMode != Old_Loco_DispMode);
                                     break;
     case IR_KEY_TV_RELEASED_FUNCKEY:Set_TV_FuncKey(CHECK_NR_ONLY, 1);          // Set Function key release by number
                                     break;
     case IR_KEY_TV_TRACKPOWER:      Set_IR_Power(-1);
                                     break;
     case IR_KEY_TV_NEXT_SWITCH:     Mode = 2;// Mode =  1
     case IR_KEY_TV_PREV_SWITCH:     Mode--;  // Mode = -1
                                     if (TV_Number_Entered())
                                          Set_TV_Switch(CHECK_NR_ONLY, -2);                                   // Switch selection by number
                                     else Set_TV_Switch(Mode, -2);                                            // Prev/Next Switch
                                     break;
#ifdef IR_KEY_TV_LOK_NR
     case IR_KEY_TV_LOK_NR:          if (TV_Number_Is(IR_NR_TYP_LOK_NR))   Set_IR_Lok_Nr(CHECK_NR_ONLY);      // Set Lok number
                                     break;
#endif
     case IR_KEY_TV_NEXT_LOK:        Mode = 2;  // No break
     case IR_KEY_TV_PREV_LOK:        Mode--;
                                     if (TV_Number_Entered())
                                          Set_IR_Lok_Nr(CHECK_NR_ONLY);                                       // Loc selection by number
                                     else Set_IR_Lok_Nr(Mode);                                                // Prev/Next Lok
                                     break;
   #if TV_STOP_CHANGE_DIR == 1
     case IR_KEY_TV_STOP_FUNC:       Mode++;  // Mode = 3 Toggle Dir
   #endif
     case IR_KEY_TV_BACKWARD:        Mode++;  // Mode = 2
     case IR_KEY_TV_FORWARD:         Mode++;
                                     Loco_DispMode = 3;
                                     Set_TV_Lok_Direction(Mode);
                                     break;

     case IR_KEY_TV_FASTER:          Mode = 2; //  1
     case IR_KEY_TV_SLOWER:          Mode--;   // -1
   #if TV_STOP_CHANGE_DIR == 0
     case IR_KEY_TV_STOP_FUNC:
   #endif
                                     Loco_DispMode = 3;
                                     Set_TV_Lok_Speed(Mode);
                                     break;
     case IR_KEY_TV_HOLD_FASTER:     Loco_DispMode = 3; Set_TV_Lok_Speed( 5); break;
     case IR_KEY_TV_HOLD_SLOWER:     Loco_DispMode = 3; Set_TV_Lok_Speed(-5); break;

     case IR_KEY_TV_FN11:            Mode++;
     case IR_KEY_TV_FN10:            Mode++;
     case IR_KEY_TV_FN9:             Mode++;
     case IR_KEY_TV_FN8:             Mode++;
     case IR_KEY_TV_FN7:             Mode++;
     case IR_KEY_TV_FN6:             Mode++;
     case IR_KEY_TV_FN5:             Mode++;
     case IR_KEY_TV_FN4:             Mode++;
     case IR_KEY_TV_FN3:             Mode++;
     case IR_KEY_TV_FN2:             Mode++;
     case IR_KEY_TV_FN1:             Mode++;
     case IR_KEY_TV_FN0:             Loco_DispMode = 3;
                                     Set_TV_FuncKeyDirect(Mode, 1, 0);
                                     Beep_Single_Key();
                                     break;
     case IR_KEY_TV_RELEASED_FN11:   Mode++;
     case IR_KEY_TV_RELEASED_FN10:   Mode++;
     case IR_KEY_TV_RELEASED_FN9:    Mode++;
     case IR_KEY_TV_RELEASED_FN8:    Mode++;
     case IR_KEY_TV_RELEASED_FN7:    Mode++;
     case IR_KEY_TV_RELEASED_FN6:    Mode++;
     case IR_KEY_TV_RELEASED_FN5:    Mode++;
     case IR_KEY_TV_RELEASED_FN4:    Mode++;
     case IR_KEY_TV_RELEASED_FN3:    Mode++;
     case IR_KEY_TV_RELEASED_FN2:    Mode++;
     case IR_KEY_TV_RELEASED_FN1:    Mode++;
     case IR_KEY_TV_RELEASED_FN0:    Loco_DispMode = 3;
                                     Set_TV_FuncKeyDirect(Mode, 1, 1);
                                     break;
     case IR_KEY_TV_EMERGENCY_STOP:  Set_TV_Emergency_Stop();
                                     break;

     //***************************** Märklin IR control *****************************

     case IR_KEY_M_RELEASED_LIGHT:   Old_Loco_DispMode = Loco_DispMode;
                                     Set_Ma_IR_Channel(Channel, 1);
                                     Switch_FuncKeySelect(Loco_DispMode != Old_Loco_DispMode || Old_Ma_Lok_Ix != Act_Ma_Lok_Ix);
                                     break;
     case IR_KEY_M_HOLD_LIGHT:       Set_Ma_IR_Channel(Channel, 0);
                                     Select_Ma_Loko_Display();
                                     Ma_FuncKeySelect = 0; // Always select the first channel. Thats usefull to reset the selection by
                                                           // holding the Light key an pressing it again => Selection = 0
                                     break;
     case IR_KEY_M_PRESS_F1:
     case IR_KEY_M_PRESS_F2:
     case IR_KEY_M_PRESS_F3:
     case IR_KEY_M_PRESS_F4:         Set_Loc_Function(Channel, IR_KEY(key), 0); break; // Function keys
     case IR_KEY_M_RELEASED_F1:
     case IR_KEY_M_RELEASED_F2:
     case IR_KEY_M_RELEASED_F3:
     case IR_KEY_M_RELEASED_F4:      Set_Loc_Function(Channel, IR_KEY(key), 1); break; // Function keys


     case IR_KEY_M_PRESS_MINUS:
     case IR_KEY_M_HOLD_MINUS:
     case IR_KEY_M_PRESS_PLUS:
     case IR_KEY_M_HOLD_PLUS:      Chg_Speed(Channel, IR_KEY(key) == IR_KEY_M_PLUS ? +1: -1, key & IR_KEY_HOLD);  // Change the speed
                                   break;

     case IR_KEY_M_RELEASED_STOP:  Set_Ma_IR_Channel(Channel, 0); Set_TV_Lok_Direction(3); break; // Lok Direction

     case IR_KEY_M_HOLD_STOP:      IR_Key_Input.Clear(); // Stop generating key messages until the current key is released.
                                   Set_Ma_IR_Channel(Channel, 0);
                                   Set_TV_Emergency_Stop();
                                   break;
     }
}

//--------------------------------------------------------------------
void Display_Ext_Loco_if_possible(uint16_t Lok_Nr, uint8_t Z21_or_CAN)
//--------------------------------------------------------------------
{
  Ex_Lok_Nr = Lok_Nr;
  if ((Loco_DispMode != 1 && (Loco_DispMode != 3)) || millis() - LastIRKeyTime > 10000)
     {
     if (Z21_or_CAN)
          Loco_DispMode = 21;
     else Loco_DispMode = 33;
     Enable_Single_Loco_Display();
     }
}

//-----------------------
void IR_Functions_setup()
//-----------------------
{
}

//----------------------
void IR_Functions_loop()
//----------------------
{
  uint16_t Duration = 0;
  IR_Key_t key = Read_IR_Key_and_Flash_LED(Duration);
  if (key != IR_NO_KEY)
     {
     //Dprintf("IR Key %08X\n", key); // Debug
     Proc_TV_IR_Keys(key, Duration);
     }


  uint32_t t = millis();
  if (t >= NextScrUpd_Time)
     {
     NextScrUpd_Time = t + 500; // Update periodic
     Show_Temp_Display = 0;
     switch (DisplayMode)
       {
       case DM_SHOW_TV_IR_SWITCH:   Show_Switch_On_LCD();
                                    break;
       case DM_SHOW_TV_IR_LOK:      Display_Single_Loko_on_LCD();
                                    break;
       /*
       case DM_DISPLAY_IR_CHANNELS: Print_IR_Lok_Data();
                                    break;
       case DM_SHOW_MEASUREMENTS:   Show_GB_Measurements();
                                    NextScrUpd_Time = t + 1000; // Slower update rate
                                    break;
       */
       }
     }
/*
  switch(DisplayMode)
     {
     case DM_DISPLAY_IR_CHANNELS: if (t >= Cursor_Flash_TO && LastIRNr >= 0)
                                     {
                                     Define_Func_Chr(LastIRNr);
                                     Cursor_Flash = !Cursor_Flash;
                                     Cursor_Flash_TO = t + 500;
                                     }
                                  break;
     case DM_SHOW_MEASUREMENTS:   Request_Measurement_Server(); break;
     }
*/
}
