#include "Global.h"

void setupOTA();


//=================== SETUP ==================
void setup()
{
  Serial.begin(115200);
  delay(500);

  Serial.println("\n===== SYSTEM BOOT =====");

  // LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Button
  pinMode(CONFIG_BUTTON_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(CONFIG_BUTTON_PIN), configButtonISR, FALLING);

  // EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadConfigFromEEPROM();
  printCurrentConfig();

  // Modbus hardware
  initModbusHardware();

  // MQTT Setup
  mqttClient.setServer(config.mqttHost, config.mqttPort);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(config.mqttKeepAlive);

  // Start WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.wifiSSID, config.wifiPASS);

  state = STATE_NORMAL_WIFI;

  Serial.println("Device started in NORMAL mode.");
  Serial.println("Hold CONFIG button for 5 sec -> Hotspot Setup Mode");
  Serial.println("===============================");

  setupOTA();
}

//=============================== LOOP ==============================

void loop()
{
  for (int i = 0; i < 5000; i++)
  {
    ArduinoOTA.handle(); // OTA service
    delay(1);
  }

  // Always check config button first
  if (checkConfigButton())
  {
    startHotspotMode();
  }

  // If hotspot mode active
  if (hotspotMode)
  {
    server.handleClient();
    blinkLEDHotspot();
    delay(1);
    return;
  }

  // Normal system operation
  loopConnectionProcess();

  if (mqttClient.connected())
  {
    mqttClient.loop();
  }

  server.handleClient();

  delay(1);
}
