#include "loopNsetup.h"

//============================= SETUP FUCN ====================================

// void hardwareSetup()
// {
//     // LED
//     pinMode(LED_PIN, OUTPUT);
//     digitalWrite(LED_PIN, LOW);

//     // Button
//     pinMode(CONFIG_BUTTON_PIN, INPUT);

//     // EEPROM
//     EEPROM.begin(EEPROM_SIZE);

// }

// //-----------------------------------------------------------------------------
// void postingSetup()
// {
//     // MQTT Setup
//     mqttClient.setServer(config.mqttHost, config.mqttPort);
//     mqttClient.setCallback(mqttCallback);
//     mqttClient.setKeepAlive(config.mqttKeepAlive);

//     // Start WiFi
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(config.wifiSSID, config.wifiPASS);

//     state = STATE_NORMAL_WIFI;

//     Serial.println("Device started in NORMAL mode.");
//     Serial.println("Hold CONFIG button for 5 sec -> Hotspot Setup Mode");
//     Serial.println("===============================");
// }

//============================= LOOP FUCN ====================================

// void pressButtonLoop()
// {
//     // ================= BUTTON =================
//     if (checkConfigButton())
//     {
//         startHotspotMode();
//     }

//     // ================= HOTSPOT =================
//     if (hotspotMode)
//     {
//         server.handleClient();
//         blinkLEDHotspot();
//         delay(1);
//         return;
//     }
// }


//-------------------------------------------------------------------------
// void loopBootConfig()
// {
//     // ================= STATE MACHINE =================
//     switch (state)
//     {
//     // -------- WIFI --------
//     case STATE_NORMAL_WIFI:
//         processWiFiState();
//         break;

//     // -------- NETWORK --------
//     case STATE_NORMAL_NET:
//         processNetState();
//         break;

//     // -------- MODBUS INIT --------
//     case STATE_NORMAL_MODBUS:
//         processModbusState();
//         break;

//     // -------- RUNNING --------
//     case STATE_RUNNING:
//     {
//         // ================= MQTT KEEP ALIVE =================
//         if (mqttClient.connected())
//             mqttClient.loop();

//         // ================= WIFI CHECK =================
//         if (WiFi.status() != WL_CONNECTED)
//         {
//             Serial.println("⚠️ WiFi dropped -> restarting WiFi");

//             mqttReady = false;
//             httpReady = false;
//             modbusReady = false;

//             state = STATE_NORMAL_WIFI;
//             break;
//         }

//         // ================= NETWORK CHECK =================
//         if (!mqttClient.connected())
//             mqttReady = false;

//         if (!(mqttReady || httpReady))
//         {
//             Serial.println("⚠️ Network lost -> restarting");

//             modbusReady = false;
//             state = STATE_NORMAL_NET;
//             break;
//         }

//         // ================= MODBUS CHECK =================
//         if (!modbusReady)
//         {
//             state = STATE_NORMAL_MODBUS;
//             break;
//         }

//         // ================= INTERVAL LOGIC 🔥 =================
//         static uint8_t failCount = 0;

//         if ((unsigned long)(millis() - lastPublishTime) >= (config.postInterval * 1000))
//         {
//             lastPublishTime = millis();

//             Serial.println("⏱ Interval Triggered");

//             bool success = readModbusAndPublish();

//             if (!success)
//             {
//                 failCount++;
//                 Serial.printf("❌ Publish failed (%d)\n", failCount);

//                 if (failCount >= 5)
//                 {
//                     Serial.println("⚠️ Modbus lost -> restarting MODBUS");
//                     failCount = 0;
//                     modbusReady = false;
//                     state = STATE_NORMAL_MODBUS;
//                 }
//             }
//             else
//             {
//                 Serial.println("✅ Data Sent");
//                 failCount = 0;
//             }
//         }
//     }
//     break;

//     // -------- HOTSPOT STATE --------
//     case STATE_HOTSPOT:
//         break;
//     }
// }
