#include "Global.h"

#include "wifiHandler.h"
#include "Handlers.h"
#include "webHandlers.h"
#include "Modbuss.h"

// =====================================================
//              GLOBAL OBJECT DEFINITIONS
// =====================================================


// -------- Network Objects --------
WiFiClient espClient;
PubSubClient mqttClient(espClient);
WebServer server(80);
ModbusMaster node;

// -------- Retry Management --------
int wifiRetry = 0;
int netRetry = 0;
int modbusRetry = 0;

unsigned long lastConnAttempt = 0;
const unsigned long CONN_RETRY_INTERVAL = 2000;

// unsigned long lastPoll = 0;
unsigned long lastPublishTime = 0;

// -------- System State --------
SystemState state = STATE_NORMAL_WIFI;

//MQTT
bool mqttReady = false;
bool httpReady = false;
bool modbusReady = false;
bool hotspotMode = false;

// LOGIN
bool isLoggedIn = false;

// -------- Device Configuration --------
DeviceConfig config;

//===========================CONFIG PRINT FUNCTION ==========================
void printCurrentConfig()
{
  Serial.println("\n================ CURRENT CONFIG ================");

  // ================= WIFI =================
  Serial.println("----------------------------- WIFI");
  Serial.print("WiFi SSID     : ");
  Serial.println(config.wifiSSID);
  Serial.print("WiFi Hidden   : ");
  Serial.println(config.wifiHidden ? "YES" : "NO");
  Serial.print("WiFi EncMode  : ");
  Serial.println(config.wifiEncMode);
  Serial.print("WiFi EncType  : ");
  Serial.println(config.wifiEncType);

  // ================= MQTT =================
  Serial.println("----------------------------- MQTT");
  Serial.print("MQTT Host     : ");
  Serial.println(config.mqttHost);
  Serial.print("MQTT Port     : ");
  Serial.println(config.mqttPort);
  Serial.print("MQTT User     : ");
  Serial.println(config.mqttUser);
  Serial.print("MQTT Topic    : ");
  Serial.println(config.mqttTopic);
  Serial.print("MQTT ClientNm : ");
  Serial.println(config.mqttClientName);
  Serial.print("MQTT ClientID : ");
  Serial.println(config.mqttClientID);
  Serial.print("MQTT QoS      : ");
  Serial.println(config.mqttQoS);
  Serial.print("MQTT KeepAlive: ");
  Serial.println(config.mqttKeepAlive);

  // ================= HTTP =================
  Serial.println("----------------------------- HTTP");
  Serial.print("HTTP URL      : ");
  Serial.println(config.httpURL);
  Serial.print("HTTP Port     : ");
  Serial.println(config.httpPort);

  // ================= MODBUS =================
  Serial.println("----------------------------- MODBUS");
  Serial.print("Modbus SlaveID: ");
  Serial.println(config.slaveID);
  Serial.print("Modbus Baud   : ");
  Serial.println(config.baudrate);
  Serial.print("Modbus Parity : ");
  Serial.println(config.parity);

  // ================= REGISTER GROUPS =================
  Serial.println("----------------------------- REGISTER GROUPS");

  bool anyGroup = false;

  for (int i = 0; i < MAX_REG_GROUPS; i++)
  {
    RegisterGroup &grp = config.groups[i];

    if (!grp.enabled)
      continue;

    anyGroup = true;

    Serial.println("----------------------------------");
    Serial.printf("Group %d\n", i + 1);

    Serial.print("Type          : ");
    Serial.println(strlen(grp.regType) ? grp.regType : "(none)");

    Serial.print("8bit Enabled  : ");
    Serial.println(grp.reg8_enable ? "YES" : "NO");

    if (grp.reg8_enable)
    {
      Serial.print("8bit List     : ");
      Serial.println(strlen(grp.reg8_list) ? grp.reg8_list : "(none)");
    }

    Serial.print("16bit Enabled : ");
    Serial.println(grp.reg16_enable ? "YES" : "NO");

    if (grp.reg16_enable)
    {
      Serial.print("16bit List    : ");
      Serial.println(strlen(grp.reg16_list) ? grp.reg16_list : "(none)");
    }
  }

  if (!anyGroup)
  {
    Serial.println("No register groups enabled.");
  }

  Serial.println("----------------------------- POST INTERVAL");
  //================= POST INTERVAL =================
  Serial.print("Post Interval : ");
  Serial.println(config.postInterval);

  Serial.println("================================================\n");
}

// =====================================================
//              DEFAULT CONFIG
// =====================================================
void setDefaultConfig()
{
  memset(&config, 0, sizeof(config));

  config.magic = CONFIG_MAGIC;
  config.version = CONFIG_VERSION;

  // ================= LOGIN =================
  SAFE_COPY(config.webUser, "admin");
  SAFE_COPY(config.webPass, "admin");

  // ================= WIFI =================
  config.wifiHidden = 0;

  SAFE_COPY(config.wifiSSID, "root");
  SAFE_COPY(config.wifiPASS, "root@123");
  SAFE_COPY(config.wifiEncMode, "WPA2_PERSONAL");
  SAFE_COPY(config.wifiEncType, "AES");

  // ================= MODBUS BASIC =================
  config.slaveID = 1;
  SAFE_COPY(config.parity, "NONE");
  config.baudrate = 9600;

  // ================= DEFAULT REGISTER GROUP =================
  config.groups[0].enabled = 23;

  SAFE_COPY(config.groups[0].regType, "input");

  config.groups[0].reg8_enable = 0;
  SAFE_COPY(config.groups[0].reg8_list, "");

  config.groups[0].reg16_enable = 1;
  SAFE_COPY(config.groups[0].reg16_list, "20,22");

  // ================= HTTP =================
  SAFE_COPY(config.httpURL, "192.168.56.1");
  config.httpPort = 80;

  // ================= MQTT =================
  SAFE_COPY(config.mqttHost, "192.168.56.1");
  config.mqttPort = 1883;

  SAFE_COPY(config.mqttUser, "mqtt_user_iiot");
  SAFE_COPY(config.mqttPass, "root@1");
  SAFE_COPY(config.mqttTopic, "/test");

  SAFE_COPY(config.mqttClientName, "ESP32_NODE");
  SAFE_COPY(config.mqttClientID, "ESP32_AUTO");

  config.mqttQoS = 0;
  config.mqttKeepAlive = 60;

  config.postInterval = 5;

  Serial.println("After reset:");
  Serial.println(config.webUser);
  Serial.println(config.webPass);
}


//--------------------------PROCESS RUNNING STATE-------------------
void processRunningState()
{
  // ------------------------------
  // Maintain MQTT
  // ------------------------------
  if (mqttClient.connected())
    mqttClient.loop();

  // ------------------------------
  // WiFi dependency
  // ------------------------------
  if (WiFi.status() != WL_CONNECTED)
  {
    modbusReady = false;
    state = STATE_NORMAL_WIFI;
    return;
  }

  // ------------------------------
  // Network dependency
  // ------------------------------
  if (!(mqttReady || httpReady))
  {
    modbusReady = false;
    state = STATE_NORMAL_NET;
    return;
  }

  // ------------------------------
  // Continuous Poll Timer
  // ------------------------------
  static unsigned long lastPoll = 0;

  if (millis() - lastPoll < 3000) // 3 sec polling
    return;

  lastPoll = millis();

  // ✅ Use your existing function
  bool success = readModbusAndPublish();

  if (!success)
  {
    static uint8_t failCount = 0;
    failCount++;

    if (failCount >= 5)
    {
      Serial.println("⚠️ Modbus device lost. Returning to MODBUS state.");
      failCount = 0;
      modbusReady = false;
      state = STATE_NORMAL_MODBUS;
    }
  }
}