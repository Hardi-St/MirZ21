#ifndef _IR_Key_Input_h_
#define _IR_Key_Input_h_

#define USE_GRUNDIG // Enable this line to use the Grundig RC-YC1 IR Remote control
#define USE_SAMSUNG // Enable this line to use the Samsung BN59-01301A IR Remote control                      // 23.07.22:

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

#ifdef USE_GRUNDIG
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

  // Samsung keys
  #define IR_KEY_S_NR0               100
  #define IR_KEY_S_NR1               101
  #define IR_KEY_S_NR2               102
  #define IR_KEY_S_NR3               103
  #define IR_KEY_S_NR4               104
  #define IR_KEY_S_NR5               105
  #define IR_KEY_S_NR6               106
  #define IR_KEY_S_NR7               107
  #define IR_KEY_S_NR8               108
  #define IR_KEY_S_NR9               109
  #define IR_KEY_S_POWER             110
  #define IR_KEY_S_SOURCE            111
  #define IR_KEY_S_MINUS             112
  #define IR_KEY_S_PRE_CH            113
  #define IR_KEY_S_VOL_PLUS          114
  #define IR_KEY_S_MUTE              115
  #define IR_KEY_S_CH_PLUS           116
  #define IR_KEY_S_VOL_MINUS         117
  #define IR_KEY_S_CH_LIST           118
  #define IR_KEY_S_CH_MINUS          119
  #define IR_KEY_S_SLEEP             120
  #define IR_KEY_S_HOME              121
  #define IR_KEY_S_GUIDE             122
  #define IR_KEY_S_SETTINGS          123
  #define IR_KEY_S_CURSOR_UP         124
  #define IR_KEY_S_INFO              125
  #define IR_KEY_S_CURSOR_LEFT       126
  #define IR_KEY_S_CURSOR_CENTER     127
  #define IR_KEY_S_CURSOR_RIGHT      128
  #define IR_KEY_S_RETURN            129
  #define IR_KEY_S_CURSOR_DOWN       130
  #define IR_KEY_S_EXIT              131
  #define IR_KEY_S_A                 132
  #define IR_KEY_S_B                 133
  #define IR_KEY_S_C                 134
  #define IR_KEY_S_D                 135
  #define IR_KEY_S_E_MANUAL          136
  #define IR_KEY_S_P_SIZE            137
  #define IR_KEY_S_CC_VD             138
  #define IR_KEY_S_STOP              139
  #define IR_KEY_S_FAST_REVERSE      140
  #define IR_KEY_S_PLAY              141
  #define IR_KEY_S_PAUSE             142
  #define IR_KEY_S_FAST_FORWARD      143

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

  #define IR_KEY_S_PRESS_NR0                (IR_KEY_S_NR0           + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_NR1                (IR_KEY_S_NR1           + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_NR2                (IR_KEY_S_NR2           + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_NR3                (IR_KEY_S_NR3           + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_NR4                (IR_KEY_S_NR4           + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_NR5                (IR_KEY_S_NR5           + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_NR6                (IR_KEY_S_NR6           + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_NR7                (IR_KEY_S_NR7           + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_NR8                (IR_KEY_S_NR8           + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_NR9                (IR_KEY_S_NR9           + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_POWER              (IR_KEY_S_POWER         + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_SOURCE             (IR_KEY_S_SOURCE        + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_MINUS              (IR_KEY_S_MINUS         + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_PRE_CH             (IR_KEY_S_PRE_CH        + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_VOL_PLUS           (IR_KEY_S_VOL_PLUS      + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_MUTE               (IR_KEY_S_MUTE          + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_CH_PLUS            (IR_KEY_S_CH_PLUS       + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_VOL_MINUS          (IR_KEY_S_VOL_MINUS     + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_CH_LIST            (IR_KEY_S_CH_LIST       + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_CH_MINUS           (IR_KEY_S_CH_MINUS      + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_SLEEP              (IR_KEY_S_SLEEP         + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_HOME               (IR_KEY_S_HOME          + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_GUIDE              (IR_KEY_S_GUIDE         + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_SETTINGS           (IR_KEY_S_SETTINGS      + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_CURSOR_UP          (IR_KEY_S_CURSOR_UP     + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_INFO               (IR_KEY_S_INFO          + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_CURSOR_LEFT        (IR_KEY_S_CURSOR_LEFT   + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_CURSOR_CENTER      (IR_KEY_S_CURSOR_CENTER + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_CURSOR_RIGHT       (IR_KEY_S_CURSOR_RIGHT  + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_RETURN             (IR_KEY_S_RETURN        + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_CURSOR_DOWN        (IR_KEY_S_CURSOR_DOWN   + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_EXIT               (IR_KEY_S_EXIT          + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_A                  (IR_KEY_S_A             + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_B                  (IR_KEY_S_B             + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_C                  (IR_KEY_S_C             + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_D                  (IR_KEY_S_D             + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_E_MANUAL           (IR_KEY_S_E_MANUAL      + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_P_SIZE             (IR_KEY_S_P_SIZE        + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_CC_VD              (IR_KEY_S_CC_VD         + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_STOP               (IR_KEY_S_STOP          + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_FAST_REVERSE       (IR_KEY_S_FAST_REVERSE  + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_PLAY               (IR_KEY_S_PLAY          + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_PAUSE              (IR_KEY_S_PAUSE         + IR_KEY_PRESSED)
  #define IR_KEY_S_PRESS_FAST_FORWARD       (IR_KEY_S_FAST_FORWARD  + IR_KEY_PRESSED)

  #define IR_KEY_S_HOLD_NR0                (IR_KEY_S_NR0           + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_NR1                (IR_KEY_S_NR1           + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_NR2                (IR_KEY_S_NR2           + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_NR3                (IR_KEY_S_NR3           + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_NR4                (IR_KEY_S_NR4           + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_NR5                (IR_KEY_S_NR5           + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_NR6                (IR_KEY_S_NR6           + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_NR7                (IR_KEY_S_NR7           + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_NR8                (IR_KEY_S_NR8           + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_NR9                (IR_KEY_S_NR9           + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_POWER              (IR_KEY_S_POWER         + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_SOURCE             (IR_KEY_S_SOURCE        + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_MINUS              (IR_KEY_S_MINUS         + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_PRE_CH             (IR_KEY_S_PRE_CH        + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_VOL_PLUS           (IR_KEY_S_VOL_PLUS      + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_MUTE               (IR_KEY_S_MUTE          + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_CH_PLUS            (IR_KEY_S_CH_PLUS       + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_VOL_MINUS          (IR_KEY_S_VOL_MINUS     + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_CH_LIST            (IR_KEY_S_CH_LIST       + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_CH_MINUS           (IR_KEY_S_CH_MINUS      + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_SLEEP              (IR_KEY_S_SLEEP         + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_HOME               (IR_KEY_S_HOME          + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_GUIDE              (IR_KEY_S_GUIDE         + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_SETTINGS           (IR_KEY_S_SETTINGS      + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_CURSOR_UP          (IR_KEY_S_CURSOR_UP     + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_INFO               (IR_KEY_S_INFO          + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_CURSOR_LEFT        (IR_KEY_S_CURSOR_LEFT   + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_CURSOR_CENTER      (IR_KEY_S_CURSOR_CENTER + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_CURSOR_RIGHT       (IR_KEY_S_CURSOR_RIGHT  + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_RETURN             (IR_KEY_S_RETURN        + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_CURSOR_DOWN        (IR_KEY_S_CURSOR_DOWN   + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_EXIT               (IR_KEY_S_EXIT          + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_A                  (IR_KEY_S_A             + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_B                  (IR_KEY_S_B             + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_C                  (IR_KEY_S_C             + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_D                  (IR_KEY_S_D             + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_E_MANUAL           (IR_KEY_S_E_MANUAL      + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_P_SIZE             (IR_KEY_S_P_SIZE        + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_CC_VD              (IR_KEY_S_CC_VD         + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_STOP               (IR_KEY_S_STOP          + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_FAST_REVERSE       (IR_KEY_S_FAST_REVERSE  + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_PLAY               (IR_KEY_S_PLAY          + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_PAUSE              (IR_KEY_S_PAUSE         + IR_KEY_HOLD)
  #define IR_KEY_S_HOLD_FAST_FORWARD       (IR_KEY_S_FAST_FORWARD  + IR_KEY_HOLD)

  #define IR_KEY_S_RELEASED_NR0                (IR_KEY_S_NR0           + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_NR1                (IR_KEY_S_NR1           + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_NR2                (IR_KEY_S_NR2           + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_NR3                (IR_KEY_S_NR3           + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_NR4                (IR_KEY_S_NR4           + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_NR5                (IR_KEY_S_NR5           + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_NR6                (IR_KEY_S_NR6           + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_NR7                (IR_KEY_S_NR7           + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_NR8                (IR_KEY_S_NR8           + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_NR9                (IR_KEY_S_NR9           + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_POWER              (IR_KEY_S_POWER         + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_SOURCE             (IR_KEY_S_SOURCE        + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_MINUS              (IR_KEY_S_MINUS         + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_PRE_CH             (IR_KEY_S_PRE_CH        + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_VOL_PLUS           (IR_KEY_S_VOL_PLUS      + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_MUTE               (IR_KEY_S_MUTE          + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_CH_PLUS            (IR_KEY_S_CH_PLUS       + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_VOL_MINUS          (IR_KEY_S_VOL_MINUS     + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_CH_LIST            (IR_KEY_S_CH_LIST       + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_CH_MINUS           (IR_KEY_S_CH_MINUS      + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_SLEEP              (IR_KEY_S_SLEEP         + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_HOME               (IR_KEY_S_HOME          + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_GUIDE              (IR_KEY_S_GUIDE         + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_SETTINGS           (IR_KEY_S_SETTINGS      + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_CURSOR_UP          (IR_KEY_S_CURSOR_UP     + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_INFO               (IR_KEY_S_INFO          + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_CURSOR_LEFT        (IR_KEY_S_CURSOR_LEFT   + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_CURSOR_CENTER      (IR_KEY_S_CURSOR_CENTER + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_CURSOR_RIGHT       (IR_KEY_S_CURSOR_RIGHT  + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_RETURN             (IR_KEY_S_RETURN        + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_CURSOR_DOWN        (IR_KEY_S_CURSOR_DOWN   + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_EXIT               (IR_KEY_S_EXIT          + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_A                  (IR_KEY_S_A             + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_B                  (IR_KEY_S_B             + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_C                  (IR_KEY_S_C             + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_D                  (IR_KEY_S_D             + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_E_MANUAL           (IR_KEY_S_E_MANUAL      + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_P_SIZE             (IR_KEY_S_P_SIZE        + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_CC_VD              (IR_KEY_S_CC_VD         + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_STOP               (IR_KEY_S_STOP          + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_FAST_REVERSE       (IR_KEY_S_FAST_REVERSE  + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_PLAY               (IR_KEY_S_PLAY          + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_PAUSE              (IR_KEY_S_PAUSE         + IR_KEY_RELEASED)
  #define IR_KEY_S_RELEASED_FAST_FORWARD       (IR_KEY_S_FAST_FORWARD  + IR_KEY_RELEASED)


  // Universal names used for the keys in switch/case statemants.
  // This keys must be defined for each IR control
  // If several keys should be used  ": case" must be added in between
  // Example:  IR_KEY_PRESS_MUTE  : case IR_KEY_G_PRESS_16_9
  #define IR_KEY_TV_PRESS_NR0        IR_KEY_G_PRESS_NR0       : case IR_KEY_S_PRESS_NR0
  #define IR_KEY_TV_PRESS_NR1        IR_KEY_G_PRESS_NR1       : case IR_KEY_S_PRESS_NR1
  #define IR_KEY_TV_PRESS_NR2        IR_KEY_G_PRESS_NR2       : case IR_KEY_S_PRESS_NR2
  #define IR_KEY_TV_PRESS_NR3        IR_KEY_G_PRESS_NR3       : case IR_KEY_S_PRESS_NR3
  #define IR_KEY_TV_PRESS_NR4        IR_KEY_G_PRESS_NR4       : case IR_KEY_S_PRESS_NR4
  #define IR_KEY_TV_PRESS_NR5        IR_KEY_G_PRESS_NR5       : case IR_KEY_S_PRESS_NR5
  #define IR_KEY_TV_PRESS_NR6        IR_KEY_G_PRESS_NR6       : case IR_KEY_S_PRESS_NR6
  #define IR_KEY_TV_PRESS_NR7        IR_KEY_G_PRESS_NR7       : case IR_KEY_S_PRESS_NR7
  #define IR_KEY_TV_PRESS_NR8        IR_KEY_G_PRESS_NR8       : case IR_KEY_S_PRESS_NR8
  #define IR_KEY_TV_PRESS_NR9        IR_KEY_G_PRESS_NR9       : case IR_KEY_S_PRESS_NR9
  #define IR_KEY_TV_SWITCH_RED       IR_KEY_G_PRESS_REPEAT    : case IR_KEY_S_PRESS_PRE_CH
  #define IR_KEY_TV_SWITCH_GREEN     IR_KEY_G_PRESS_16_9      : case IR_KEY_S_PRESS_MINUS
  #define IR_KEY_TV_SWITCH_TOGGLE    IR_KEY_G_PRESS_A_B       : case IR_KEY_S_PRESS_MUTE
  #define IR_KEY_TV_SWITCH_CNG_DISP  IR_KEY_G_PRESS_MUTE      : case IR_KEY_S_PRESS_SOURCE
  #define IR_KEY_TV_SWITCH_RST_DISP  IR_KEY_G_HOLD_MUTE       : case IR_KEY_S_HOLD_SOURCE
  #define IR_KEY_TV_FUNCKEY          IR_KEY_G_PRESS_EPG       : case IR_KEY_S_PRESS_CH_LIST : case IR_KEY_S_PRESS_RETURN
  #define IR_NR_TYP_HOLD_FUNCKEY     IR_KEY_G_HOLD_EPG        : case IR_KEY_S_HOLD_CH_LIST  : case IR_KEY_S_HOLD_RETURN
  #define IR_KEY_TV_RELEASED_FUNCKEY IR_KEY_G_RELEASED_EPG    : case IR_KEY_S_RELEASED_CH_LIST
  #define IR_KEY_TV_NEXT_SWITCH      IR_KEY_G_PRESS_V_PLUS    : case IR_KEY_S_PRESS_VOL_PLUS
  #define IR_KEY_TV_PREV_SWITCH      IR_KEY_G_PRESS_V_MINUS   : case IR_KEY_S_PRESS_VOL_MINUS
  #define IR_KEY_TV_NEXT_LOK         IR_KEY_G_PRESS_PR_PLUS   : case IR_KEY_S_PRESS_CH_PLUS
  #define IR_KEY_TV_PREV_LOK         IR_KEY_G_PRESS_PR_MINUS  : case IR_KEY_S_PRESS_CH_MINUS
  #define IR_KEY_TV_FORWARD          IR_KEY_G_PRESS_RIGHT     : case IR_KEY_S_PRESS_CURSOR_RIGHT
  #define IR_KEY_TV_BACKWARD         IR_KEY_G_PRESS_LEFT      : case IR_KEY_S_PRESS_CURSOR_LEFT
  #define IR_KEY_TV_FASTER           IR_KEY_G_PRESS_UP        : case IR_KEY_S_PRESS_CURSOR_UP
  #define IR_KEY_TV_SLOWER           IR_KEY_G_PRESS_DOWN      : case IR_KEY_S_PRESS_CURSOR_DOWN
  #define IR_KEY_TV_HOLD_FASTER      IR_KEY_G_HOLD_UP         : case IR_KEY_S_HOLD_CURSOR_UP
  #define IR_KEY_TV_HOLD_SLOWER      IR_KEY_G_HOLD_DOWN       : case IR_KEY_S_HOLD_CURSOR_DOWN
  #define IR_KEY_TV_STOP_FUNC        IR_KEY_G_PRESS_OK        : case IR_KEY_S_PRESS_CURSOR_CENTER
  #define IR_KEY_TV_EMERGENCY_STOP   IR_KEY_G_HOLD_OK         : case IR_KEY_S_HOLD_CURSOR_CENTER
  #define IR_KEY_TV_FN0              IR_KEY_G_PRESS_FIND      : case IR_KEY_S_PRESS_A
  #define IR_KEY_TV_FN1              IR_KEY_G_PRESS_M         : case IR_KEY_S_PRESS_B
  #define IR_KEY_TV_FN2              IR_KEY_G_PRESS_BACK      : case IR_KEY_S_PRESS_C
  #define IR_KEY_TV_FN3              IR_KEY_G_PRESS_i         : case IR_KEY_S_PRESS_D
  #define IR_KEY_TV_FN4              IR_KEY_G_PRESS_RED       : case IR_KEY_S_PRESS_E_MANUAL
  #define IR_KEY_TV_FN5              IR_KEY_G_PRESS_GREEN     : case IR_KEY_S_PRESS_P_SIZE
  #define IR_KEY_TV_FN6              IR_KEY_G_PRESS_YELLOW    : case IR_KEY_S_PRESS_CC_VD
  #define IR_KEY_TV_FN7              IR_KEY_G_PRESS_BLUE      : case IR_KEY_S_PRESS_STOP
  #define IR_KEY_TV_FN8              IR_KEY_G_PRESS_TXT       : case IR_KEY_S_PRESS_FAST_REVERSE
  #define IR_KEY_TV_FN9              IR_KEY_G_PRESS_R         : case IR_KEY_S_PRESS_PLAY
  #define IR_KEY_TV_FN10             IR_KEY_G_PRESS_BOX       : case IR_KEY_S_PRESS_PAUSE
  #define IR_KEY_TV_FN11             IR_KEY_G_PRESS_SPEAKER   : case IR_KEY_S_PRESS_FAST_FORWARD
  #define IR_KEY_TV_RELEASED_FN0     IR_KEY_G_RELEASED_FIND   : case IR_KEY_S_RELEASED_A
  #define IR_KEY_TV_RELEASED_FN1     IR_KEY_G_RELEASED_M      : case IR_KEY_S_RELEASED_B
  #define IR_KEY_TV_RELEASED_FN2     IR_KEY_G_RELEASED_BACK   : case IR_KEY_S_RELEASED_C           // OK
  #define IR_KEY_TV_RELEASED_FN3     IR_KEY_G_RELEASED_i      : case IR_KEY_S_RELEASED_D
  #define IR_KEY_TV_RELEASED_FN4     IR_KEY_G_RELEASED_RED    : case IR_KEY_S_RELEASED_E_MANUAL
  #define IR_KEY_TV_RELEASED_FN5     IR_KEY_G_RELEASED_GREEN  : case IR_KEY_S_RELEASED_P_SIZE
  #define IR_KEY_TV_RELEASED_FN6     IR_KEY_G_RELEASED_YELLOW : case IR_KEY_S_RELEASED_CC_VD
  #define IR_KEY_TV_RELEASED_FN7     IR_KEY_G_RELEASED_BLUE   : case IR_KEY_S_RELEASED_STOP
  #define IR_KEY_TV_RELEASED_FN8     IR_KEY_G_RELEASED_TXT    : case IR_KEY_S_RELEASED_FAST_REVERSE
  #define IR_KEY_TV_RELEASED_FN9     IR_KEY_G_RELEASED_R      : case IR_KEY_S_RELEASED_PLAY
  #define IR_KEY_TV_RELEASED_FN10    IR_KEY_G_RELEASED_BOX    : case IR_KEY_S_RELEASED_PAUSE
  #define IR_KEY_TV_RELEASED_FN11    IR_KEY_G_RELEASED_SPEAKER: case IR_KEY_S_RELEASED_FAST_FORWARD

  #define IR_KEY_TV_YES              IR_KEY_G_PRESS_OK        : case IR_KEY_S_PRESS_CURSOR_CENTER
  #define IR_KEY_TV_TRACKPOWER       IR_KEY_G_PRESS_POWER     : case IR_KEY_S_PRESS_POWER

#endif // USE_GRUNDIG


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
#define IR_CHANNEL_14            0x0E00
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
