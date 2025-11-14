# HA-Climate2MQTT
A lightweight solution for integrating any climate control system with **Home Assistant** via **MQTT** and an **ESP32**

## ❄️ Supported Climates
- [X] Mitsubishi (UART)
- [X] Toshiba (UART) - (Seiya, Shorai, Yukai, ...)
- [X] Midea (IR protocol)
- [ ] Daikin


## ⚙️ Supported Features
- [x] HA Auto discovery
- [X] Set temperature, mode, power, swing
- [ ] AC can be remotly control via WebInterface
- [ ] Fully configurable from WebInterface
- [ ] Access point to setup it
- [ ] Firmware upgrade


## 🧪 Tested Hardware
- [x] ESP32-C6 (https://fr.aliexpress.com/item/1005008953214925.html)

## 🚀 How to Flash & Run
1. Download the firmware for your microcontroller (In release section)
2. Wire your AC unit.
Refer to the documentation in the [doc/](https://github.com/o0Zz/ha-climate2mqtt/tree/main/doc) folder for wiring details.
3. Flash your ESP32 via ESP flasher
https://www.espressif.com/en/support/download/other-tools  
or 
```python 
pip install esptool
esptool.py --port COM3 erase_flash
esptool.py --chip esp32 --port COM3 --baud 460800 write_flash -z 0x1000 firmware.bin
```
4. Power on the ESP32.
By default, it will create a Wi-Fi access point named `ESP32_Climate2MQTT`.
4. Connect to the ESP32 using that access point and access to "http://192.168.8.1"

## 🧰 Build Instructions

### ⚙️ Prerequisites

Ensure you have the following installed before building:

- [CMake](https://cmake.org/download/)  
- [Ninja](https://github.com/ninja-build/ninja/releases)
- [ESP-IDF (v5.5.1 or newer)](https://dl.espressif.com/dl/esp-idf/)

### 🖥️ Using ESP-IDF Command Line

Open the official **ESP-IDF Command Line** and type:

```bash
git clone https://github.com/o0zz/ha-climate2mqtt.git --recursive
cd ha-climate2mqtt
idf.py set-target {esp32|esp32s2|esp32s3|esp32c2|esp32c3|esp32c6}
idf.py menuconfig
idf.py build
idf.py flash
```

### 🧩 Using VSCode - Windows

Before building, ensure you have set the correct target as described in the previous section.

Open the official ESP-IDF Command Line and run:
```
idf.py set-target {esp32|esp32s2|esp32s3|esp32c2|esp32c3|esp32c6}
```

#### ⚙️ Configure VS Code Terminal
In VS Code, set **Command Prompt** as the default terminal:
1. Press **Ctrl + Shift + P**
2. Select **Terminal: Select Default Profile**
3. Choose **Command Prompt (C:\Windows\System32\cmd.exe)**

#### 🚀 Build the Project

1. Press **Ctrl + Shift + P**
2. Select **Tasks: Run Task**
3. Choose **Build & Run (ESP-IDF)**
