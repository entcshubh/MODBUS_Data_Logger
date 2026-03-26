#include "webHandlers.h"

String generateHotspotSSID()
{
  String mac = WiFi.macAddress(); // 24:A1:60:3F:9C:12
  mac.replace(":", "");           // 24A1603F9C12

  String ssid = "NODE_";
  ssid += mac;

  return ssid;
}


void handleSystemStatus()
{
  JsonDocument doc;

  doc["wifi"] = (WiFi.status() == WL_CONNECTED);
  doc["mqtt"] = mqttClient.connected();
  doc["modbus"] = modbusReady;
  doc["state"] = state;

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}


// ================= BUTTON HOLD CHECK =================
bool checkConfigButton()
{
  static unsigned long pressStartTime = 0;
  static bool isPressing = false;

  int buttonReading = digitalRead(CONFIG_BUTTON_PIN);

  // Button pressed (LOW because external pull-up)
  if (buttonReading == HIGH)
  {
    if (!isPressing)
    {
      isPressing = true;
      pressStartTime = millis();
      Serial.println("Config button pressed...");
    }
    else
    {
      if (millis() - pressStartTime >= 5000)
      {
        Serial.println("🔥 Config button held 5s -> Hotspot Mode");

        // wait for release
        while (digitalRead(CONFIG_BUTTON_PIN) == HIGH)
        {
          delay(10);
        }

        isPressing = false;
        return true;
      }
    }
  }
  else
  {
    if (isPressing)
    {
      Serial.println("Button released too early");
      isPressing = false;
    }
  }

  return false;
}


// ================= LED BLINK =================
void blinkLEDHotspot()
{
  static unsigned long lastToggle = 0;
  static bool ledState = false;

  if (millis() - lastToggle >= 200)
  {
    lastToggle = millis();
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}

// ================= WEB ROUTES =================
void setupWebServerRoutes()
{
  server.on("/", HTTP_GET, handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  
  server.on("/get_config", HTTP_GET, handleGetConfig);

  server.on("/save_wifi", HTTP_POST, handleSaveWiFi);
  server.on("/save_mqtt", HTTP_POST, handleSaveMQTT);
  server.on("/save_http", HTTP_POST, handleSaveHTTP);
  server.on("/save_modbus", HTTP_POST, handleSaveModbus);
  server.on("/save_interval", HTTP_POST, handleSaveInterval);
  server.on("/status", handleSystemStatus);

  server.on("/apply_now", HTTP_POST, handleApplyNow);

  server.onNotFound([]()
                    { server.send(404, "text/plain", "Not found"); });
}

// ================= HOTSPOT MODE =================
void startHotspotMode()
{
  if (hotspotMode)
    return;

  Serial.println("\n🔥 Config button held -> Hotspot Mode");

  hotspotMode = true;
  state = STATE_HOTSPOT;

  // Stop active services
  mqttClient.disconnect();
  WiFi.disconnect(true);

  mqttReady = false;
  httpReady = false;
  modbusReady = false;

  delay(200);

  // Start AP
  WiFi.mode(WIFI_AP);

  IPAddress local_ip(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.softAPConfig(local_ip, gateway, subnet);
  String apSSID = generateHotspotSSID();

  WiFi.softAP(apSSID.c_str(), AP_PASS);

  Serial.print("Hotspot Name: ");
  Serial.println(apSSID);

  Serial.println("✅ Hotspot started");
  Serial.print("Open: http://");
  Serial.println(WiFi.softAPIP());

  setupWebServerRoutes();
  server.begin();

  Serial.println("🌐 Web server running");
}

// ================= APPLY NOW =================
void handleApplyNow()
{
  server.send(200, "text/plain", "OK. Restarting...");
  delay(200);
  ESP.restart();
}

// ================= STATUS RESPONSE =================
void sendStatus(bool ok)
{
  if (ok)
    server.send(200, "application/json", "{\"status\":\"OK\"}");
  else
    server.send(200, "application/json", "{\"status\":\"FAIL\"}");
}