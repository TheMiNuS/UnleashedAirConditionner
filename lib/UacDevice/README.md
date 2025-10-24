# UartDevice (UacDevice) — Stateful UART Protocol Engine

This library shows how to implement a **stateful UART device engine** that:
- reads framed messages from a serial link,
- decodes and stores a structured state,
- builds configuration frames for the peer,
- exposes setters/getters to higher layers (Web/MQTT/etc.).

> The provided implementation targets a specific AC protocol internally, but the design pattern is generic and reusable for other UART‑framed protocols.

---

## Responsibilities

- Frame parsing (header → payload → CRC) with a small internal state machine
- Decoding selected fields into public variables
- Building outbound config frames from setters (bitfields, masks)
- Optional publish/subscribe hooks (topic base passed in constructor)

---

## Constructor & Core Methods

```cpp
UacDevice(Stream& serialPort,
          PubSubClient& mqtt,
          ESP8266WiFiClass& wifi,
          const char* topicBase,
          char topicSize = 0,
          uint16_t eepromStart = 0x0400);

void begin();                         // init persistent structures
void subscribeToTopics();             // optional: bind to a message bus
uint8_t preFillConfiguration(uint8_t macBlocks[6]); // seed outbound frame
void updateConfiguration(String key, uint16_t value);
uint8_t decodeConfiguration();        // parse inbound frames into state
String  FrameToHexString(uint8_t type);
void    process();                    // pump UART (read/parse/dispatch)
void    sendHeartbeatFrame();         // example periodic TX
void    heartBeatControl();           // maintain "alive" indicator
```

**Dependencies:** `Arduino.h`, `PubSubClient` (optional for messaging), `ESP8266WiFi.h` for MAC/topic scoping.

---

## Example (generic harness)

```cpp
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include "UacDevice.h"
#include "Utils.h"

const char* WIFI_SSID = "ssid";
const char* WIFI_PASS = "pass";
const char* TOPIC_BASE = "MyDeviceBus/";

WiFiClient net;
PubSubClient mqtt(net);
SoftwareSerial bus;  // RX/TX pins adjusted to your wiring

UacDevice dev(bus, mqtt, WiFi, TOPIC_BASE, (char)strlen(TOPIC_BASE));

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(200);

  mqtt.setServer("192.168.1.10", 1883);
  mqtt.connect("uart-device");

  bus.begin(9600, SWSERIAL_8N1, 4, 5); // RX=D2(4), TX=D1(5)

  dev.begin();
  dev.subscribeToTopics();

  // Seed outbound configuration with MAC (or any identity material)
  uint8_t mac[6]; getMacAddressBlocks(WiFi, mac);
  dev.preFillConfiguration(mac);

  // Example setter → queues outbound update
  dev.updateConfiguration("powerMode", 1);
  dev.updateConfiguration("temperatureSetPoint", 23);
  dev.sendConfigurationToAC = 1;   // request transmit
}

void loop() {
  if (mqtt.connected()) mqtt.loop();
  dev.process();          // parse inbound frames & send pending config
  dev.heartBeatControl(); // optional heartbeat state
  delay(10);
}
```

---

## Notes & Best Practices

- Call `process()` **very frequently**; avoid long blocking code in `loop()`.
- Keep frame building & decoding centralized to prevent bit‑mask drift.
- For other projects, adapt the parser (header/length/CRC) and the set of decoded fields; the surrounding usage pattern remains valid.
