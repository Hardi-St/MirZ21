#ifndef _Dprintf_h_
#define _Dprintf_h_

//#if defined(ESP32)
//  #define __FlashStringHelper  char
//  #define F(str)               str
//#endif

#define PRINT_DEBUG_MESSAGES  // Enable this line to print debug messages to the PC (USB)
                                 // This would reduce the number of possible loks because there
                                 // is not enough RAM available if debug messages are generated.

#ifdef PRINT_DEBUG_MESSAGES
  // #define DEBUG_LCD_OUTPUTS   // Print all LCD outputs additionally to the debug output (serial port)
  // #define PRINT_CAN_FUNC_DESC // Enable this line to print a description of the received CAN messages (Slows down the program => Could generate problems )
  // #define PRINT_LOK_NAME_UID  // Print the Lok name and the uid in Print_Lok_Name_and_uid()
     #define DEBUG_IR_ACTIONS    // Print IR actions
  // #define DEBUG_CAN_INIT      // Debug CAN initialisation
  // #define DEBUG_CAN_SEND      // Print CAN Send messages
     #define DEBUG_READ_MS2      // Debug reading MS2 data
  // #define DEBUG_POWER_SUPPLY  // Print the power supply voltage at program start
  // #define DEBUG_LOK2EEPROM_W  // Debug Lok name/uid to EEPROM Write
  // #define DEBUG_LOK2EEPROM_R  // Debug Lok name/uid to EEPROM Read
#endif


#ifdef PRINT_DEBUG_MESSAGES
   // F() makro saves dynamic memory
   #define Dprintf(Format, ...) Dprintf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
   // Memory based on 16.08.17:
   // Flash: 22736   70%
   // Ram:    1642   80%
#else
   #define DPRINTF(Str)          {}
   #define Dprintf(Format, ...)  {}
   // Flash: 20516   63%     save: 2220
   // Ram:   1292    63%     save:  390
#endif

#include <stdarg.h>

//#if !defined(ESP32)
  #include <Wstring.h>
//#endif

void Dprintf_proc(const __FlashStringHelper *format, ...);

#endif
