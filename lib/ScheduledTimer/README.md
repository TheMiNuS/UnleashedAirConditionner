# ScheduledTimer — Daily Time‑Window Trigger (NTP‑Backed)

Triggers a callback once when the current time enters a configured `[start..end]` window; resets when leaving the window. Supports windows that cross midnight.

---

## API

```cpp
class ScheduledTimer {
public:
  enum RepeatType { Recurring, Once };

  ScheduledTimer(NTPClient& ntp,
                 int startH, int startM, int startS,
                 int endH,   int endM,   int endS,
                 RepeatType repeat,
                 void (*callback)());

  void run();     // Call frequently
  void reset();   // Manually clear latch
  void update(int sH, int sM, int sS, int eH, int eM, int eS);
};
```

**Dependencies:** `NTPClient` (any time source with getHours/Minutes/Seconds/Day).

---

## Example (generic heater window)

```cpp
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "ScheduledTimer.h"
#include "Utils.h"

const char* WIFI_SSID = "ssid";
const char* WIFI_PASS = "pass";

WiFiUDP ntpUDP;
NTPClient ntp(ntpUDP, "europe.pool.ntp.org", 3600, 60*1000);

bool heaterRequested = false;
void onMorningWindow(){ heaterRequested = true; Serial.println("Enter window: request heat once"); }

ScheduledTimer morning(ntp, 6,0,0, 8,0,0, ScheduledTimer::Recurring, onMorningWindow);

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(200);
  ntp.begin();
}

void loop(){
  ntp.update();
  updateNTPClientOffset(ntp);
  morning.run();

  if (heaterRequested) {
    // Act once (then clear)
    heaterRequested = false;
  }
  delay(250);
}
```

---

## Notes & Best Practices

- Crossing‑midnight windows are supported (e.g., 22:00→06:00).
- Use `reset()` if you want to retrigger during the same window.
