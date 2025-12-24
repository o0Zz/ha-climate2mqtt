# Generic IR Blaster

## Overview

This project explains how to convert build an **IR Blaster**

The core idea is to connect a **microcontroller (ESP32)** to the IR Receiver (TSOP) and IR Sender (Led) and place this box at near a possible from your original aiconditionner IR.

The main advantage of this solution, it do not require any access to the climate this projet will simulat IR and receive IR from remote controller. So your air condition warranty etc.. is preserved.

The main disadvantage is you will have a small "case" visible on your air condition (or next to it) which is less elegant.

## Key Advantages

- ✅ Bidirectional IR communication (send & receive)
- ✅ Accurate device state tracking (If well place on the wall)
- ✅ Works with existing remote controls

## Hardware Schematic



## Bill of Materials

| Component | Description | Link |
|---------|------------|------|
| 1Kohm Resistor | | https://fr.aliexpress.com/item/1005010228694988.html |
| 2N2222 | NPN Transistor | https://fr.aliexpress.com/item/1005001728337190.html |
| TSOP 38Khz | IR Receiver | https://fr.aliexpress.com/item/1005006160739428.html | 
| IR Infrared LED | IR Emitter | https://fr.aliexpress.com/item/1005010437966420.html |

## ESP32 Configuration

On the ESP32, you must select one GPIO for **IR receive (RX)** and one GPIO for **IR transmit (TX)**.

According to the provided schematic:

- **GPIO0 → IR RX**
- **GPIO1 → IR TX**
