#include "Handlers.h"
#include "Global.h"
#include "webHandlers.h"

// =================== CHECK LOGIN ==================
bool checkLogin()
{
  if (!isLoggedIn)
  {
    server.send(403, "application/json", "{\"status\":\"UNAUTHORIZED\"}");
    return false;
  }
  return true;
}

//=================== HANDLER LOGIN PAGE ==================
void handleLogin()
{
  String u = server.arg("user");
  String p = server.arg("pass");

  Serial.println("==== LOGIN DEBUG ====");
  Serial.println("Entered User: " + u);
  Serial.println("Entered Pass: " + p);
  Serial.println("Stored User: " + String(config.webUser));
  Serial.println("Stored Pass: " + String(config.webPass));

  if (u == config.webUser && p == config.webPass)
  {
    isLoggedIn = true;
    server.send(200, "application/json", "{\"status\":\"OK\"}");
  }
  else
  {
    server.send(200, "application/json", "{\"status\":\"FAIL\"}");
  }
}

//=================== HANDLER WIFI CONFIG ==================
void handleSaveWiFi()
{
  if (!checkLogin())
    return;

  Serial.println("\n===== /save_wifi HIT =====");

  Serial.print("Args count = ");
  Serial.println(server.args());

  for (int i = 0; i < server.args(); i++)
  {
    Serial.print("ARG[");
    Serial.print(server.argName(i));
    Serial.print("] = ");
    Serial.println(server.arg(i));
  }

  if (server.hasArg("wifiMode"))
  {
    String mode = server.arg("wifiMode");
    config.wifiHidden = (mode == "hidden") ? 1 : 0;
  }

  if (server.hasArg("wifiEncMode"))
    SAFE_COPY(config.wifiEncMode, server.arg("wifiEncMode").c_str());

  if (server.hasArg("wifiEncType"))
    SAFE_COPY(config.wifiEncType, server.arg("wifiEncType").c_str());

  if (server.hasArg("wifiSSID"))
    SAFE_COPY(config.wifiSSID, server.arg("wifiSSID").c_str());

  if (server.hasArg("wifiPASS"))
    SAFE_COPY(config.wifiPASS, server.arg("wifiPASS").c_str());

  if (String(config.wifiEncMode) == "NONE")
    SAFE_COPY(config.wifiPASS, "");

  saveConfigToEEPROM();

  server.send(200, "application/json", "{\"status\":\"OK\"}");
}


//=================== HANDLER MQTT CONFIG ==================
void handleSaveMQTT()
{
  if (!checkLogin())
    return;

  if (server.hasArg("mqttHost"))
    SAFE_COPY(config.mqttHost, server.arg("mqttHost").c_str());

  if (server.hasArg("mqttPort"))
  {
    int port = server.arg("mqttPort").toInt();
    if (port > 0 && port <= 65535)
      config.mqttPort = port;
  }

  if (server.hasArg("mqttUser"))
    SAFE_COPY(config.mqttUser, server.arg("mqttUser").c_str());

  if (server.hasArg("mqttPass"))
    SAFE_COPY(config.mqttPass, server.arg("mqttPass").c_str());

  if (server.hasArg("mqttTopic"))
  {
    String topic = server.arg("mqttTopic");
    if (topic.length() > 0)
      SAFE_COPY(config.mqttTopic, topic.c_str());
  }

  if (server.hasArg("mqttClientName"))
    SAFE_COPY(config.mqttClientName, server.arg("mqttClientName").c_str());

  if (server.hasArg("mqttClientID"))
    SAFE_COPY(config.mqttClientID, server.arg("mqttClientID").c_str());

  if (server.hasArg("mqttQoS"))
  {
    int qos = server.arg("mqttQoS").toInt();
    if (qos >= 0 && qos <= 2)
      config.mqttQoS = qos;
  }

  if (server.hasArg("mqttKeepAlive"))
  {
    int ka = server.arg("mqttKeepAlive").toInt();
    if (ka >= 10 && ka <= 120)
      config.mqttKeepAlive = ka;
  }

  Serial.println("MQTT Config Updated:");
  Serial.println(config.mqttHost);
  Serial.println(config.mqttPort);
  Serial.println(config.mqttTopic);

  saveConfigToEEPROM();
  server.send(200, "application/json", "{\"status\":\"OK\"}");
}

//=================== HANDLER HTTP CONFIG ==================
void handleSaveHTTP()
{
  if (!checkLogin())
    return;

  if (!server.hasArg("httpURL") || !server.hasArg("httpPort"))
  {
    sendStatus(false);
    return;
  }

  String url = server.arg("httpURL");
  int port = server.arg("httpPort").toInt();

  if (url.length() == 0 || port <= 0 || port > 65535)
  {
    sendStatus(false);
    return;
  }

  strlcpy(config.httpURL, url.c_str(), sizeof(config.httpURL));
  config.httpPort = port;

  Serial.println("HTTP Config Updated:");
  Serial.println(config.httpURL);
  Serial.println(config.httpPort);

  saveConfigToEEPROM();
  sendStatus(true);
}

//=================== HANDLER MODBUS CONFIG ==================
void handleSaveModbus()
{
  if (!checkLogin())
    return;

  if (server.hasArg("slaveID"))
    config.slaveID = server.arg("slaveID").toInt();

  if (server.hasArg("baudrate"))
    config.baudrate = server.arg("baudrate").toInt();

  if (server.hasArg("parity"))
    SAFE_COPY(config.parity, server.arg("parity").c_str());

  if (server.hasArg("publishInterval"))
  {
    config.publishInterval = server.arg("publishInterval").toInt();

    if (config.publishInterval < 1)
      config.publishInterval = 1;

    if (config.publishInterval > 3600)
      config.publishInterval = 3600;
  }

  // 🔥 Clear all groups first
  memset(config.groups, 0, sizeof(config.groups));

  // Parse Register Groups
  for (int i = 0; i < MAX_REG_GROUPS; i++)
  {
    String enableKey = "reg_enable_" + String(i);

    if (!server.hasArg(enableKey))
      continue;

    RegisterGroup &grp = config.groups[i];

    grp.enabled = 1;

    String typeKey = "regType_" + String(i);
    if (server.hasArg(typeKey))
      SAFE_COPY(grp.regType, server.arg(typeKey).c_str());

    // 8 bit
    grp.reg8_enable = server.hasArg("reg8_enable_" + String(i)) ? 1 : 0;
    if (grp.reg8_enable && server.hasArg("reg8_list_" + String(i)))
      SAFE_COPY(grp.reg8_list,
                server.arg("reg8_list_" + String(i)).c_str());

    // 16 bit
    grp.reg16_enable = server.hasArg("reg16_enable_" + String(i)) ? 1 : 0;
    if (grp.reg16_enable && server.hasArg("reg16_list_" + String(i)))
      SAFE_COPY(grp.reg16_list,
                server.arg("reg16_list_" + String(i)).c_str());
  }

  saveConfigToEEPROM();
  Serial.println("✅ Modbus configuration saved");

  server.send(200, "application/json", "{\"status\":\"OK\"}");
}

//=================== HANDLER POST INTERVAL CONFIG ==================
void handleSaveInterval()
{
  if (!checkLogin())
    return;

  if (server.hasArg("postInterval"))
  {
    config.postInterval = server.arg("postInterval").toInt();

    saveConfigToEEPROM(); // ✅ FIXED

    lastPublishTime = 0; // ✅ apply immediately

    Serial.println("Post Interval Saved: " + String(config.postInterval));
  }

  server.send(200, "application/json", "{\"status\":\"OK\"}");
}


// ================== HANDLER GET CONFIG ==================
void handleGetConfig()
{
  JsonDocument doc;

  doc["wifiHidden"] = config.wifiHidden;
  doc["wifiSSID"] = config.wifiSSID;
  doc["wifiEncMode"] = config.wifiEncMode;
  doc["wifiEncType"] = config.wifiEncType;

  doc["mqttHost"] = config.mqttHost;
  doc["mqttPort"] = config.mqttPort;
  doc["mqttUser"] = config.mqttUser;
  doc["mqttTopic"] = config.mqttTopic;

  doc["mqttClientName"] = config.mqttClientName;
  doc["mqttClientID"] = config.mqttClientID;
  doc["mqttQoS"] = config.mqttQoS;
  doc["mqttKeepAlive"] = config.mqttKeepAlive;

  doc["httpURL"] = config.httpURL;
  doc["httpPort"] = config.httpPort;

  doc["slaveID"] = config.slaveID;
  doc["baudrate"] = config.baudrate;
  doc["parity"] = config.parity;
  doc["postInterval"] = config.postInterval;

  JsonArray arr = doc["groups"].to<JsonArray>();

  for (int i = 0; i < MAX_REG_GROUPS; i++)
  {
    RegisterGroup &grp = config.groups[i];

    if (!grp.enabled)
      continue;

    JsonObject g = arr.add<JsonObject>();

    g["type"] = grp.regType;
    g["reg8_enable"] = grp.reg8_enable;
    g["reg8"] = grp.reg8_list;
    g["reg16_enable"] = grp.reg16_enable;
    g["reg16"] = grp.reg16_list;
  }

  String out;
  serializeJson(doc, out);

  server.send(200, "application/json", out);
}

// ================== SAVE TO EEPROM ==================
void saveConfigToEEPROM()
{
  config.magic = CONFIG_MAGIC;
  config.version = CONFIG_VERSION;

  EEPROM.put(0, config);

  if (EEPROM.commit())
    Serial.println("✅ Config saved to EEPROM");
  else
    Serial.println("❌ EEPROM commit failed!");
}

void loadConfigFromEEPROM()
{
  EEPROM.get(0, config);

  // Validate magic + version
  if (config.magic != CONFIG_MAGIC ||
      config.version != CONFIG_VERSION)
  {
    Serial.println("⚠️ Invalid config or version mismatch. Resetting...");
    setDefaultConfig();
    saveConfigToEEPROM();
    return;
  }

  Serial.println("✅ Config loaded from EEPROM");

  // 🔒 Sanitize group data
  for (int i = 0; i < MAX_REG_GROUPS; i++)
  {
    config.groups[i].regType[sizeof(config.groups[i].regType) - 1] = '\0';
    config.groups[i].reg8_list[sizeof(config.groups[i].reg8_list) - 1] = '\0';
    config.groups[i].reg16_list[sizeof(config.groups[i].reg16_list) - 1] = '\0';

    if (config.groups[i].enabled > 1)
      config.groups[i].enabled = 0;

    if (config.groups[i].reg8_enable > 1)
      config.groups[i].reg8_enable = 0;

    if (config.groups[i].reg16_enable > 1)
      config.groups[i].reg16_enable = 0;
  }

  // 🔒 Sanitize main strings
  config.wifiSSID[sizeof(config.wifiSSID) - 1] = '\0';
  config.wifiPASS[sizeof(config.wifiPASS) - 1] = '\0';
  config.wifiEncMode[sizeof(config.wifiEncMode) - 1] = '\0';
  config.wifiEncType[sizeof(config.wifiEncType) - 1] = '\0';

  config.httpURL[sizeof(config.httpURL) - 1] = '\0';

  config.mqttHost[sizeof(config.mqttHost) - 1] = '\0';
  config.mqttUser[sizeof(config.mqttUser) - 1] = '\0';
  config.mqttPass[sizeof(config.mqttPass) - 1] = '\0';
  config.mqttTopic[sizeof(config.mqttTopic) - 1] = '\0';
  config.mqttClientName[sizeof(config.mqttClientName) - 1] = '\0';
  config.mqttClientID[sizeof(config.mqttClientID) - 1] = '\0';

  config.parity[sizeof(config.parity) - 1] = '\0';

  if (config.postInterval <= 0 || config.postInterval > 3600)
  {
    Serial.println("⚠️ Invalid postInterval detected. Resetting to default (5 sec)");
    config.postInterval = 5;

    saveConfigToEEPROM();
  }
}

// ================= MQTT CALLBACK =================
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("📩 MQTT Msg on topic: ");
  Serial.println(topic);
}

// ================= NET STATE MACHINE =======================================
void processNetState()
{
  // ===============================
  // WIFI LOST → RESET MQTT CLEANLY
  // ===============================
  if (WiFi.status() != WL_CONNECTED)
  {
    if (mqttClient.connected())
    {
      Serial.println("⚠ WiFi lost → Disconnecting MQTT cleanly");
      mqttClient.disconnect();
    }

    mqttReady = false;
    httpReady = false;

    state = STATE_NORMAL_WIFI;
    return;
  }

  // ===============================
  // MQTT CONFIGURATION
  // ===============================
  mqttClient.setServer(config.mqttHost, config.mqttPort);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(config.mqttKeepAlive);

  // ===============================
  // MQTT RECONNECT TIMER
  // ===============================
  static unsigned long lastMQTTRetry = 0;

  if (!mqttClient.connected())
  {
    if (millis() - lastMQTTRetry > 3000)
    {
      lastMQTTRetry = millis();

      Serial.println("🔄 Attempting MQTT connection...");

      String clientId;

      // -------- ClientID generation --------
      if (strlen(config.mqttClientID) == 0)
      {
        clientId = "ESP32_" + String((uint32_t)ESP.getEfuseMac(), HEX);
      }
      else
      {
        clientId = String(config.mqttClientID);
      }

      bool connected;

      // -------- Connect with or without authentication --------
      if (strlen(config.mqttUser) == 0)
      {
        connected = mqttClient.connect(clientId.c_str());
      }
      else
      {
        connected = mqttClient.connect(
            clientId.c_str(),
            config.mqttUser,
            config.mqttPass);
      }

      if (connected)
      {
        Serial.println("✅ MQTT Connected");
      }
      else
      {
        Serial.print("❌ MQTT Failed, rc=");
        Serial.println(mqttClient.state());
      }
    }
  }

  mqttReady = mqttClient.connected();

  // MQTT keep-alive
  if (mqttClient.connected())
  {
    mqttClient.loop();
  }

  // ===============================
  // HTTP CHECK (every 5 seconds)
  // ===============================
  static unsigned long lastHTTP = 0;

  if (millis() - lastHTTP > 5000)
  {
    lastHTTP = millis();

    HTTPClient http;

    String url = "http://";
    url += String(config.httpURL);

    if (config.httpPort > 0)
    {
      url += ":";
      url += String(config.httpPort);
    }

    url += "/";

    Serial.print("Testing HTTP: ");
    Serial.println(url);

    http.setTimeout(1500);

    if (http.begin(url))
    {
      http.addHeader("Content-Type", "application/json");

      String payload = "{\"status\":\"test\"}";

      // delay(config.postInterval * 1000);

      int code = http.POST(payload);

      Serial.print("HTTP POST code: ");
      Serial.println(code);

      http.end();

      httpReady = (code >= 200 && code < 300);
    }
    else
    {
      Serial.println("HTTP begin failed");
      httpReady = false;
    }
  }

  // ===============================
  // SUCCESS CONDITION
  // ===============================
  if (mqttReady || httpReady)
  {
    Serial.println("✅ MQTT or HTTP OK → Proceeding to Modbus");
    netRetry = 0;
    state = STATE_NORMAL_MODBUS;
    return;
  }

  // ===============================
  // RETRY COUNTER
  // ===============================
  if (millis() - lastConnAttempt > CONN_RETRY_INTERVAL)
  {
    lastConnAttempt = millis();
    netRetry++;

    Serial.printf("🔄 MQTT/HTTP connecting | Retry %d/%d\n",
                  netRetry, MAX_RETRIES);
  }

  if (netRetry >= MAX_RETRIES)
  {
    Serial.println("🔥 MQTT + HTTP failed. Restarting...");
    delay(500);
    ESP.restart();
  }
}