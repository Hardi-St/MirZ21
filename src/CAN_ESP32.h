#if defined(ESP32)
/*
 Class which defines the same functions like the MCP_CAN class for the ESP32

 Just a quick Hack, not all functions added

 The implemeted functions have been tested
*/

#ifndef DEBUG_CAN_ESP32
  #define DEBUG_CAN_ESP32 0
#endif

#include <CAN.h>             // The Sandeep Mistry library must be installed (0.3.1)

#define CAN_OK             (0)
#define CAN_FAILINIT       (1)
#define CAN_FAILTX         (2)
#define CAN_MSGAVAIL       (3)
#define CAN_NOMSG          (4)
#define CAN_CTRLERROR      (5)
#define CAN_GETTXBFTIMEOUT (6)
#define CAN_SENDMSGTIMEOUT (7)
#define CAN_FAIL       (0xff)

#define MCP_STDEXT   0                                                  /* Standard and Extended        */
#define MCP_STD      1                                                  /* Standard IDs ONLY            */
#define MCP_EXT      2                                                  /* Extended IDs ONLY            */
#define MCP_ANY      3                                                  /* Disables Masks and Filters   */

#define MCP_20MHZ    0
#define MCP_16MHZ    1
#define MCP_8MHZ     2

#define CAN_4K096BPS 0
#define CAN_5KBPS    1
#define CAN_10KBPS   2
#define CAN_20KBPS   3
#define CAN_31K25BPS 4
#define CAN_33K3BPS  5
#define CAN_40KBPS   6
#define CAN_50KBPS   7
#define CAN_80KBPS   8
#define CAN_100KBPS  9
#define CAN_125KBPS  10
#define CAN_200KBPS  11
#define CAN_250KBPS  12
#define CAN_500KBPS  13
#define CAN_1000KBPS 14

#define MCP_NORMAL     0x00

#ifndef INT32U
#define INT32U unsigned long
#endif

#ifndef INT8U
#define INT8U byte
#endif

class CAN_ESP32
{
    private:
      INT8U RcvLen;
      INT32U _Mask;
      INT32U _Filt;

      //  Workaround for filterExtended bug in Mistry library (0.3.1)  by Jürgen
	  #define REG_BASE                   0x3ff6b000
	  #define REG_MOD                    0x00
	  #define REG_ACRn(n)                (0x10 + n)
	  #define REG_AMRn(n)                (0x14 + n)

      //------------------------------------------------
	  void writeRegister(uint8_t address, uint8_t value)
      //------------------------------------------------
	  {
	    volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);
	    *reg = value;
	  }

      //---------------------------------------------------------------
	  void modifyRegister(uint8_t address, uint8_t mask, uint8_t value)
      //---------------------------------------------------------------
	  {
	    volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);
	    *reg = (*reg & ~mask) | value;
	  }

      //------------------------------------
	  int filterExtended(long id, long mask)
      //------------------------------------
	  {
	    id &= 0x1FFFFFFF;
	    mask = ~(mask & 0x1FFFFFFF);
        modifyRegister(REG_MOD, 0x17, 0x09); // reset      // Hier müsste 0x09 stehen "Acceptance Filter Mode: Bit 3)    // 14.02.22:  Old 0x01
	    writeRegister(REG_ACRn(0), id >> 21);
        writeRegister(REG_ACRn(1), id >> 13);
        writeRegister(REG_ACRn(2), id >> 5);
        writeRegister(REG_ACRn(3), id << 3);                                                                  // Old 14.02.22:  << 5

	    writeRegister(REG_AMRn(0), mask >> 21);
	    writeRegister(REG_AMRn(1), mask >> 13);
	    writeRegister(REG_AMRn(2), mask >> 5);
        writeRegister(REG_AMRn(3), (mask << 3));                                                              // Old 14.02.22:  (mask << 5) |0x1f)
	    modifyRegister(REG_MOD, 0x17, 0x00); // normal
	    return 1;
	  }

      //------------------
      int filterExtended()
      //------------------
      {
        #if DEBUG_CAN_ESP32
          Serial.print(F("filterExtended: ")); Serial.print( _Filt, HEX); Serial.print(", "); Serial.println(_Mask, HEX);
        #endif
        int res = filterExtended(_Filt, _Mask);
        _Filt = _Mask = 0xFFFFFFFF;
        return res;
      }

public:
    CAN_ESP32()
    {
     _Mask = _Filt = 0xFFFFFFFF;
    }

    //----------------------------------------------------------
    INT8U begin(INT8U idmodeset, INT8U speedset, INT8U clockset)           // Initialize controller parameters
    //----------------------------------------------------------
    {
      CAN.setPins(4, 5);
      switch (speedset)
        {
        case CAN_250KBPS: CAN.begin(250E3); break;
        case CAN_500KBPS: CAN.begin(500E3); break;
        default: return CAN_FAILINIT;
        }
      return CAN_OK;
    }

    //---------------------------------------------------
    INT8U init_Mask(INT8U num, INT8U ext, INT32U mask)                     // Initialize Mask(s)
    //---------------------------------------------------
    // Attention: Only the extendet Mode is supportet at the moment
    {
        #if DEBUG_CAN_ESP32
          Serial.print(F("CAN_ESP32.init_Mask(")); Serial.print(num); Serial.print(F(", ")); Serial.print(ext); Serial.print(F(", ")); Serial.print(mask, HEX); Serial.println(')');
        #endif
        if (!ext) return CAN_FAIL;
        if (num == 0)
           {
           _Mask = mask & 0x1FFFFFFF;
           if (_Filt != 0xFFFFFFFF) filterExtended();
           }
        return CAN_OK;
    }

//    INT8U init_Mask(INT8U num, INT32U ulData);                          // Initialize Mask(s)

    //----------------------------------------------
    INT8U init_Filt(INT8U num, INT8U ext, INT32U id)                      // Initialize Filter(s)
    //----------------------------------------------
    // Attention: Only the extendet Mode is supportet at the moment
    {
        #if DEBUG_CAN_ESP32
          Serial.print(F("CAN_ESP32.init_Filt(")); Serial.print(num); Serial.print(F(", ")); Serial.print(ext); Serial.print(F(", ")); Serial.print(id, HEX); Serial.println(')');
        #endif
        if (!ext) return CAN_FAIL;
        if (num == 0)
           {
           _Filt = id & 0x1FFFFFFF;
           if (_Mask != 0xFFFFFFFF) filterExtended();
           }
        return CAN_OK;
    }
//    INT8U init_Filt(INT8U num, INT32U ulData);                          // Initialize Filter(s)

    INT8U setMode(INT8U opMode) { return CAN_OK; }                        // Set operational mode

    //-----------------------------------------------------------
    INT8U sendMsgBuf(INT32U id, INT8U ext, INT8U len, INT8U *buf)         // Send message to transmit buffer
    //-----------------------------------------------------------
    {
      if (ext)
            CAN.beginExtendedPacket(id);
      else  CAN.beginPacket(id);
      for (uint8_t i = 0; i < len; i++)
          CAN.write(buf[i]);
       CAN.endPacket();
       return CAN_OK;
    }

    //------------------------------------------------
    INT8U sendMsgBuf(INT32U id, INT8U len, INT8U *buf)                   // Send message to transmit buffer
    //------------------------------------------------
    {
      return sendMsgBuf(id, id & 0x80000000, len, buf);  // Attention: rtr flag (0x40000000) is not supported
    }

//    INT8U readMsgBuf(INT32U *id, INT8U *ext, INT8U *len, INT8U *buf);   // Read message from receive buffer

    //--------------------------------------------------
    INT8U readMsgBuf(INT32U *id, INT8U *len, INT8U *buf)                  // Read message from receive buffer
    //--------------------------------------------------
    {
      *id = CAN.packetId();
      *len = RcvLen;
      for (uint8_t i = 0; i < *len; i++)
        buf[i] = CAN.read();
      return CAN_OK;
    }

    //----------------------
    INT8U checkReceive(void)                                              // Check for received data
    //----------------------
    {
      if ((RcvLen = CAN.parsePacket()) > 0)
           return CAN_MSGAVAIL;
      else return CAN_NOMSG;
    }

//    INT8U checkError(void);                                             // Check for errors
//    INT8U getError(void);                                               // Check for errors
//    INT8U errorCountRX(void);                                           // Get error count
//    INT8U errorCountTX(void);                                           // Get error count
//    INT8U enOneShotTX(void);                                            // Enable one-shot transmission
//    INT8U disOneShotTX(void);                                           // Disable one-shot transmission
//    INT8U abortTX(void);                                                // Abort queued transmission(s)
//    INT8U setGPO(INT8U data);                                           // Sets GPO
//    INT8U getGPI(void);                                                 // Reads GPI

    void  EnableStatusLEDs(void) {}                                       // Status LEDs are not available
};

/*********************************************************************************************************
 *  END FILE
 *********************************************************************************************************/

#endif // #if defined(ESP32)
