# Unleashed Air Conditioner (ESP8266)

A small ESP8266 firmware to bridge a split-unit air conditioner’s indoor UART protocol with Wi-Fi, a web interface, OTA updates, and MQTT control/status.

> **License:** CC BY-NC-ND 4.0 (non-commercial, no derivatives). See `LICENSE`.  
> **Author:** The_MiNuS

---

## Features

- **AC protocol bridge (UART)**  
  Parses RX status frames and sends TX configuration frames to the indoor unit. CRC16/Modbus handling is implemented. Frames A1/A2/A3/A4/A5/A6/AB/AC are supported with selected fields decoded and exposed.

- **Web UI (HTTP)**  
  - Status dashboard (live decoded values & frame hex dumps)  
  - Module configuration (Wi-Fi, hostname, HTTP auth, MQTT host/login/password/port, OTA password)  
  - “Write configuration” page to push set-point, mode, fan, sweep, etc.  
  - Web-based firmware upload endpoint (`/doUpdate`)  
  Routes are protected by HTTP Basic Auth.

- **MQTT integration**  
  - Base topic: `UnleashedAirConditioner/<MAC>/...`  
  - Publishes decoded status under `/AC_status/*` (temperature, modes, timers, etc.)  
  - Subscribes to `/AC_config/*` to apply remote changes (setpoint, mode, fan, etc.)  
  - “Programs” and “Timers” configuration/state topics are exposed under `/AC_programs_*` and `/timers_*`.

- **Internal Timers (EEPROM-backed)**  
  16 timers with on/off schedules, day masks, and linkage to a “Program” ID. Configuration via MQTT, persisted to EEPROM, and executed at runtime.

- **OTA updates**  
  - Arduino OTA (port 8266, password hash)  
  - Web upload handler with signed binary support (BearSSL signature verification using embedded public key)

---

## Architecture

- **`main.cpp`** – App entry, Wi-Fi/NTP/MQTT/OTA setup, scheduler loops, and MQTT callback to update AC/programs/timers from topics.  
- **`lib/`** – Modular version of `TheMiNuSToolBox`: utilities, EEPROM config, timers, and AC device class.  
- **`webInterfacePages.h` + `htmlCode.h`** – Web routes and HTML/CSS templates.  
- **`platformio.ini`** – Build config & libs. (Signing post-action defined in `sign_firmware.py`).  
- **`signing/`** – Folder containing the firmware signing scripts and public key (private key ignored by Git).

---

## MQTT Topic Map (summary)

Base:  
```
UnleashedAirConditioner/<MAC>/
```

Publish (examples):  
```
AC_status/returnAirTemperature
AC_status/fanSpeed
AC_status/powerMode
AC_status/temperatureSetPoint
AC_programs_status/<N>/timerName
AC_programs_status/<N>/activationProgram_0..7
timers_status/<N>/enable_time | disable_time | activation | linked_program_ID | Sunday..Saturday
```

Subscribe (examples):  
```
AC_config/temperatureSetPoint | operationMode | powerMode | fanSpeed | ...
AC_programs_config/<N>/timerName | activationProgram_0..7 | deactivationProgram_0..7
timers_config/<N>/enable_time | disable_time | activation | linked_program_ID | Sunday..Saturday
```

---

## Web API (authenticated)

- `/` – home/status (colored health dots)  
- `/module-configuration` – Wi-Fi, host, HTTP user/pass, MQTT params, OTA password  
- `/read-configuration` – decoded live state  
- `/write-configuration` – setpoint/mode/fan/timers/etc. (send to unit)  
- `/debug-information` – RX/TX frames (hex)  
- `/doUpdate` (POST multipart) – upload firmware (signed)  
- `/wifi` – apply module config (persists to EEPROM + reboot)  
- `/reboot` – reboot device  
- `/styles.css` – CSS theme

---

## Build & Flash (PlatformIO)

### Dependencies
(auto-installed via `platformio.ini`):  
- `NTPClient@^3.2.1`  
- `ESPAsyncWebServer` (git)  
- `Timezone@^1.2.4`  
- `PubSubClient@^2.8`

---

## ⚙️ PlatformIO Configuration Guide

The build and upload process can be done in **two modes**:
- via **serial** (first flash or recovery),
- via **OTA** (after the first configuration).

### Example `platformio.ini`

```ini
[env:esp12e]
platform = espressif8266
board = esp12e
framework = arduino

; Common parameters
upload_speed = 115200
monitor_speed = 115200

; --- SERIAL UPLOAD MODE (default) ---
; Comment the following OTA section if using USB
upload_port = /dev/ttyUSB0
monitor_port = /dev/ttyUSB0

; --- OTA UPLOAD MODE (network) ---
; Uncomment to enable OTA once Wi-Fi is configured
; upload_protocol = espota
; upload_port = 192.168.1.42     ; IP of your device
; upload_flags =
;   --port=8266
;   --auth="yourOTAPassword"     ; must match WebUI OTA password
;
; Note: OTA will only be available if an OTA password was set
;       in the web interface (stored hashed in EEPROM).
;
; Switching between modes:
; - Comment the OTA section to go back to serial
; - Uncomment it again to push over the network
```

### Serial flashing
1. Connect the ESP12 via USB serial adapter.  
2. Ensure `upload_port` matches `/dev/ttyUSBx` (Linux/Mac) or `COMx` (Windows).  
3. Run:  
   ```bash
   pio run -t upload
   ```

### OTA flashing
1. Configure Wi-Fi and OTA password in the web UI.  
2. Replace the IP and password in `platformio.ini`.  
3. Run:  
   ```bash
   pio run -t upload
   ```
   PlatformIO connects over the network and performs a signed firmware update.

---

## 🔏 Firmware Signing Workflow

The project includes **cryptographic signature verification** using BearSSL.

### Principle
1. The **public key** is compiled into the firmware (in `main.cpp`).  
2. The **private key** stays offline in `signing/private.key`.  
3. When you build, PlatformIO triggers the `sign_firmware.py` script automatically after linking.

### How it works
The hook at the bottom of `sign_firmware.py`:

```python
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", sign_firmware)
```

tells PlatformIO to execute the `sign_firmware()` function right after producing the `.bin`.

This function:
- locates `private.key` (in `signing/private.key`),
- calls Espressif’s built-in signing tool (`signing.py`) from the SDK,
- signs the firmware with the private key,
- creates a new file `firmware.signed.bin`.

### On the device
When uploading through `/doUpdate` or OTA:
- The ESP8266 verifies the SHA-256 signature using the embedded **public key**.
- If the signature is valid → the update is applied.  
- If invalid → update is rejected.

### Manual signing (optional)
If you prefer to sign manually:
```bash
python3 ./sign_firmware.py
```
or directly use Espressif’s signing tool:
```bash
python ~/.platformio/packages/framework-arduinoespressif8266/tools/signing.py   --mode sign --privatekey signing/private.key   --bin .pio/build/esp12e/firmware.bin   --out .pio/build/esp12e/firmware.signed.bin
```

---

## First boot & configuration

1. **Boot:** If no Wi-Fi credentials in EEPROM, the device starts in AP (recovery) mode with the MAC as SSID. Configure via the Web UI.  
2. **Setup:** Go to `/module-configuration`, set Wi-Fi/host/HTTP user+pass/MQTT, and optionally an **OTA password** (stored as MD5 hash).  
3. **Save** and reboot. The device reconnects automatically.  
4. **MQTT:** After connection, the device publishes under `UnleashedAirConditioner/<MAC>/...`.  
   You can push AC settings by publishing to `.../AC_config/...`.

---

## Security notes (important)

- **Change defaults**: don’t ship with `admin/admin` or a sample OTA password.  
- **Web UI secrets**: Password fields are masked and not pre-filled.  
- **OTA**: Disabled if no OTA password is defined in EEPROM.  
- **HTTPS**: For LAN use, HTTP is acceptable; for WAN, use a reverse proxy with TLS.  
- **Signing keys**: keep `private.key` offline, commit only `public.key`.

---

## Roadmap / Ideas

- Optional HTTPS listener (ESP32 target)
- UI for internal timers
- MQTT auto-discovery (Home Assistant)

---

## License

This work is licensed under a **Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License (CC BY-NC-ND 4.0)**. See `LICENSE` for details.  
Commercial use and derivative works require prior written permission from **The_MiNuS**.

---

## Disclaimer

This project interfaces with mains-powered HVAC equipment. Use at your own risk. The authors and contributors are not responsible for any damage, malfunction, or legal issues resulting from its use.
