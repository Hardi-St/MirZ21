//--------------------------------------------------------------
/*
  additional OLED Display 128x64 for Z21PG

  - show system info
  - show power status
  - display IP Address
  - show software information

  Copyright (c) by Philipp Gahtow, year 2022

  Revision History:
  ~~~~~~~~~~~~~~~~~
  21.01.22:  - Added support for 1.3" OLED Display (Activated with OLED_TYP 13)
               Therefore the U8g2lib is used.
             - Added function Write_OLED() which
                - uses printf style: Write_OLED("Hallo %i\n", i);
                - Supports special characters
                  - '\n"  - New line
                  - '\14' = Clear screen
                - automatic strolling if it reaches the end
                - works with any font height
  27.01.22:  -

*/

#if defined(Z21DISPLAY)

boolean DisplayReady = false;
boolean AllowStartDisplay = true;

#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#include <SPI.h>
#include <Wire.h>


#define SCREEN_WIDTH                 128 // OLED display width, in pixels
#define SCREEN_HEIGHT                64  // OLED display height, in pixels
#define DISPLAYLINEHIGHT_SMALL_FONT  9   //Pixel to get the next line (small font)

uint8_t Write_OLED_y = 0;
uint8_t Write_OLED_LineHeight = DISPLAYLINEHIGHT_SMALL_FONT;

#include <U8g2lib.h>                // "U8g2" Library from Oliver Kraus
#if ROTATE_DISPLAY_180_DEG
  #define U8G2_ROT U8G2_R2
#else
  #define U8G2_ROT U8G2_R0
#endif
// U8g2Lib supports different display drivers. The differ in the used RAM. We use the full frame buffer mode.
// => All data for the display fit into the RAM => The content for the display has to be drawn only once
// The other drivers use less RAM => The display has to be drawn several times and written partly to the OLED
// If several parts are used the OLED update time is longer ;-(
#if OLED_TYP == 13
    U8G2_SH1106_128X64_NONAME_F_HW_I2C  display(U8G2_ROT, U8X8_PIN_NONE);  // 1.3"  I2C 128X64  OLED  SH1106   (https://de.aliexpress.com/item/32683739839.html)
#elif OLED_TYP == 96
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_ROT, U8X8_PIN_NONE);  // 0.96" I2C 128X64  OLED  SSD1306  (https://de.aliexpress.com/item/32643950109.html)
#elif OLED_TYP == 7920
   #if defined(USE_HARDWARE_SPI)                                                     //                                   (https://www.amazon.de/dp/B09SLXVH1P) (https://www.amazon.de/AZDelivery-%E2%AD%90%E2%AD%90%E2%AD%90%E2%AD%90%E2%AD%90-Display-Zeichen-Arduino/dp/B07DDKBCY7)
        U8G2_ST7920_128X64_F_HW_SPI     display(U8G2_ROT, U8X8_PIN_NONE, U8X8_PIN_NONE); // No CS(=VCC), no Reset         // R/W = D23 = KEY80 Pin 6 = MLL_EXT_OUT Pin 9,    GND, VCC, V0 = NC, RS = VCC, PSB = GND, BLA = VCC, BLK = GND
   #else // Software SPI is slow ;-(                                                                                      // E   = D18 = MLL_EXT_OUT Pin 7
        U8G2_ST7920_128X64_F_SW_SPI     display(U8G2_ROT, SPI_DISP_CLK,  SPI_DISP_DAT, U8X8_PIN_NONE, U8X8_PIN_NONE);     // No CS(=VCC), no Reset
   #endif                                                                                                                 // GND, VCC, V0 = NC, RS = VCC, R/W = SPI_DISP_DAT, E = SPI_DISP_CLK, ... PSB = GND, BLA = VCC, BLK = GND
#else
   #error "OLED_TYP not supported yet ;-("
 #endif


// Some of the functions have to be adapted to the behavior of the Adafruit_SSD1306 library
#define SSD1306_WHITE                1
#define Display_Begin()              display.begin()
#define Display_display()            display.sendBuffer()
#define Display_setTextColor(Color)  display.setDrawColor(Color);
#define Display_setCursor(x,y)       display.setCursor(x,1+y+DISPLAYLINEHIGHT_SMALL_FONT); // don't use display.getMaxCharHeight() because it must be the height of the prior line

#include "font_Fkt_Icons.bdf.h"
#include "font_Stummilok.bdf.h"
#include "font_Haus.bdf.h"
#include "font_Weichen.bdf.h"

//------------------------------------
void Display_setTextSize(uint8_t Size)
//------------------------------------
// Die CS2 verwendet den UTF-8 Zeichensatz => Die Deutschen Umlaute müssen
// konvertiert werden wenn sie mit einer Windows Cp1252 Schrift angezeigt
// werden sollen. Das macht die Funktion "drawUTF8()"
{
  switch (Size)
     {
//   case 6:  display.setFont(u8g2_font_crox1cb_mf);                break;  //             w:9  h:13
//   case 6:  display.setFont(u8g2_font_unifont_tf               ); break;  //             w:16 h:16  8.00  x 4.00
//   case 4:  display.setFont(u8g2_font_torussansbold8_8r        ); break;  //             w:8  h:7   16.00 x 9.14
//   case 8:  display.setFont(u8g2_font_calibration_gothic_nbp_tf); break;  //             w:15 h:23  8.53  x  2.78
//   case 7:  display.setFont(u8g2_font_crox4h_tf);                 break;  //             w:18 h:24  7.11  x  2.67
//   case 6:  display.setFont(u8g2_font_helvR14_tf);                break;  //             w:18 h:22  7.11  x  2.91
//   case 5:  display.setFont(u8g2_font_logisoso16_tf);             break;  //             w:19 h:23  6.74  x  2.78
//   case 4:  display.setFont(u8g2_font_crox5t_tf);                 break;  //             w:23 h:27  5.57  x  2.37
//   case 5:  display.setFont(u8g2_font_4x6_tf);                    break;  //             w:4  h:6   32.00 x 10.67

     case 13: display.setFont(font_Weichen);                        break;
     case 12: display.setFont(font_Haus);                           break;
     case 11: display.setFont(font_Stummilok);                      break;
     case 10: display.setFont(font_Fkt_Icons);                      break;  // Font for the Icons

     case 5:  display.setFont(u8g2_font_5x7_tf);                    break;  //             w:5  h:7   25.60 x  9.14  Deutsche Umlaute            u8g2_font_5x7_tf.png
     case 4:  display.setFont(u8g2_font_8x13B_mf);                  break;  //             w:8  h:13  16.00 x  4.92  Deutsche Umlaute            u8g2_font_8x13B_mf.png     https://github.com/olikraus/u8g2/wiki/fntgrpx11#8x13b
     case 3:  display.setFont(u8g2_font_crox2cb_mf);                break;  // medium font w:10 h:16  12.80 x  4.00  Griechisch, Keine Umlaute!  u8g2_font_crox2cb_mf.png
     case 2:  display.setFont(u8g2_font_crox3cb_mf);                break;  // large  font w:13 h:20  9.85  x  3.20  Griechisch, Keine Umlaute!  u8g2_font_crox3cb_mf.png
     default: display.setFont(u8g2_font_6x10_tf);                           // small  font w:6  h:10  21.33 x  6.40  Deutsche Umlaute            u8g2_font_6x10_tf.png
                                                                            //   (7 lines if the reduced line height 9 is used)
     }
  if (Size == 1)
       Write_OLED_LineHeight = DISPLAYLINEHIGHT_SMALL_FONT;
  else Write_OLED_LineHeight = display.getMaxCharHeight();
}


//-------------------------
void Display_clearDisplay()
//-------------------------
// Clear display without flicker
{
  uint8_t *p = display.getBufferPtr();
  memset(p, 0, SCREEN_WIDTH*SCREEN_HEIGHT/8);
}


//--------------------------------------------------------------
//remove old writing with a block box
void DisplayClearPart(uint8_t x, uint8_t y, uint8_t width) {
  //x,y,width,hight
  display.setDrawColor(0);
  display.drawBox(x, y+1, width-x, DISPLAYLINEHIGHT_SMALL_FONT);
  display.setDrawColor(1);
  Display_setCursor(x,y);
}

//--------------------------------------------------------------
//update Z21 system info
#if defined(BOOSTER_INT_MAINCURRENT)
void DisplayUpdateRailData(uint16_t inAm, float volt, float temp) {
    DisplayClearPart(80,0, SCREEN_WIDTH);
    display.print(inAm);
    display.print(F("mA"));
    //ESP8266 has only one ADC, so there is only the sense Value!
    #if !defined(ESP8266_MCU)
    DisplayClearPart(80, DISPLAYLINEHIGHT_SMALL_FONT, SCREEN_WIDTH);
    display.print(temp,1);  //one decimal place only!
    display.print((char)247);
    display.print(F("C"));
    DisplayClearPart(40, 0, 80);
    display.print(volt / 1000,1); //one decimal place only!
    display.print(F("V"));
    #endif
  // drawing commands to make them visible on screen!
  Display_display();
}
#endif

void DisplayStart(); // Forward definition

//--------------------------------------------------------------
//update power status
void DisplayUpdateRailPower() {
  if (!AllowStartDisplay) return ;
  if (!DisplayReady) DisplayStart();

  //Railpower: ON, OFF, PROG, SHORT
    DisplayClearPart(40, DISPLAYLINEHIGHT_SMALL_FONT, 80);
    switch (Railpower) {
        case csNormal: display.print(F("-ON-")); break;
        case csTrackVoltageOff: display.print(F("-OFF-")); break;
        case csServiceMode: display.print(F("PROG")); break;
        case csShortCircuit: display.print(F("SHORT")); break;
        case csEmergencyStop: display.print(F("STOP")); break;
        default: display.print(F("-!?!-")); break;
        }
  // drawing commands to make them visible on screen!
  Display_display();
}


//--------------------------------------------------------------
//update S88N Module
#if defined(S88N)
void DisplayUpdateS88N() {
  if (!AllowStartDisplay) return ;
  if (DisplayReady) {
    Display_setTextSize(1);
    DisplayClearPart(0, DISPLAYLINEHIGHT_SMALL_FONT * 2, 60);
    display.print(F("S88:  "));
    display.print(S88Module);
    // drawing commands to make them visible on screen!
    Display_display();
  }
}
#endif  //S88 Module

//--------------------------------------------------------------
//report OTA Update on Display!
#if (defined(ESP8266_MCU)  || defined(ESP32_MCU)) && defined(ESP_OTA)
void DisplayOTAStart() {
  // Clear the buffer
  Display_clearDisplay();
  Display_setCursor(0,0);             // Start at top-left corner
  display.print(F("OTA Update..."));
  // drawing commands to make them visible on screen!
  Display_display();
}

void DisplayOTAProgress(uint8_t state) {
  DisplayClearPart(0, 35, SCREEN_WIDTH);
  display.printf("Progress: %u%%", state);
  // drawing commands to make them visible on screen!
  Display_display();
}

void DisplayOTAFinish() {
  Display_setCursor(0,50);             // Start at top-left corner
  display.print(F("FINISH"));
  // drawing commands to make them visible on screen!
  Display_display();
}
#endif //OTA Messages

//--------------------------------------------------------------
//global Information
void DisplayGobal() {
  //prepare fix data:
  Display_setTextSize(1);
  Display_setCursor(0,0);             // Start at top-left corner
  display.print(F("Z21PG"));
  Display_setCursor(0,DISPLAYLINEHIGHT_SMALL_FONT);             // Start at top-left corner
  display.print(F("v"));
  display.print(String(Z21mobileSwVer).substring(0,1));
  display.print(".");
  display.print(String(Z21mobileSwVer).substring(1));
  // drawing commands to make them visible on screen!
  Display_display();
}

//----------------------
void OtherDisplayShown()
//----------------------
{
  DisplayReady = 0;
  AllowStartDisplay = false;
}


//--------------
void ScrollUp8()
//--------------
{
  #
  uint8_t *p = display.getBufferPtr();
  #if ROTATE_DISPLAY_180_DEG == 0 // Normal mode
      memmove(p, p+SCREEN_WIDTH,  SCREEN_WIDTH*((SCREEN_HEIGHT/8)-1));
      memset(p+SCREEN_WIDTH*((SCREEN_HEIGHT/8)-1), !display.getDrawColor(), SCREEN_WIDTH);
  #else // Rotated display => Scroll in the other direction
      memmove(p+SCREEN_WIDTH,  p, SCREEN_WIDTH*((SCREEN_HEIGHT/8)-1));
      memset(p, !display.getDrawColor(), SCREEN_WIDTH); // Füllt unten
  #endif
}

//--------------------------------
void ScrollUp_small(uint8_t Steps)
//--------------------------------
// Steps 1..7
{
  #if OLED_TYP == 7920
     // Memory layout LCD Display (OLED_TYP == 7920)
     // Erstes Byte links oben, Bit Anordnung
     // 76543210 76543210 76543210 76543210
     //  Byte 0   Byte 1   Byte 2
     #if ROTATE_DISPLAY_180_DEG == 0 // Normal mode
        uint8_t *p = display.getBufferPtr();
        memmove(p, p+((SCREEN_WIDTH/8)*Steps), (SCREEN_WIDTH/8)*(SCREEN_HEIGHT-Steps));
        memset(p+(SCREEN_WIDTH/8)*(SCREEN_HEIGHT-Steps), !display.getDrawColor(), ((SCREEN_WIDTH/8)*Steps));
     #else
        uint8_t *p = display.getBufferPtr();
        memmove(p+((SCREEN_WIDTH/8)*Steps), p, (SCREEN_WIDTH/8)*(SCREEN_HEIGHT-Steps));
        memset(p, !display.getDrawColor(), ((SCREEN_WIDTH/8)*Steps));
     #endif
  #else // OLED_TYP != 7920
     #if ROTATE_DISPLAY_180_DEG == 0 // Normal mode
        // Der Bildschim Speicher hat folgenden Aufbau:
        // - Das erste Byte enthält die Pixel x=0, y=0 0 x=0, y=7
        //   0
        //   1
        //   :
        //   7
        // - Wenn der Bildschirm um ein Pixel nach oben gescrollt werden soll, dann wird das Bit 0 rausgeschoben.
        //   Alle anderen Bits rutschen um eins nach oben. Bit 7 wird mit Bit 0 der folgenden Zeile aufgefüllt.
        // - In der letzen Zeile ist das auffüllen nicht mehr nötig
        // - ToDo: Bei einem inversen Hintergrund müssten hier Einsen anstelle von Nullen reingeschoben werden.
        uint8_t *p = display.getBufferPtr();
        uint8_t OtherSteps = 8 - Steps;
        for (uint16_t i = 0; i < SCREEN_WIDTH*((SCREEN_HEIGHT/8)-1); i++, p++)
          *p = (*p >> Steps) | (p[SCREEN_WIDTH] << OtherSteps);

        for (uint16_t i = 0; i < SCREEN_WIDTH; i++, p++)
          *p = *p >> Steps;
     #else // Rotated display
        // Hier muss in der anderen Richtung geschoben werden
        uint8_t *p = display.getBufferPtr() + SCREEN_WIDTH*(SCREEN_HEIGHT/8) - 1;
        uint8_t OtherSteps = 8 - Steps;
        for (uint16_t i = 0; i < SCREEN_WIDTH*((SCREEN_HEIGHT/8)-1); i++, p--)
          *p = (*p << Steps) | (p[-SCREEN_WIDTH] >> OtherSteps);

        for (uint16_t i = 0; i < SCREEN_WIDTH; i++, p--)
          *p = *p << Steps;
     #endif
  #endif // OLED_TYP == 7920
}



//--------------------------
void ScrollUp(uint8_t Steps)
//--------------------------
{
  #if OLED_TYP != 7920
    while(Steps > 8)
      {
      ScrollUp8();
      Steps -= 8;
      }
  #endif
  if (Steps) ScrollUp_small(Steps);
}



#include <cstdarg>
#if !defined(ESP32)
  #include <Wstring.h>
#endif

#define Write_OLED(Format, ...) Write_OLED_Proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#define PRINTF_BUF 100

//----------------------------------------------------------
void Write_OLED_Proc(const __FlashStringHelper *format, ...)
//----------------------------------------------------------
// Don't call this function directly. Use the Write_OLED() instead to activate the __FlashStringHelper
// Attention: It takes 32 ms to update the OLED Display (Measured with the 1.3" Display)
// Special control characters:
//   \14 = Clear screen
//   \21 = Font 1
//   \22 = Font 2
//   \23 = Font 3
//   \24 = Font 4
//   \25 = Font 5
//   \37 = Don't show the display now. Usefull if several Write_OLED commands are used for one display
// ToDo:
// Special Charachter commands:
//   \1  = Line 1
//   \2  = Line 2
//    :
//   \5  = Line 5
//   \17 = Clr line
//   \18 = Clr end of screen
{
  OtherDisplayShown();
  char buf[PRINTF_BUF], *p = buf;
  va_list ap;
  va_start(ap, format);
  #ifdef __AVR__
     vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
  #else
     vsnprintf  (buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
  #endif
  va_end(ap);

  bool Display_Now = 1;

  while (*p)
        {
        bool SkipChar = 0;
        uint8_t Font = 0;
        switch (*p)
            {
            case '\14': Display_clearDisplay();
                        display.setCursor(0, Write_OLED_LineHeight+1);  // Old: Display_setCursor(0,0);
                        Write_OLED_y = 0;
                        SkipChar = 1;
                        break;
            case '\37': Display_Now = 0;
                        SkipChar = 1;
                        break;
            case '\21': Font = 1; break;
            case '\22': Font = 2; break;
            case '\23': Font = 3; break;
            case '\24': Font = 4; break;
            case '\25': Font = 5; break;
            }
        if (Font)
             Display_setTextSize(Font);
        else if (!SkipChar)
                {
                if (Write_OLED_y + Write_OLED_LineHeight > SCREEN_HEIGHT)
                   {
                   ScrollUp(Write_OLED_LineHeight);
                   Write_OLED_y -= Write_OLED_LineHeight;
                   Display_setCursor(0, Write_OLED_y);
                   }

                if (*p != '\n')
                     display.print(*p);
                else {
                     Write_OLED_y += Write_OLED_LineHeight;
                     Display_setCursor(0, Write_OLED_y);
                     }
                }
        p++;
        }
  if (Display_Now)
     Display_display(); // drawing commands to make them visible on screen!
                        // This takes 32 ms !
                        // The function waits until all I2C operations are finished
}

//-----------------------------------------
void Write_OLED_SetXY(uint8_t x, uint8_t y)
//-----------------------------------------
{
  Write_OLED_y = y;
  Display_setCursor(x, Write_OLED_y);
}

//----------------
void Test_Scroll()
//----------------
// Test the scrolling
{
  Display_clearDisplay();

   Display_setTextSize(1);
   for (uint16_t i = 0; i < 20; i++)
    {
    //Display_setTextSize(1+i%2);
    Write_OLED("\nTest ");       // '\n' at the beginning to use the correct line height if different fonts are used
    Write_OLED("%i\n", i);
    delay(500);
    }
   Write_OLED_SetXY(20, 20);
   Write_OLED("End\n");
  delay(30000);
}

//--------------------------------------------------------------
//Update full display
void DisplayStart() {
  DisplayReady = true;

  //Test_Scroll();


  // Clear the buffer
  Display_clearDisplay();

  DisplayGobal();
  DisplayUpdateRailPower();
  #if defined(S88N)
      DisplayUpdateS88N();
  #endif

  byte y = DISPLAYLINEHIGHT_SMALL_FONT * 2;  //Start after S88 Module output!

  #if defined(LAN)
  y += DISPLAYLINEHIGHT_SMALL_FONT;
  DisplayClearPart(0, y, SCREEN_WIDTH);
  display.print(F("Eth:  "));
  display.print(LAN_ip);
  #endif

  #if defined(WIFI)
  y += DISPLAYLINEHIGHT_SMALL_FONT;
  DisplayClearPart(0, y, SCREEN_WIDTH);
  display.print(F("WLAN: "));
  display.print(WLANgetConf(4));  //Client IP
  y += DISPLAYLINEHIGHT_SMALL_FONT;
  DisplayClearPart(0, y, SCREEN_WIDTH);
  display.print(WLANgetConf(5));  //Client SSID
  y += DISPLAYLINEHIGHT_SMALL_FONT;
  DisplayClearPart(0, y, SCREEN_WIDTH);
  display.print(F("ESP Sw v"));
  display.print(WLANgetConf(8));  //ESP Sw Version
  #endif

  #if defined(ESP_WIFI)
  y += DISPLAYLINEHIGHT_SMALL_FONT;
  DisplayClearPart(0, y, SCREEN_WIDTH);
  display.print(ssidAP);  //AP Name
  y += DISPLAYLINEHIGHT_SMALL_FONT;
  DisplayClearPart(0, y, SCREEN_WIDTH);
  display.print(F("AP:   "));
  display.print(WiFi.softAPIP());

  y += DISPLAYLINEHIGHT_SMALL_FONT;
  DisplayClearPart(0, y, SCREEN_WIDTH);
  display.print(F("WLAN: "));
  display.print(WiFi.localIP().toString());
  y += DISPLAYLINEHIGHT_SMALL_FONT;
  DisplayClearPart(0, y, SCREEN_WIDTH);
  display.print(ssid.c_str());
  #endif

  // drawing commands to make them visible on screen!
  Display_display();
}

//--------------------------------------------------------------
void DisplayBoot() {
  display.print(F("."));
  // drawing commands to make them visible on screen!
  Display_display();
}

/*
uint8_t Read_Z21Button(); // Forward definition
//----------------
void Scroll_Test()
//----------------
{
  for (uint8_t i = 0; i < 3; i++)
    {
    ScrollUp(1);
    Display_display();
    while (Read_Z21Button() != 0);
    while (Read_Z21Button() == 0);
    }

  Display_setTextSize(1);
  for (uint8_t i = 0; i < 10; i++)
    {
    Write_OLED("Aber Hallo %i\n", i);
    Display_display();
    while (Read_Z21Button() != 0);
    while (Read_Z21Button() == 0);
    }
}
*/

//----------------------------------------------
void Display_Boot_Start_Display(const char *Txt)
//----------------------------------------------
{
  // Clear the buffer
  Display_clearDisplay();
  Display_setTextColor(SSD1306_WHITE);        // Draw white text
  DisplayGobal();
  Display_setCursor(0,40);
  display.print(F("Sw-Build:"));
  Display_setCursor(0,50);
  display.print(comp_date);

  Display_setTextSize(2);
  Display_setCursor(0,18);
  display.print(Txt);

  // drawing commands to make them visible on screen!
  Display_display();

  //Scroll_Test();
}

//--------------------------------------------------------------
//Init the Display
void DisplaySetup(void) {
  if (!Display_Begin()) {
       #if defined(Debug)
       Debug.println(F("Display not found!"));
       #endif
       }
  else {
       Display_Boot_Start_Display("Boot");

       #if 0 // Enable to print the font sizes
         for (uint8_t i = 5; i >= 1; i--) // debug
           {
           Serial.print(i); Serial.print(": w:"); Serial.print(display.getMaxCharWidth()); Serial.print(" h:"); Serial.print(display.getMaxCharHeight());
           Serial.print("  "); Serial.print(128.0/display.getMaxCharWidth()); Serial.print(" x "); Serial.println(64.0 / display.getMaxCharHeight());
           }
         Display_setTextSize(1);
       #endif
       }
}

uint32_t NextTime_to_DisplayStart = 0;  // Time to show the standard display with IP adress, ...

//-------------------------------------
void DisplayStart_in_xx_ms(uint32_t ms)
//-------------------------------------
{
  NextTime_to_DisplayStart = millis() + ms;
}


//--------------
void OLED_loop()
//--------------
{
  if (NextTime_to_DisplayStart && millis() > NextTime_to_DisplayStart)
     {
     NextTime_to_DisplayStart = 0;
     DisplayStart();
     }
}


#else // Z21DISPLAY is not defined
  // Emptiy functions in case Z21DISPLAY is not defined to avoid "#if defined(Z21DISPLAY)" in the code
  void DisplayStart()                               {}
  void Display_Boot_Start_Display(const char *Txt)  {}
  void DisplayBoot()                                {}
  void DisplaySetup()                               {}
  void Display_setTextSize(uint8_t Size)            {}
  void OLED_loop()                                  {}
  void DisplayStart_in_xx_ms(ms al uint32_t)        {}
  void OtherDisplayShown()                          {}
  #define Write_OLED(Format, ...)

#endif // Z21DISPLAY
