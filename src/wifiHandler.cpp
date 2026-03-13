#include "wifiHandler.h"
#include "Global.h"


// ================= WIFI STATE MACHINE =================
void processWiFiState()
{
  static bool printed = false;

  // =========================
  // IF CONNECTED
  // =========================
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!printed)
    {
      printed = true;
      Serial.println("✅ WiFi Connected!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      
      setupWebServerRoutes();
          // START WEB SERVER
    server.begin();
    Serial.println("🌐 Web server started");

    }

    wifiRetry = 0;
    state = STATE_NORMAL_NET;
    return;
  }

  printed = false;

  // =========================
  // RETRY TIMER (2 seconds)
  // =========================
  if (millis() - lastConnAttempt < 2000)   // 2 sec retry interval
    return;

  lastConnAttempt = millis();
  wifiRetry++;

  Serial.printf("❌ WiFi not connected | Retry %d/%d\n",
                wifiRetry,
                MAX_RETRIES);

  // =========================
  // START NEW CONNECTION ATTEMPT
  // =========================

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();   // IMPORTANT: removed (true)
  
  bool isHidden = (config.wifiHidden == 1);
  bool isOpen = (String(config.wifiEncMode) == "NONE");

  // printWiFiAttempt();

  if (isOpen)
  {
    Serial.println("🔓 Open WiFi selected");

    if (isHidden)
      WiFi.begin(config.wifiSSID, NULL, 0, NULL, true);
    else
      WiFi.begin(config.wifiSSID);
  }
  else
  {
    if (isHidden)
      WiFi.begin(config.wifiSSID, config.wifiPASS, 0, NULL, true);
    else
      WiFi.begin(config.wifiSSID, config.wifiPASS);
  }

  // =========================
  // FAILSAFE RESTART
  // =========================
  if (wifiRetry >= MAX_RETRIES)
  {
    Serial.println("🔥 WiFi failed after max retries. Restarting...");
    delay(300);
    ESP.restart();
  }
}