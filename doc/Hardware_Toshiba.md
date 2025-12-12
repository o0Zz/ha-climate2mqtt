# Toshiba Heat Pump – CN105 Connector

## Overview

This repository documents the **communication connector** used on **Toshiba heat pumps**.  
It describes the electrical characteristics, pinout, and how to interface the connector with a microcontroller such as an **ESP32** for monitoring and control purposes.

The connector exposes a simple UART interface commonly used by OEM controllers and third-party integrations.

---

## Communication Details

The connector provides a **UART (serial) interface** with the following parameters:

- **Logic level:** 5V TTL  
- **Baud rate:** 9600 bps  
- **Data format:** 8E1  
  - 8 data bits  
  - Even parity  
  - 1 stop bit  

⚠️ **Important:**  
The ESP32 operates at **3.3V logic levels**.  
A **bi-directional level shifter** is required to safely interface with the CN105 connector and avoid damage to the ESP32.

---

## Connector

### Physical Connector

![Toshiba Connector](img/toshiba_connector.png)

- **Type:** JST female connector  
- **Pitch:** 2.00 mm   

---

### Pinout

| Pin | Wire Color | Signal | Description |
|-----|------------|--------|-------------|
| 1 | Blue | TX | UART transmit (from heat pump) |
| 2 | Pink | GND | Ground |
| 3 | Black | +5V | 5V power output |
| 4 | White | RX | UART receive (to heat pump) |
| 5 | — | — | Not connected |

---

## Bill of Materials (BOM)

| Component | Description | Link |
|----------|-------------|------|
| JST 2.00 mm Male Connector | CN105 mating connector | https://fr.aliexpress.com/item/1005009087160808.html |

---

## Adapter: JST 2.54 mm to JST 2.00 mm male

To simplify prototyping and connection to standard breadboards or modules, a **JST 2.54 mm to JST 2.00 mm adapter cable** can be used.

![CN105 Wiring Adapter](img/toshiba_wiring.png)

---

## Connecting to an ESP32

For detailed wiring diagrams and ESP32 configuration, see:

➡️ **[ESP32 Hardware Documentation](Hardware_ESP32.md)**
