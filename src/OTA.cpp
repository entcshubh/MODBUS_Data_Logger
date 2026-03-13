#include "Global.h"


void setupOTA()
{
    ArduinoOTA.setHostname("ESP32_MODBUS_NODE");

    ArduinoOTA.setPassword("root@1");

    ArduinoOTA.onStart([]()
                       { Serial.println("OTA Update Started"); });

    ArduinoOTA.onEnd([]()
                     { Serial.println("\nOTA Update Finished"); });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("OTA Progress: %u%%\n", (progress * 100) / total); });

    ArduinoOTA.onError([](ota_error_t error)
                       { Serial.printf("OTA Error[%u]\n", error); });

    ArduinoOTA.begin();

    Serial.println("OTA Ready");
}