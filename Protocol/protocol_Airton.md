# Unleashed Air Conditioner Protocol Description

This document describes the reverse-engineered communication protocol used by the **Unleashed Air Conditioner** firmware.  
It is based on the analysis of serial communication frames exchanged between the ESP-based module and the air conditioning indoor unit.

---

## 1. Frame Structure Overview

Each frame transmitted between the Wi-Fi module and the AC unit follows the same global structure:

| Field | Size (bytes) | Description |
|--------|---------------|-------------|
| Header | 2 | Always `0x7A 0x7A`, marks the start of frame |
| Source | 1 | Source address (`0x21` for ESP / `0xD5` for AC) |
| Destination | 1 | Destination address (`0xD5` or `0x21`) |
| Length | 1 | Total frame length (including header and CRC) |
| Header2 | 2 | Additional data (`UnknownHeader1`, `UnknownHeader2`) |
| Type | 1 | Frame type identifier (A1, A3, etc.) |
| Footer | 2 | Usually `0x0A 0x0A`, marks the end of header |
| Data | n | Payload, varies by frame type |
| CRC16 | 2 | CRC16-Modbus checksum for integrity |

CRC16 is calculated using the Modbus algorithm (polynomial `0xA001`).

```cpp
uint16_t calculateCRC(const char* data, size_t length) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < length; ++i) {
      crc ^= data[i];
      for (int j = 0; j < 8; ++j)
          crc = (crc & 0x0001) ? (crc >> 1) ^ 0xA001 : (crc >> 1);
  }
  return crc;
}
```

---

## 2. Frame Types Summary

| Frame | Direction | Size (bytes) | Role / Function |
|--------|------------|--------------|------------------|
| A1 | ESP → AC | 24 | Send configuration and commands to AC |
| A2 | ESP → AC | 12 | End of Wi-Fi configuration |
| A3 | AC → ESP | 34 | State publication from the AC (temperatures, modes, etc.) |
| A4 | AC → ESP | 13 | Remote control activation/deactivation |
| A5 | ESP → AC | 27 | Wi-Fi enrollment or display control |
| A6 | AC → ESP | 28 | Reserved or unused (empty) |
| AB | ESP → AC | 12 | Heartbeat from Wi-Fi module |
| AC | AC → ESP | 18 | Response to heartbeat |

---

## 3. Frame A3 — State Publication (AC → ESP)

This frame contains all the operational states of the indoor unit, sent roughly every 10 seconds.

| Bytes | Field | Description | Extraction Formula | Example |
|--------|--------|--------------|--------------------|----------|
| 10–11 | Return Air Temperature | Room temperature sensed by the AC | `(H * 10) + L` → °C×10 | `0x01 0x05 = 16.5°C` |
| 12 | Functional Info | Bit 0: Lock<br>Bit 1: 3D Air Mode<br>Bit 2: Cleaning Request | `(byte >> bit) & 0x01` | |
| 13 | Fan & Operation Mode | Bits 7–4: Fan Speed (0–5)<br>Bit 3: Power On/Off<br>Bits 2–0: Mode (0=Auto,1=Cool,2=Dry,3=Fan,4=Heat)` | `(byte >> n) & mask` | |
| 14 | Quiet & Setpoint | Bit 6: Quiet Mode<br>Bit 5: °C/°F<br>Bits 3–0: Setpoint −16 | `Temp = (byte & 0x0F) + 16` | |
| 15 | Sweep | Bits 7–4: Sweep L/R<br>Bits 3–0: Sweep Up/Down | | |
| 16 | Config Word | Bit 7: Display<br>Bit 6: Ionizer<br>Bit 4: Aux Heater<br>Bits 3–2: Temp Display Mode<br>Bit 1: Sleep<br>Bit 0: Energy Saving | | |
| 19–20 | Timer Remaining | Remaining time (minutes) | `(L<<8)+H` | 60 = 1 hour |
| 23–24 | Operating Time | Total operating time (hours) | `(H<<8)+L` | |
| 32–33 | CRC | CRC16(Modbus) | | |

Decoded example (hexadecimal):
```
7A 7A D5 21 22 00 00 A3 0A 0A 01 05 03 32 60 24 80 00 10 00 00 02 00 00 00 00 00 00 00 00 3A 7C
```

---

## 4. Frame A1 — Configuration Push (ESP → AC)

Sent when a new configuration is applied through the web interface or via MQTT.

| Bytes | Field | Description | Note |
|--------|--------|--------------|------|
| 10–11 | Timer | Remaining timer (minutes) |
| 12 | Fan & Mode | Bits 7–4: Fan speed<br>Bit 3: Power<br>Bits 2–0: Operation mode |
| 13 | Quiet & Setpoint | Bit 6: Quiet<br>Bit 5: °C/°F<br>Bits 3–0: Temp−16 |
| 14 | Sweep | Bits 7–4: L/R<br>Bits 3–0: Up/Down |
| 15 | Config Word | Bit 7: Display<br>Bit 6: Ionizer<br>Bit 4: Aux Heater<br>Bits 3–2: Display Mode<br>Bit 1: Sleep<br>Bit 0: Eco |
| 16–21 | MAC Address | 6 bytes of the module’s MAC address |
| 22–23 | CRC | CRC16(Modbus) checksum |

---

## 5. Frame A4 — Remote Control Management (AC → ESP)

This short frame informs the Wi-Fi module whether the infrared remote control is active.

| Bytes | Field | Meaning |
|--------|--------|----------|
| 10 | Data | 0x00 → Remote ON<br>0x01 → Remote OFF<br>0xA5 → Unknown state |
| 11–12 | CRC | CRC16 checksum |

---

## 6. Frame AB — Heartbeat (ESP → AC)

Sent every 60 seconds to maintain communication with the AC.  
Example payload:
```
7A 7A 21 D5 0C 00 00 AB 0A 0A XX XX
```
`XX XX` = CRC16 calculated on all bytes except CRC itself.

---

## 7. Data Encoding Rules

- **Boolean values**: Bit masks (`(byte >> n) & 0x01`)
- **Temperature setpoint**: 4-bit value (16–31°C)
- **Fan speed**: 3-bit value (0=Auto → 5=Max)
- **Mode**: 3-bit value  
  | Code | Mode |
  |------|------|
  | 0 | Auto |
  | 1 | Cooling |
  | 2 | Dry |
  | 3 | Fan only |
  | 4 | Heating |

- **CRC**: Computed on all bytes except the last two CRC bytes.

---

## 8. MQTT Topic Convention

The module uses the following topic hierarchy (example MAC: `AA:BB:CC:DD:EE:FF`):

```
UnleashedAirConditioner/AABBCCDDEEFF/
├── AC_status/                # Published data from the AC
│   ├── returnAirTemperature
│   ├── fanSpeed
│   ├── temperatureSetPoint
│   └── ...
├── AC_config/                # Write-only topics to update configuration
│   ├── powerMode
│   ├── sweepModeLR
│   ├── temperatureUnit
│   └── ...
└── timers_config/            # Timer configuration (EEPROM-based)
```

---

## 9. Example of Full Communication Cycle

1. **AC sends Frame A3** → ESP updates internal state variables.  
2. **User modifies settings** via web/MQTT.  
3. ESP builds and sends **Frame A1** with new configuration.  
4. AC applies changes and confirms via next **A3 frame**.  
5. ESP periodically sends **Frame AB (heartbeat)**.  
6. AC may respond with **Frame AC** or **A4** for remote control changes.

---

## 10. Notes & Unknown Fields

- `UnknownHeader1` and `UnknownHeader2` are still not clearly identified; they might encode a checksum, device ID, or packet counter.
- Frame `A6` and `AC` are likely manufacturer test frames or reserved for advanced features.
- Some older units may not implement all features (ionizer, auxiliary heater, etc.).

---

**Author:** TheMiNuS  
**License:** [CC BY-NC-ND 4.0](https://creativecommons.org/licenses/by-nc-nd/4.0/)  
*Use for research, hobbyist, or personal projects only.*
