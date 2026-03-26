#include "Modbus.h"
#include "Global.h"
#include <string.h>

// ================= UTILS =================
void sortRegisters(uint16_t regs[], int count)
{
  for (int i = 0; i < count - 1; i++)
  {
    for (int j = i + 1; j < count; j++)
    {
      if (regs[j] < regs[i])
      {
        uint16_t temp = regs[i];
        regs[i] = regs[j];
        regs[j] = temp;
      }
    }
  }
}

//-----------------------------------------------------------------------------------
// FLOAT CONVERSION (HIGH WORD FIRST)
float convertFloat(uint16_t w1, uint16_t w2)
{
  uint32_t raw = ((uint32_t)w2 << 16) | w1;   // SWAPPED

  float f;
  memcpy(&f, &raw, sizeof(f));
  return f;
}

//-----------------------------------------------------------------------------------
// ================= RS485 CONTROL =================
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, HIGH);
  digitalWrite(MAX485_DE, HIGH);
  delayMicroseconds(200);
}

//-----------------------------------------------------------------------------------
void postTransmission()
{
  delayMicroseconds(200);
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
}

//-----------------------------------------------------------------------------------
// ================= MODBUS SETUP =================
void initModbusHardware()
{
  pinMode(MAX485_DE, OUTPUT);
  pinMode(MAX485_RE_NEG, OUTPUT);

  digitalWrite(MAX485_DE, LOW);
  digitalWrite(MAX485_RE_NEG, LOW);

  Serial2.begin(config.baudrate, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial2.flush();

  node.begin(config.slaveID, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println("✅ Modbus hardware initialized");
}

//-----------------------------------------------------------------------------------
// ================= REGISTER PARSER =================
int parseRegList(const char *list, uint16_t regs[], int maxRegs)
{
  int count = 0;
  char temp[120];

  strncpy(temp, list, sizeof(temp));
  temp[sizeof(temp) - 1] = '\0';

  char *token = strtok(temp, ",");

  while (token && count < maxRegs)
  {
    regs[count++] = atoi(token);
    token = strtok(NULL, ",");
  }

  return count;
}

//-----------------------------------------------------------------------------------
// ================= MODBUS STATE =================
void processModbusState()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    state = STATE_NORMAL_WIFI;
    return;
  }

  if (!(mqttReady || httpReady))
  {
    state = STATE_NORMAL_NET;
    return;
  }

  if (modbusReady)
  {
    state = STATE_RUNNING;
    return;
  }

  static unsigned long lastModbusAttempt = 0;

  if (millis() - lastModbusAttempt < 1000)
    return;

  lastModbusAttempt = millis();
  modbusRetry++;

  Serial.printf("🔄 Modbus check | Retry %d\n", modbusRetry);

  uint16_t regAddrs[30];
  bool tested = false;
  uint8_t result = 1;

  for (int g = 0; g < MAX_REG_GROUPS; g++)
  {
    RegisterGroup &grp = config.groups[g];

    if (!grp.enabled)
      continue;

    // -------- TEST FLOAT REGISTER ----------
    if (grp.reg16_enable && strlen(grp.reg16_list) > 0)
    {
      int count = parseRegList(grp.reg16_list, regAddrs, 30);

      if (count > 0)
      {
        tested = true;
        uint16_t reg = regAddrs[0];

        Serial.print("Testing register: ");
        Serial.println(reg);

        if (strcmp(grp.regType, "holding") == 0)
          result = node.readHoldingRegisters(reg, 2);
        else
          result = node.readInputRegisters(reg, 2);

        break;
      }
    }

    // -------- TEST 8 BIT REGISTER ----------
    if (grp.reg8_enable && strlen(grp.reg8_list) > 0)
    {
      int count = parseRegList(grp.reg8_list, regAddrs, 30);

      if (count > 0)
      {
        tested = true;
        uint16_t reg = regAddrs[0];

        if (strcmp(grp.regType, "holding") == 0)
          result = node.readHoldingRegisters(reg, 1);
        else
          result = node.readInputRegisters(reg, 1);

        break;
      }
    }
  }

  if (!tested)
  {
    Serial.println("⚠️ No register configured.");
    return;
  }

  if (result == node.ku8MBSuccess)
  {
    modbusReady = true;
    modbusRetry = 0;

    Serial.println("✅ Modbus responding!");
    state = STATE_RUNNING;
    return;
  }

  Serial.print("❌ Modbus fail | err=");
  Serial.println(result);

  if (modbusRetry >= 5)
  {
    Serial.println("⚠️ Modbus device not responding.");
    modbusRetry = 0;
  }
}

//-----------------------------------------------------------------------------------
// ================= READ + PUBLISH =================
bool readModbusAndPublish()
{
  if (!modbusReady)
  {
    Serial.println("⚠️ Modbus not ready.");
    return false;
  }

  JsonDocument doc;
  doc["id"] = config.slaveID;

  JsonObject dataObj = doc["d"].to<JsonObject>();

  uint16_t regAddrs[30];
  bool anySuccess = false;

  for (int g = 0; g < MAX_REG_GROUPS; g++)
  {
    RegisterGroup &grp = config.groups[g];

    if (!grp.enabled)
      continue;

    char typePrefix;

    if (strcmp(grp.regType, "input") == 0)
      typePrefix = 'I';
    else if (strcmp(grp.regType, "holding") == 0)
      typePrefix = 'H';
    else
      continue;

    //===================== 8 BIT =====================
    if (grp.reg8_enable && strlen(grp.reg8_list) > 0)
    {
      int count = parseRegList(grp.reg8_list, regAddrs, 30);

      for (int i = 0; i < count; i++)
      {
        uint16_t reg = regAddrs[i];
        uint8_t result;

        if (strcmp(grp.regType, "holding") == 0)
        {
          Serial.println(reg);

          result = node.readHoldingRegisters(reg, 1);
        }
        else
        {
          Serial.println(reg);
          result = node.readInputRegisters(reg, 1);
        }

        char key[12];
        sprintf(key, "%cB%d", typePrefix, reg);

        if (result == node.ku8MBSuccess)
        {
          uint8_t value = node.getResponseBuffer(0) & 0xFF;
          dataObj[key] = value;
          anySuccess = true;
        }
        else
        {
          dataObj[key] = -1;
        }

        delay(5);
      }
    }

    //===================== FLOAT =====================
    if (grp.reg16_enable && strlen(grp.reg16_list) > 0)
    {
      int count = parseRegList(grp.reg16_list, regAddrs, 30);

      if (count == 0)
        continue;

      sortRegisters(regAddrs, count);

      uint16_t startReg = regAddrs[0];
      uint16_t maxReg = regAddrs[count - 1];

      uint16_t regCount = (maxReg - startReg) + 2;

      uint8_t result;

      if (strcmp(grp.regType, "holding") == 0)
      {
        Serial.println(startReg);
        Serial.println(regCount);
        result = node.readHoldingRegisters(startReg , regCount);
      }
      else
      {
        Serial.println(startReg);
        Serial.println(regCount);
        result = node.readInputRegisters(startReg, regCount);
      }

      Serial.print("Block read start=");
      Serial.print(startReg);
      Serial.print(" count=");
      Serial.print(regCount);
      Serial.print(" result=");
      Serial.println(result);

      if (result == node.ku8MBSuccess)
      {
        for (int i = 0; i < count; i++)
        {
          uint16_t reg = regAddrs[i];

          int index = reg - startReg;

          if (index + 1 >= regCount)
          {
            Serial.println("⚠️ Float pair out of range");
            continue;
          }

          uint16_t word1 = node.getResponseBuffer(index);
          uint16_t word2 = node.getResponseBuffer(index + 1);

          Serial.print("Reg ");
          Serial.print(reg);
          Serial.print(" raw words: ");
          Serial.print(word1);
          Serial.print(" ");
          Serial.println(word2);

          // Correct order: HIGH word first, LOW word second
          float value = convertFloat(word1, word2);

          // Safety filter
          if (isnan(value) || isinf(value))
            value = 0;

          Serial.print("Decoded float: ");
          Serial.println(value, 6);

          char valueStr[20];
          sprintf(valueStr, "%.4f", value); // print like meter output

          char key[12];
          sprintf(key, "%cA%d", typePrefix, reg);

          dataObj[key] = valueStr;
          anySuccess = true;
        }
      }
      else
      {
        for (int i = 0; i < count; i++)
        {
          char key[12];
          sprintf(key, "%cA%d", typePrefix, regAddrs[i]);
          dataObj[key] = -1;
        }
      }

      delay(10);
    }
  }

  if (!anySuccess)
  {
    Serial.println("❌ All Modbus reads failed.");
    return false;
  }

  char payload[512];
  serializeJson(doc, payload);

  Serial.println("Payload:");
  Serial.println(payload);

  bool publishSuccess = false;

  //================ MQTT =================
  if (mqttReady && mqttClient.connected())
  {
    if (mqttClient.publish(config.mqttTopic, payload))
      publishSuccess = true;
  }

  //================ HTTP =================
  if (httpReady)
  {
    HTTPClient http;

    String url = "http://";
    url += String(config.httpURL);

    if (config.httpPort != 80 && config.httpPort > 0)
    {
      url += ":";
      url += String(config.httpPort);
    }

    if (http.begin(url))
    {
      http.addHeader("Content-Type", "application/json");

      int code = http.POST((uint8_t *)payload, strlen(payload));

      http.end();

      if (code > 0)
        publishSuccess = true;
    }
  }

  return publishSuccess;
}