# HA-Climate2MQTT

A lightweight solution for integrating any climate control system with **Home Assistant** via **MQTT** and **ESP32**

---

## 🧊 Supported Climates

- [ ] Mitsubishi  
- [ ] Toshiba  
- [ ] Midea  

---

## 🧊 Supported Features

- [x] HA Auto discovery
- [x] Set temperature, mode, power, swing

---

## 🧪 Tested Hardware

- [x] ESP32-C6 (https://fr.aliexpress.com/item/1005008953214925.html)

---

## ⚙️ Prerequisites

Ensure you have the following installed before building:

- [CMake](https://cmake.org/download/)  
- [Ninja](https://github.com/ninja-build/ninja/releases)  
- [ESP-IDF (v5.1 or newer)](https://dl.espressif.com/dl/esp-idf/)  

---

## 🧰 Build Instructions

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

---

### 🧩 Build the Project (VS Code)

> **TODO:** Add Visual Studio Code configuration and setup steps.

---

## 🪲 Debugging

To monitor logs from your ESP device:

```bash
idf.py monitor
```

Use `Ctrl + ]` to exit the monitor.

---

## Wiring

> **TODO**