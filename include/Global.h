#ifndef GLOBAL_H
#define GLOBAL_H

// ================== LIBRARIES ==================
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ModbusMaster.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#include "wifiHandler.h"
#include "Handlers.h"
#include "webHandlers.h"
#include "Modbuss.h"
#include "OTA.h"

// ================== FUNCTION PROTOTYPES ==================
void setDefaultConfig();
void loopBootConfig();
void loopConnectionProcess();
void printCurrentConfig();

// ================== PIN CONFIG ==================
#define RX_PIN 16
#define TX_PIN 17

#define MAX485_DE 4
#define MAX485_RE_NEG 4

#define CONFIG_BUTTON_PIN 34
#define LED_PIN 33

#define MAX_RETRIES 200

#define MAX_REG_GROUPS 5
#define MAX_REG_LIST 80



// ================== EEPROM ==================
#define EEPROM_SIZE 2048
#define CONFIG_MAGIC 0xA55A5678
#define CONFIG_VERSION 2

// ================== HOTSPOT ==================
// #define AP_SSID "NODE_SETUP"
#define AP_PASS "12345678"

// ================== SAFE STRING COPY ==================
#define SAFE_COPY(dst, src)             \
  do                                    \
  {                                     \
    strncpy(dst, src, sizeof(dst) - 1); \
    dst[sizeof(dst) - 1] = '\0';        \
  } while (0)

// ================== SYSTEM STATE ==================
enum SystemState
{
  STATE_NORMAL_WIFI,
  STATE_NORMAL_NET,
  STATE_NORMAL_MODBUS,
  STATE_RUNNING,
  STATE_HOTSPOT
};

typedef struct
{
  uint8_t enabled;              

  char regType[12];             // input / holding / coil / discrete

  uint8_t reg8_enable;
  char reg8_list[MAX_REG_LIST];

  uint8_t reg16_enable;
  char reg16_list[MAX_REG_LIST];

} RegisterGroup;

// ================== CONFIG STRUCT ==================
typedef struct
{
  uint32_t magic;
  uint16_t version;   

  uint8_t wifiHidden;

  uint8_t slaveID;

  char parity[10];
  uint32_t baudrate;

  uint16_t publishInterval;

  RegisterGroup groups[MAX_REG_GROUPS];

  char wifiSSID[32];
  char wifiPASS[32];
  char wifiEncMode[20];
  char wifiEncType[10];

  char httpURL[80];
  uint16_t httpPort;

  char mqttHost[50];
  uint16_t mqttPort;
  char mqttUser[32];
  char mqttPass[32];
  char mqttTopic[50];

  char mqttClientName[32];
  char mqttClientID[40];
  uint8_t mqttQoS;
  uint16_t mqttKeepAlive;

} DeviceConfig;


// ---- Network Objects ----
extern WiFiClient espClient;
extern PubSubClient mqttClient;
extern WebServer server;
extern ModbusMaster node;


// ---- Retry Management ----
extern int wifiRetry;
extern int netRetry;
extern int modbusRetry;

extern unsigned long lastConnAttempt;
extern const unsigned long CONN_RETRY_INTERVAL;

extern unsigned long lastPoll;

// ---- System State ----
extern SystemState state;

extern bool mqttReady;
extern bool httpReady;
extern bool modbusReady;
extern bool hotspotMode;

// ---- Device Configuration ----
extern DeviceConfig config;

void processRunningState();

#endif