# 📡 MODBUS Data Logger (ESP32-Based IoT Gateway)

## 🚀 Overview

The **MODBUS Data Logger** is an ESP32-based industrial IoT gateway that reads data from **Modbus RTU devices (via RS485)** and publishes it to a **cloud server using MQTT**.

It also provides:

* 📶 WiFi configuration via hotspot
* 🌐 Web-based configuration portal
* 🔄 OTA (Over-the-Air) firmware updates
* 💾 EEPROM-based configuration storage

This project is designed for **industrial automation, energy monitoring, and remote data acquisition systems**.

---

## 🧠 System Architecture

```
Modbus Device (Sensor/Meter)
        │
        │ RS485
        ▼
     ESP32 (Gateway)
        │
        │ WiFi
        ▼
    MQTT Broker
        │
        ▼
 Cloud Dashboard / Server
```

---

## ⚙️ Features

* ✅ Modbus RTU Master (RS485 communication)
* ✅ Reads holding registers from industrial devices
* ✅ Converts register data into float/int values
* ✅ JSON-based payload creation
* ✅ MQTT data publishing
* ✅ WiFi auto-connect and reconnect
* ✅ Hotspot mode for configuration
* ✅ Web interface for setup
* ✅ EEPROM configuration storage
* ✅ OTA firmware update support
* ✅ Status LED indication

---

## 🛠️ Hardware Requirements

* ESP32 Development Board
* RS485 Module (e.g., MAX485)
* Modbus-enabled sensor/device (Energy meter, PLC, etc.)
* Power supply (5V/3.3V)

---

## 🔌 Pin Configuration (Example)

| ESP32 Pin    | Function                |
| ------------ | ----------------------- |
| TX/RX        | UART for Modbus         |
| GPIO (DE/RE) | RS485 direction control |
| GPIO         | Status LED              |
| GPIO         | Config Button           |

---

## 📂 Project Structure

```
MODBUS_Data_Logger/
│
├── include/        # Header files
├── src/            # Source files
│   ├── main.cpp
│   ├── Modbuss.cpp
│   ├── wifiHandler.cpp
│   ├── Handlers.cpp
│   ├── webHandlers.cpp
│   ├── OTA.cpp
│
├── lib/            # External libraries
├── test/           # Test files
├── platformio.ini  # Build configuration
└── README.md
```

---

## ⚡ Getting Started

### 1️⃣ Clone the Repository

```
git clone https://github.com/your-username/MODBUS_Data_Logger.git
cd MODBUS_Data_Logger
```

---

### 2️⃣ Open in PlatformIO / VS Code

* Install **PlatformIO extension**
* Open project folder

---

### 3️⃣ Build and Upload

```
pio run
pio run --target upload
```

---

## 📶 WiFi Configuration (Hotspot Mode)

If no WiFi is configured or config button is pressed:

* ESP32 creates hotspot:

  ```
  SSID: NODE_SETUP
  Password: 12345678
  ```

* Connect to hotspot

* Open browser:

  ```
  http://192.168.4.1
  ```

* Enter:

  * WiFi SSID & Password
  * MQTT Broker details
  * Modbus parameters

---

## 🔁 OTA Firmware Update

* Ensure ESP32 and PC are on same network
* Upload firmware wirelessly using PlatformIO / Arduino OTA

Benefits:

* No USB required
* Remote updates possible

---

## 🔄 Modbus Communication

* Protocol: **Modbus RTU**
* Function Code: `0x03` (Read Holding Registers)

Example:

```
Slave ID: 23
Start Address: 20
Number of Registers: 8
```

---

## 📦 MQTT Data Format

Data is published in JSON format:

```
{
  "id": 23,
  "d": {
    "IA20": 25.3,
    "IA22": 26.1
  }
}
```

---

## 🌐 Web Configuration Portal

Accessible in hotspot mode.

Allows configuration of:

* WiFi credentials
* MQTT server & port
* Slave ID
* Register mapping
* Publish interval

---

## 💾 EEPROM Storage

Configuration is stored in EEPROM:

* WiFi credentials
* MQTT details
* Modbus settings

Ensures persistence after reboot.

---

## 🔄 Device Modes

### 🔹 Normal Mode

* Connects to WiFi
* Reads Modbus data
* Publishes to MQTT

### 🔹 Setup Mode

* Starts hotspot
* Opens web server
* Saves configuration

---

## 💡 Use Cases

* Energy monitoring systems
* Industrial IoT gateways
* Remote sensor logging
* Smart factories
* Building automation

---

## 🔐 Future Improvements

* 🔒 MQTT over TLS (secure communication)
* 🕒 NTP time synchronization
* 💽 SD card data logging
* 🔁 Remote configuration via MQTT
* 🧠 Modbus auto-discovery
* 🐶 Watchdog timer integration

---

## 🤝 Contributing

Contributions are welcome!

Steps:

1. Fork the repository
2. Create a feature branch
3. Commit changes
4. Submit a pull request

---

## 📜 License

This project is open-source and available under the **MIT License**.

---

## 👨‍💻 Author

**Shubham Athane**

* Embedded Systems Developer
* ESP32 | Modbus | IoT | ADAS

---

## ⭐ Support

If you like this project, please ⭐ the repository and share it!

---
