#ifndef WEBHANDLERS_H
#define WEBHANDLERS_H

#include "Global.h"
#include "Handlers.h"
#include "WebPage.h"

// ================= BUTTON / ISR =================
// void IRAM_ATTR configButtonISR();
bool checkConfigButton();

// ================= LED =================
void blinkLEDHotspot();

// ================= HOTSPOT =================
void startHotspotMode();
void setupWebServerRoutes();

// ================= ROUTES =================
void handleApplyNow();
void sendStatus(bool ok);
void handleSystemStatus();

#endif