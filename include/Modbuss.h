#ifndef MODBUS_H
#define MODBUS_H


// ================= MODBUS CONTROL =================
void preTransmission();
void postTransmission();
float convertFloat(uint16_t r0, uint16_t r1);

void initModbusHardware();

// ================= UTILITIES =================
int parseRegList(const char *list, uint16_t regs[], int maxRegs);

// ================= STATE MACHINE =================
void processModbusState();

// ================= DATA FLOW =================
bool readModbusAndPublish();

#endif