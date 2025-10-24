# Utils — Cross‑Project Helper Utilities

A collection of lightweight helpers commonly needed on embedded projects (ESP8266/Arduino):
- MD5 hashing for password/secret derivation
- CET/CEST daylight‑saving detection & NTP offset update
- MAC address parsing (string → 6 raw bytes)
- Hex helpers for diagnostics/logging

---

## API

```cpp
// Convert "AA:BB:CC:DD:EE:FF" into 6 raw bytes.
void getMacAddressBlocks(ESP8266WiFiClass& wifi, uint8_t mac[6]);

// Compute MD5(input) → 16 raw bytes in md5[16].
void calculateMD5(const char* input, uint8_t* md5);

// EU daylight saving helper + NTP offset management (CET/CEST).
bool isDST(unsigned long epoch);
void updateNTPClientOffset(NTPClient& timeClient);

// Hex helpers for logs and frame dumps.
String byteToHexString(char value);
String arrayToHexString(char* arr, int length);
```

**Dependencies:** `Arduino.h`, `ESP8266WiFi.h`, `MD5Builder.h`, `Timezone.h`, `NTPClient.h`.

---

## Example (generic)

```cpp
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "Utils.h"

const char* WIFI_SSID = "ssid";
const char* WIFI_PASS = "pass";

WiFiUDP ntpUDP;
NTPClient ntp(ntpUDP, "europe.pool.ntp.org", 3600, 60*1000);

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(200);

  // Maintain proper timezone (CET/CEST) and show time
  ntp.begin();
  updateNTPClientOffset(ntp);
  ntp.update();
  Serial.printf("Time %02d:%02d:%02d\n", ntp.getHours(), ntp.getMinutes(), ntp.getSeconds());

  // MD5 demo
  uint8_t md5[16]; calculateMD5("secret", md5);
  String md5hex; for (int i=0;i<16;i++) md5hex += byteToHexString((char)md5[i]);
  Serial.printf("MD5(secret)=%s\n", md5hex.c_str());

  // MAC as 6 bytes
  uint8_t mac[6]; getMacAddressBlocks(WiFi, mac);
  char raw[6]; for (int i=0;i<6;i++) raw[i] = mac[i];
  Serial.printf("MAC bytes: %s\n", arrayToHexString(raw, 6).c_str());
}

void loop() {
  ntp.update();
  updateNTPClientOffset(ntp);
  delay(1000);
}
```

---

## Notes & Best Practices

- `updateNTPClientOffset()` should be called periodically to seamlessly cross DST boundaries.
- MD5 output is **binary**; convert to hex before persisting/printing.
- Avoid logging secrets in plaintext; prefer printing hashes or masked values.
