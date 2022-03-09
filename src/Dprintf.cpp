
#include "Dprintf.h"

//#if !defined(ESP32)
  #include "HardwareSerial.h"  // Defines "Serial"
  #include <Arduino.h>
//#endif
#include <stdarg.h>


//-------------------------------------------------------
void Dprintf_proc(const __FlashStringHelper *format, ...)
//-------------------------------------------------------
{
//#if !defined(ESP32)  // ToDo ESP
  char buf[200];                                                                                              // 15.01.22:  Old: 50
  va_list ap;
  va_start(ap, format);
  #ifdef __AVR__
     vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
  #else
     vsnprintf  (buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
  #endif

  va_end(ap);
  Serial.print(buf);
//#endif
}

