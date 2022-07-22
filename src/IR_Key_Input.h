#ifndef _IR_Key_Input_h_
#define _IR_Key_Input_h_

#define GRUNDIG // Enable this line to use the Grundig RC-YC1 IR Remote control

#define IR_KEY_SHORT_TIME        300 // ms
#define IR_KEY_RCV_PERIOD        200 // ms maximal period between to received key codes if a button is hold

#define IR_KEY_M_LIGHT             0x00
#define IR_KEY_M_F1                0x01
#define IR_KEY_M_F2                0x02
#define IR_KEY_M_F3                0x03
#define IR_KEY_M_F4                0x04
#define IR_KEY_M_MINUS             0x05
#define IR_KEY_M_PLUS              0x06
#define IR_KEY_M_STOP              0x07
#define IR_KEY_J_STOP              0x08                                                                       // 19.07.22:

#define IR_KEY_M_PRESS_LIGHT       (IR_KEY_M_LIGHT + IR_KEY_PRESSED)
#define IR_KEY_M_PRESS_F1          (IR_KEY_M_F1    + IR_KEY_PRESSED)
#define IR_KEY_M_PRESS_F2          (IR_KEY_M_F2    + IR_KEY_PRESSED)
#define IR_KEY_M_PRESS_F3          (IR_KEY_M_F3    + IR_KEY_PRESSED)
#define IR_KEY_M_PRESS_F4          (IR_KEY_M_F4    + IR_KEY_PRESSED)
#define IR_KEY_M_PRESS_MINUS       (IR_KEY_M_MINUS + IR_KEY_PRESSED)
#define IR_KEY_M_PRESS_PLUS        (IR_KEY_M_PLUS  + IR_KEY_PRESSED)
#define IR_KEY_M_PRESS_STOP        (IR_KEY_M_STOP  + IR_KEY_PRESSED)
#define IR_KEY_J_PRESS_STOP        (IR_KEY_J_STOP  + IR_KEY_PRESSED)                                          // 19.07.22:

#define IR_KEY_M_HOLD_LIGHT        (IR_KEY_M_LIGHT + IR_KEY_HOLD)
#define IR_KEY_M_HOLD_F1           (IR_KEY_M_F1    + IR_KEY_HOLD)
#define IR_KEY_M_HOLD_F2           (IR_KEY_M_F2    + IR_KEY_HOLD)
#define IR_KEY_M_HOLD_F3           (IR_KEY_M_F3    + IR_KEY_HOLD)
#define IR_KEY_M_HOLD_F4           (IR_KEY_M_F4    + IR_KEY_HOLD)
#define IR_KEY_M_HOLD_MINUS        (IR_KEY_M_MINUS + IR_KEY_HOLD)
#define IR_KEY_M_HOLD_PLUS         (IR_KEY_M_PLUS  + IR_KEY_HOLD)
#define IR_KEY_M_HOLD_STOP         (IR_KEY_M_STOP  + IR_KEY_HOLD)
#define IR_KEY_J_HOLD_STOP         (IR_KEY_J_STOP  + IR_KEY_HOLD)

#define IR_KEY_M_RELEASED_LIGHT    (IR_KEY_M_LIGHT + IR_KEY_RELEASED)
#define IR_KEY_M_RELEASED_F1       (IR_KEY_M_F1    + IR_KEY_RELEASED)
#define IR_KEY_M_RELEASED_F2       (IR_KEY_M_F2    + IR_KEY_RELEASED)
#define IR_KEY_M_RELEASED_F3       (IR_KEY_M_F3    + IR_KEY_RELEASED)
#define IR_KEY_M_RELEASED_F4       (IR_KEY_M_F4    + IR_KEY_RELEASED)
#define IR_KEY_M_RELEASED_MINUS    (IR_KEY_M_MINUS + IR_KEY_RELEASED)
#define IR_KEY_M_RELEASED_PLUS     (IR_KEY_M_PLUS  + IR_KEY_RELEASED)
#define IR_KEY_M_RELEASED_STOP     (IR_KEY_M_STOP  + IR_KEY_RELEASED)
#define IR_KEY_J_RELEASED_STOP     (IR_KEY_J_STOP  + IR_KEY_RELEASED)

#ifdef GRUNDIG
// The Grundig Key use the sufix _G_
  #define IR_KEY_G_NR0               0x10
  #define IR_KEY_G_NR1               0x11
  #define IR_KEY_G_NR2               0x12
  #define IR_KEY_G_NR3               0x13
  #define IR_KEY_G_NR4               0x14
  #define IR_KEY_G_NR5               0x15
  #define IR_KEY_G_NR6               0x16
  #define IR_KEY_G_NR7               0x17
  #define IR_KEY_G_NR8               0x18
  #define IR_KEY_G_NR9               0x19
  #define IR_KEY_G_POWER             0x1C
  #define IR_KEY_G_MUTE              0x1D
  #define IR_KEY_G_V_PLUS            0x20
  #define IR_KEY_G_V_MINUS           0x21
  #define IR_KEY_G_RIGHT             0x22
  #define IR_KEY_G_LEFT              0x23
  #define IR_KEY_G_UP                0x26
  #define IR_KEY_G_DOWN              0x27
  #define IR_KEY_G_PR_PLUS           0x30
  #define IR_KEY_G_PR_MINUS          0x31
  #define IR_KEY_G_OK                0x45
  #define IR_KEY_G_GREEN             0x46
  #define IR_KEY_G_RED               0x47
  #define IR_KEY_G_16_9              0x48
  #define IR_KEY_G_REPEAT            0x49
  #define IR_KEY_G_A_B               0x50
  #define IR_KEY_G_EPG               0x51
  #define IR_KEY_G_FIND              0x52
  #define IR_KEY_G_M                 0x53
  #define IR_KEY_G_BACK              0x54
  #define IR_KEY_G_i                 0x55
  #define IR_KEY_G_YELLOW            0x56
  #define IR_KEY_G_BLUE              0x57
  #define IR_KEY_G_TXT               0x58
  #define IR_KEY_G_R                 0x59
  #define IR_KEY_G_BOX               0x60
  #define IR_KEY_G_SPEAKER           0x61

  #define IR_KEY_G_PRESS_NR0        (IR_KEY_G_NR0      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_NR1        (IR_KEY_G_NR1      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_NR2        (IR_KEY_G_NR2      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_NR3        (IR_KEY_G_NR3      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_NR4        (IR_KEY_G_NR4      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_NR5        (IR_KEY_G_NR5      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_NR6        (IR_KEY_G_NR6      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_NR7        (IR_KEY_G_NR7      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_NR8        (IR_KEY_G_NR8      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_NR9        (IR_KEY_G_NR9      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_POWER      (IR_KEY_G_POWER    + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_MUTE       (IR_KEY_G_MUTE     + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_V_PLUS     (IR_KEY_G_V_PLUS   + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_V_MINUS    (IR_KEY_G_V_MINUS  + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_RIGHT      (IR_KEY_G_RIGHT    + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_LEFT       (IR_KEY_G_LEFT     + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_UP         (IR_KEY_G_UP       + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_DOWN       (IR_KEY_G_DOWN     + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_PR_PLUS    (IR_KEY_G_PR_PLUS  + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_PR_MINUS   (IR_KEY_G_PR_MINUS + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_OK         (IR_KEY_G_OK       + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_GREEN      (IR_KEY_G_GREEN    + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_RED        (IR_KEY_G_RED      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_16_9       (IR_KEY_G_16_9     + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_REPEAT     (IR_KEY_G_REPEAT   + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_A_B        (IR_KEY_G_A_B      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_EPG        (IR_KEY_G_EPG      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_FIND       (IR_KEY_G_FIND     + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_M          (IR_KEY_G_M        + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_BACK       (IR_KEY_G_BACK     + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_i          (IR_KEY_G_i        + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_YELLOW     (IR_KEY_G_YELLOW   + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_BLUE       (IR_KEY_G_BLUE     + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_TXT        (IR_KEY_G_TXT      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_R          (IR_KEY_G_R        + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_BOX        (IR_KEY_G_BOX      + IR_KEY_PRESSED)
  #define IR_KEY_G_PRESS_SPEAKER    (IR_KEY_G_SPEAKER  + IR_KEY_PRESSED)

  #define IR_KEY_G_HOLD_NR0         (IR_KEY_G_NR0      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_NR1         (IR_KEY_G_NR1      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_NR2         (IR_KEY_G_NR2      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_NR3         (IR_KEY_G_NR3      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_NR4         (IR_KEY_G_NR4      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_NR5         (IR_KEY_G_NR5      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_NR6         (IR_KEY_G_NR6      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_NR7         (IR_KEY_G_NR7      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_NR8         (IR_KEY_G_NR8      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_NR9         (IR_KEY_G_NR9      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_POWER       (IR_KEY_G_POWER    + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_MUTE        (IR_KEY_G_MUTE     + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_V_PLUS      (IR_KEY_G_V_PLUS   + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_V_MINUS     (IR_KEY_G_V_MINUS  + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_RIGHT       (IR_KEY_G_RIGHT    + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_LEFT        (IR_KEY_G_LEFT     + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_UP          (IR_KEY_G_UP       + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_DOWN        (IR_KEY_G_DOWN     + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_PR_PLUS     (IR_KEY_G_PR_PLUS  + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_PR_MINUS    (IR_KEY_G_PR_MINUS + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_OK          (IR_KEY_G_OK       + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_GREEN       (IR_KEY_G_GREEN    + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_RED         (IR_KEY_G_RED      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_16_9        (IR_KEY_G_16_9     + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_REPEAT      (IR_KEY_G_REPEAT   + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_A_B         (IR_KEY_G_A_B      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_EPG         (IR_KEY_G_EPG      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_FIND        (IR_KEY_G_FIND     + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_M           (IR_KEY_G_M        + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_BACK        (IR_KEY_G_BACK     + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_i           (IR_KEY_G_i        + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_YELLOW      (IR_KEY_G_YELLOW   + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_BLUE        (IR_KEY_G_BLUE     + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_TXT         (IR_KEY_G_TXT      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_R           (IR_KEY_G_R        + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_BOX         (IR_KEY_G_BOX      + IR_KEY_HOLD)
  #define IR_KEY_G_HOLD_SPEAKER     (IR_KEY_G_SPEAKER  + IR_KEY_HOLD)

  #define IR_KEY_G_RELEASED_FIND    (IR_KEY_G_FIND     + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_M       (IR_KEY_G_M        + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_BACK    (IR_KEY_G_BACK     + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_i       (IR_KEY_G_i        + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_GREEN   (IR_KEY_G_GREEN    + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_RED     (IR_KEY_G_RED      + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_YELLOW  (IR_KEY_G_YELLOW   + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_BLUE    (IR_KEY_G_BLUE     + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_TXT     (IR_KEY_G_TXT      + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_R       (IR_KEY_G_R        + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_BOX     (IR_KEY_G_BOX      + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_SPEAKER (IR_KEY_G_SPEAKER  + IR_KEY_RELEASED)
  #define IR_KEY_G_RELEASED_EPG     (IR_KEY_G_EPG      + IR_KEY_RELEASED)

  // Universal names used for the keys in switch/case statemants.
  // This keys bust be defined for each IR control
  // If several keys should be used  ": case" must be added in between
  // Example:  IR_KEY_PRESS_MUTE  : case IR_KEY_G_PRESS_16_9
  #define IR_KEY_TV_PRESS_NR0        IR_KEY_G_PRESS_NR0
  #define IR_KEY_TV_PRESS_NR1        IR_KEY_G_PRESS_NR1
  #define IR_KEY_TV_PRESS_NR2        IR_KEY_G_PRESS_NR2
  #define IR_KEY_TV_PRESS_NR3        IR_KEY_G_PRESS_NR3
  #define IR_KEY_TV_PRESS_NR4        IR_KEY_G_PRESS_NR4
  #define IR_KEY_TV_PRESS_NR5        IR_KEY_G_PRESS_NR5
  #define IR_KEY_TV_PRESS_NR6        IR_KEY_G_PRESS_NR6
  #define IR_KEY_TV_PRESS_NR7        IR_KEY_G_PRESS_NR7
  #define IR_KEY_TV_PRESS_NR8        IR_KEY_G_PRESS_NR8
  #define IR_KEY_TV_PRESS_NR9        IR_KEY_G_PRESS_NR9
  #define IR_KEY_TV_SWITCH_RED       IR_KEY_G_PRESS_REPEAT  // IR_KEY_PRESS_POWER
  #define IR_KEY_TV_SWITCH_GREEN     IR_KEY_G_PRESS_16_9    // IR_KEY_PRESS_MUTE
  #define IR_KEY_TV_SWITCH_TOGGLE    IR_KEY_G_PRESS_A_B
  #define IR_KEY_TV_SWITCH_CNG_DISP  IR_KEY_G_PRESS_MUTE    // IR_KEY_PRESS_16_9
  #define IR_KEY_TV_SWITCH_RST_DISP  IR_KEY_G_HOLD_MUTE     // IR_KEY_HOLD_16_9
  #define IR_KEY_TV_FUNCKEY          IR_KEY_G_PRESS_EPG
  #define IR_NR_TYP_HOLD_FUNCKEY     IR_KEY_G_HOLD_EPG
  #define IR_KEY_TV_RELEASED_FUNCKEY IR_KEY_G_RELEASED_EPG
//#define IR_KEY_TV_LOK_NR           IR_KEY_G_PRESS_REPEAT
  #define IR_KEY_TV_NEXT_SWITCH      IR_KEY_G_PRESS_V_PLUS
  #define IR_KEY_TV_PREV_SWITCH      IR_KEY_G_PRESS_V_MINUS
  #define IR_KEY_TV_NEXT_LOK         IR_KEY_G_PRESS_PR_PLUS
  #define IR_KEY_TV_PREV_LOK         IR_KEY_G_PRESS_PR_MINUS
  #define IR_KEY_TV_FORWARD          IR_KEY_G_PRESS_RIGHT
  #define IR_KEY_TV_BACKWARD         IR_KEY_G_PRESS_LEFT
  #define IR_KEY_TV_FASTER           IR_KEY_G_PRESS_UP
  #define IR_KEY_TV_SLOWER           IR_KEY_G_PRESS_DOWN
  #define IR_KEY_TV_HOLD_FASTER      IR_KEY_G_HOLD_UP
  #define IR_KEY_TV_HOLD_SLOWER      IR_KEY_G_HOLD_DOWN
  #define IR_KEY_TV_STOP_FUNC        IR_KEY_G_PRESS_OK
  #define IR_KEY_TV_EMERGENCY_STOP   IR_KEY_G_HOLD_OK
  #define IR_KEY_TV_FN0              IR_KEY_G_PRESS_FIND
  #define IR_KEY_TV_FN1              IR_KEY_G_PRESS_M
  #define IR_KEY_TV_FN2              IR_KEY_G_PRESS_BACK
  #define IR_KEY_TV_FN3              IR_KEY_G_PRESS_i
  #define IR_KEY_TV_FN4              IR_KEY_G_PRESS_RED
  #define IR_KEY_TV_FN5              IR_KEY_G_PRESS_GREEN
  #define IR_KEY_TV_FN6              IR_KEY_G_PRESS_YELLOW
  #define IR_KEY_TV_FN7              IR_KEY_G_PRESS_BLUE
  #define IR_KEY_TV_FN8              IR_KEY_G_PRESS_TXT
  #define IR_KEY_TV_FN9              IR_KEY_G_PRESS_R
  #define IR_KEY_TV_FN10             IR_KEY_G_PRESS_BOX
  #define IR_KEY_TV_FN11             IR_KEY_G_PRESS_SPEAKER
  #define IR_KEY_TV_RELEASED_FN0     IR_KEY_G_RELEASED_FIND
  #define IR_KEY_TV_RELEASED_FN1     IR_KEY_G_RELEASED_M
  #define IR_KEY_TV_RELEASED_FN2     IR_KEY_G_RELEASED_BACK
  #define IR_KEY_TV_RELEASED_FN3     IR_KEY_G_RELEASED_i
  #define IR_KEY_TV_RELEASED_FN4     IR_KEY_G_RELEASED_RED
  #define IR_KEY_TV_RELEASED_FN5     IR_KEY_G_RELEASED_GREEN
  #define IR_KEY_TV_RELEASED_FN6     IR_KEY_G_RELEASED_YELLOW
  #define IR_KEY_TV_RELEASED_FN7     IR_KEY_G_RELEASED_BLUE
  #define IR_KEY_TV_RELEASED_FN8     IR_KEY_G_RELEASED_TXT
  #define IR_KEY_TV_RELEASED_FN9     IR_KEY_G_RELEASED_R
  #define IR_KEY_TV_RELEASED_FN10    IR_KEY_G_RELEASED_BOX
  #define IR_KEY_TV_RELEASED_FN11    IR_KEY_G_RELEASED_SPEAKER

  #define IR_KEY_TV_YES              IR_KEY_G_PRESS_OK
  #define IR_KEY_TV_TRACKPOWER       IR_KEY_G_PRESS_POWER

#endif // GRUNDIG


#define IR_NO_KEY                0x00

//  New mask for additional numerical keys for switches...   27.09.17
//    76543210 76543210
//    phr-cccc kkkkkkkk   k = key 0..8 IR, 9-.. Other                                                         // 19.07.22:  Old: key 0..7 IR, 8-.. Other
//                        c = channel 0..15
//                        p = key pressed
//                        h = key hold
//                        r = key released
#define IR_Key_t                 uint16_t

#define IR_KEY_MASK              0xFF
#define IR_KEY_SHIFT             0

#define IR_CHANNEL_0             0x0000
#define IR_CHANNEL_1             0x0100
#define IR_CHANNEL_2             0x0200
#define IR_CHANNEL_3             0x0300
#define IR_CHANNEL_15            0x0F00

#define IR_KEY_PRESSED           0x8000 // If the key is pressed the first time
#define IR_KEY_HOLD              0x4000 // Is send if the key is HOLD longer than KEY_SHORT_TIME until the key is released
#define IR_KEY_RELEASED          0x2000 // If the key is released

#define IR_CHANNEL_MASK          0x0F00
#define IR_CHANNEL_SHIFT         8


#define IR_KEY(Key)              (((key) & IR_KEY_MASK) >> IR_KEY_SHIFT)
#define IR_CHANNEL(Key)          ((IR_Key_t)((key) & IR_CHANNEL_MASK) >> IR_CHANNEL_SHIFT)
#define IR_CHANNEL_AND_KEY_MASK  (IR_CHANNEL_MASK | IR_KEY_MASK)
#define IR_STATUS_MASK           (IR_KEY_PRESSED + IR_KEY_HOLD + IR_KEY_RELEASED)
#define IR_KEY_PRESS_OR_HOLD     (IR_KEY_PRESSED + IR_KEY_HOLD)
#define IR_ANY_KEY_PRESS_OR_HOLD (IR_KEY_PRESS_OR_HOLD + IR_KEY_MASK)
#define IR_ANY_KEY               (IR_ANY_KEY_PRESS_OR_HOLD + IR_KEY_RELEASED)

#if defined(__AVR_ATmega2560__)
    #include <IRremote_Maerklin.h>
#else
    #include <IRremoteESP8266.h>
    #include <IRrecv.h>
    #include <IRutils.h>
#endif

#include <stdarg.h>


class IR_Key_Input_c
{
  public:
                IR_Key_Input_c(uint8_t PinNr);    // Constructor
    IR_Key_t    Read_IR_Key(uint16_t &Duration);
    void        Clear(void);
    void        IR_Test(void);
    void        Init(void);

  private:
    void        Test_IR_Period(void);

    uint8_t     PinNr;
    IRrecv*     irrecv;
    IR_Key_t    LastKey;
    uint32_t    LastCode;
    uint32_t    LastTime;
    uint32_t    FirstTime;
};

#endif // _IR_Key_Input_h_
