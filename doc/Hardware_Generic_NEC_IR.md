# Generic NEC IR – Heat Pump Connectivity Mod

## Overview

This project explains how to convert a **non-connected heat pump** into a **connected device** by interfacing directly with its **infrared (IR) receiver**.

The core idea is to connect a **microcontroller (ESP32)** to the IR data line so it can:

- **Receive IR commands** coming from the original remote control  
- **Transmit IR commands** directly to the heat pump  

Unlike most IoT IR blasters that only *send* commands, this approach also **captures commands sent by the original remote**, giving you **true state feedback**. This ensures that the system always knows the real operating state of the heat pump, even when changes are made manually with the remote control.


## Key Advantages

- ✅ Bidirectional IR communication (send & receive)
- ✅ Accurate device state tracking
- ✅ Works with existing remote controls

## ⚠️ Important Safety Notes

This hardware design is provided **"AS IS"**, **without any warranty of any kind**.

Please read carefully before building:

- **⚠️ Never inject any voltage into the IR data pin**
- The circuit **only pulls the data pin down to GND**
- The provided schematic **must not drive the line high**
- A **Schottky diode** is used as a protection mechanism in case of:
  - Firmware bugs
  - GPIO misconfiguration (e.g. pin accidentally set as output)

**Adding the Schottky diode is strongly recommended** for safety.

## Hardware Schematic

![IR Interface Schematic](img/schema_IR.png)


## Bill of Materials

| Component | Description | Link |
|---------|------------|------|
| ESP32-C6 Mini | Main microcontroller | https://fr.aliexpress.com/item/1005008653649948.html |
| Level Shifter | Logic level conversion | https://fr.aliexpress.com/item/1005006823347058.html |
| Schottky Diode (SR540) | GPIO protection | https://fr.aliexpress.com/item/1005010429977536.html |
| 2N2222 | NPN Transistor | https://fr.aliexpress.com/item/1005001728337190.html |


## ESP32 Configuration

On the ESP32, you must select one GPIO for **IR receive (RX)** and one GPIO for **IR transmit (TX)**.

According to the provided schematic:

- **GPIO0 → IR RX**
- **GPIO1 → IR TX**
