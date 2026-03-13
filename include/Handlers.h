#ifndef HANDLERS_H
#define HANDLERS_H

#include "Global.h"

// ================== WEB ROUTE HANDLERS ==================
void handleGetConfig();

void handleSaveWiFi();
void handleSaveMQTT();
void handleSaveHTTP();
void handleSaveModbus();

// ================== CONFIG STORAGE ==================
void saveConfigToEEPROM();
void loadConfigFromEEPROM();

// ================== MQTT ==================
void mqttCallback(char *topic, byte *payload, unsigned int length);

// ================== STATE MACHINE ==================
void processNetState();

#endif