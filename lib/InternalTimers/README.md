# InternalTimers — 16 Persistent Schedules with Callbacks

Implements 16 timers with on/off times (seconds since midnight), day‑of‑week mask, activation flag and a “program ID” to pass into your callbacks. Persisted to EEPROM; designed to be controllable locally or via higher‑level messaging.

---

## API (summary)

```cpp
class InternalTimers {
public:
  using onFunction  = uint8_t (*)(uint8_t programId, uint8_t timerId);
  using offFunction = uint8_t (*)(uint8_t programId, uint8_t timerId);

  InternalTimers(PubSubClient& mqtt, ESP8266WiFiClass& wifi, NTPClient& ntp,
                 const char* topicBase,
                 onFunction onCb=nullptr, offFunction offCb=nullptr,
                 uint16_t eepromBase=0x300);

  void begin();                 // Load/init from EEPROM
  void subscribeToTopics();     // (optional) subscribe to config topics
  void publishTimerStates();    // (optional) publish current state
  void updateConfiguration(String key, uint64_t value, uint16_t index);
  void run();                   // Evaluate windows and invoke callbacks

  void setOnFunction(onFunction);
  void setOffFunction(offFunction);
};
```

**Dependencies:** `EEPROM`, `NTPClient`, optional `PubSubClient` for messaging.

---

## Example (generic scheduler)

```cpp
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include "InternalTimers.h"
#include "Utils.h"

const char* WIFI_SSID = "ssid";
const char* WIFI_PASS = "pass";
const char* TOPIC_BASE = "MyScheduler/";

WiFiClient net;
PubSubClient mqtt(net);
WiFiUDP ntpUDP;
NTPClient ntp(ntpUDP, "europe.pool.ntp.org", 3600, 60*1000);

uint8_t onCb (uint8_t programId, uint8_t timerId){ Serial.printf("ON p=%u t=%u\n", programId, timerId); return 0; }
uint8_t offCb(uint8_t programId, uint8_t timerId){ Serial.printf("OFF p=%u t=%u\n", programId, timerId); return 0; }

InternalTimers timers(mqtt, WiFi, ntp, TOPIC_BASE, onCb, offCb);

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(200);
  ntp.begin();

  timers.begin();

  // Configure timer #0: 06:30..07:00 on weekdays, program 1
  timers.updateConfiguration("enable_time",        6*3600 + 30*60, 0);
  timers.updateConfiguration("disable_time",       7*3600,         0);
  timers.updateConfiguration("activation",         1,              0);
  timers.updateConfiguration("linked_program_ID",  1,              0);
  for (auto d : { "Monday","Tuesday","Wednesday","Thursday","Friday" })
    timers.updateConfiguration(d, 1, 0);
}

void loop(){
  ntp.update();
  updateNTPClientOffset(ntp);
  timers.run();
  delay(250);
}
```

---

## Notes & Best Practices

- Your callbacks should be **idempotent**: return `0` on success to latch state, non‑zero to retry next cycle.
- When used without messaging, `subscribeToTopics()`/`publishTimerStates()` can be skipped.
