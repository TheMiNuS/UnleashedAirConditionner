# SystemConfiguration — EEPROM‑Backed Settings Singleton

Stores module‑level settings (Wi‑Fi, hostname, HTTP/MQTT credentials, OTA password hash, etc.) in a struct persisted to EEPROM. Exposes a global instance `config`.

---

## Public Interface (excerpt)

```cpp
class SystemConfiguration {
public:
  SystemConfiguration();

  void load();    // Read full struct from EEPROM (and init on first boot)
  void save();    // Persist current struct to EEPROM
  void reset();   // Factory defaults + save
  void flash_led(uint8_t repeat, uint8_t blinks); // Simple status blink

  // Public fields (excerpt)
  uint16_t FlashStatus, WifiConfig;
  char wifi_ssid[32], wifi_password[64];
  char old_wifi_ssid[32], old_wifi_password[64];
  char ota_password[64];      // 32‑char MD5 hex; empty → OTA disabled
  char hostname[32];
  char http_login[32], http_password[64];
  char mqtt_login[32], mqtt_password[64], mqtt_host[32];
  uint16_t mqtt_port;
};

extern SystemConfiguration config;
```

**Dependencies:** `EEPROM`, `ESP8266WiFi` for defaults, `Arduino.h`.

---

## Example (generic usage)

```cpp
#include <Arduino.h>
#include "SystemConfiguration.h"

extern SystemConfiguration config;

void setup() {
  Serial.begin(115200);
  config.load();

  Serial.printf("Hostname: %s  MQTT: %s:%u\n",
                config.hostname, config.mqtt_host, config.mqtt_port);

  // Change a value safely
  strncpy(config.hostname, "MyNode-01", sizeof(config.hostname)-1);
  config.save();
}

void loop() {}
```

---

## Notes & Best Practices

- Never print or publish passwords; mask them in UIs/logs.
- Gate OTA: enable only if `strlen(config.ota_password) > 0`.
- Consider a migration strategy if you evolve the struct layout (versioning).
