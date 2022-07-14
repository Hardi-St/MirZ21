/*
 * Z21type.h
 * Created on: 16.04.2015
 *
 * Define param for Z21 client handle
 *
 * Copyright (c) by Philipp Gahtow, year 2021
*/
//**************************************************************
//Client Configuration:

//Speichergroesse/Slot size:
#if defined(__AVR_ATmega1284P__)
   #define LANmaxIP 15    //max IP-Adressen (max Clients)
   #define WLANmaxIP 15   //Anzahl Clients ueber ATmega1284P
#elif defined(MEGA_MCU)   //Arduino MEGA
  #define LANmaxIP 15
  #define WLANmaxIP 15    //Anzahl Clients ueber Arduino MEGA
#elif defined (DUE_MCU)
  #define LANmaxIP 20
  #define WLANmaxIP 20    //Anzahl Clients ueber DUE
#elif defined (__AVR_ATmega644P__)
  #define LANmaxIP 15
  #define WLANmaxIP 15    //Anzahl Clients ueber ATmega644P
#elif defined(ESP8266_MCU) || defined(ESP32_MCU)
  #define LANmaxIP 50     //Clients auf ESP werden als LAN behandelt                                          // 10.07.22:  Old: 20, uses 588 Bytes Ram
  #define WLANmaxIP 0     //no support for Ethernet Shield on ESP!
#else   //Arduino UNO
  #define LANmaxIP 5
  #define WLANmaxIP 10    //Anzahl Clients ueber UNO
#endif

#define ActTimeIP 10      //Aktivhaltung einer IP fuer (sec./2)
#define IPinterval 4000   //Interval in milliseconds for checking IP aktiv state





