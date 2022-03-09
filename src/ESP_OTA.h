//--------------------------------------------------------------
/*

  ESP OTA Upload Support
  -> Only for ESP8266 and ESP32 Central Station!

  Copyright (c) by Philipp Gahtow, year 2022
  
*/

#if (defined(ESP8266_MCU)  || defined(ESP32_MCU)) && defined(ESP_OTA)

#if defined(Z21DISPLAY)
extern void DisplayOTAStart(void);
extern void DisplayOTAProgress(uint8_t state);
extern void DisplayOTAFinish(void);
#endif

void ESP_OTA_Init() {
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("Z21pg");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  
   ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    #if defined(DEBUG)
    Debug.println("Start updating " + type);  //OTA Update Start
    #endif
    #if defined(Z21DISPLAY)
    DisplayOTAStart();
    #endif
  });
  ArduinoOTA.onEnd([]() {
    #if defined(DEBUG)
    Debug.println("\nEnd"); //OTA Update Finish
    #endif
    #if defined(Z21DISPLAY)
    DisplayOTAFinish();
    #endif
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    #if defined(DEBUG)
    Debug.printf("Progress: %u%%", (progress / (total / 100))); // //OTA Update Running
    Debug.println();
    #endif
    #if defined(Z21DISPLAY)
    DisplayOTAProgress(progress / (total / 100));
    #endif
  });
  ArduinoOTA.onError([](ota_error_t error) {
    #if defined(DEBUG)
    Debug.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Debug.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Debug.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Debug.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Debug.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Debug.println("End Failed");
    }
    #endif
  });
  ArduinoOTA.begin();
    
}

#endif
