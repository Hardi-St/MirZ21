#define BEEP_SINGLE_KEY_TIME   25 // Duration of the short high beeps which is used when a single IR key is received sucessfully
#define BEEP_REPEAT_KEY_TIME   5

#ifdef __AVR_ATmega2560__
//-----------------
void Toggle_Piezo()
//-----------------
{
  #ifdef __AVR_ATmega2560__
        if (PIEZO_PORT & PIEZO_MASK)
             PIEZO_PORT &= ~PIEZO_MASK;
        else PIEZO_PORT |=  PIEZO_MASK;
  #else
        digitalWrite(PIEZO_PIN, !digitalRead(PIEZO_PIN));
  #endif
}
#endif

//-----------------
void Enable_Piezo()
//-----------------
{
  #ifdef __AVR_ATmega2560__
    PIEZO_DDR |= PIEZO_MASK;
  #else
    #ifdef PIEZO_PIN                    // ToDo: ESP32
      pinMode(PIEZO_PIN, OUTPUT);
    #endif
  #endif
}


//------------------
void Disable_Piezo()
//------------------
{
  #ifdef __AVR_ATmega2560__
    PIEZO_DDR &= ~PIEZO_MASK;
  #else
    #ifdef PIEZO_PIN
      digitalWrite(PIEZO_PIN, 0);                                                                             // 24.02.22:  Old: Produces an almost inaudible beep   pinMode(PIEZO_PIN, INPUT);
    #endif
  #endif
}

#ifdef __AVR_ATmega2560__
//--------------------------------------------
void Beep_Var(uint16_t ms, uint16_t HalfPeriod)
//--------------------------------------------
// Generate a tone with variable frequency
// Can't use tone() because it generates a interrupt conflict with the Infraread library "IRremote.h"
{
    Enable_Piezo();
    for (uint16_t i = 0; i < ms; i++)
        {
        Toggle_Piezo();
        delayMicroseconds(HalfPeriod);
        }
    Disable_Piezo();
}

//--------------------
void Beep(uint16_t ms)
//--------------------
// Generate a tone with 500 Hz
{
  Beep_Var(ms, 1000);
}

//-------------------------
void Beep_Deep(uint16_t ms)
//-------------------------
// Generate a tone with 250 Hz
{
  Beep_Var(ms, 2000);
}


//-------------------------
void Beep_High(uint16_t ms)
//-------------------------
// Generate a tone with 1 KHz
{
  Beep_Var(ms, 500);
}


#else // not __AVR_ATmega2560__


#if defined(ESP32_MCU)
    //------------------------------------------------
    void tone(uint8_t Pin, uint16_t Freq, uint16_t ms)
    //------------------------------------------------
    {
      ledcSetup(0, 2000, 8);  // setup beeper
      ledcAttachPin(Pin, 0);  // attach beeper
      ledcWriteTone(0, Freq); // play tone
      delay(ms);
      ledcWriteTone(0, 0);
    }
#endif

//--------------------
void Beep(uint16_t ms)
//--------------------
// Generate a tone with 494 Hz (B4)
{
  #ifdef PIEZO_PIN
    tone(PIEZO_PIN, 494, ms);
  #endif
}

//-------------------------
void Beep_Deep(uint16_t ms)
//-------------------------
// Generate a tone with 247 Hz (B3)
{
  #ifdef PIEZO_PIN
    tone(PIEZO_PIN, 247, ms);
  #endif
}


//-------------------------
void Beep_High(uint16_t ms)
//-------------------------
// Generate a tone with 988 Hz (B5)
{
  #ifdef PIEZO_PIN
    tone(PIEZO_PIN, 988, ms);
  #endif
}


#endif // __AVR_ATmega2560__

//--------------------
void Beep_Single_Key()
//--------------------
// Beep to indicate a received single key from the TV IR control
{
  #if BEEP_SINGLE_KEY_TIME > 0
    Beep_High(BEEP_SINGLE_KEY_TIME);
  #endif
}

//--------------------
void Beep_Repeat_Key()
//--------------------
// Beep to indicate a received single key from the TV IR control
{
  #if BEEP_REPEAT_KEY_TIME > 0
    Beep_High(BEEP_REPEAT_KEY_TIME);
  #endif
}
