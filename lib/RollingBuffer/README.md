# RollingBuffer — Triple‑Stage Rolling Averages

Maintains three circular buffers to compute short‑/mid‑/long‑term averages for noisy signals (sensors, ADC, etc.). Unfilled slots are initialized to `0xFFFF` and ignored in averages.

---

## API

```cpp
class RollingBuffer {
public:
  RollingBuffer(uint16_t firstSize, uint16_t secondSize, uint16_t thirdSize);
  ~RollingBuffer();

  void addValue(uint16_t value);

  uint16_t getFirstAverage();
  uint16_t getSecondAverage();
  uint16_t getThirdAverage();

  uint16_t getFirstLastValue();
  uint16_t getSecondLastValue();
  uint16_t getThirdLastValue();
};
```

---

## Example (generic ADC smoothing)

```cpp
#include <Arduino.h>
#include "RollingBuffer.h"

RollingBuffer smooth(16, 8, 4);  // fine / medium / coarse

void setup() { Serial.begin(115200); }

void loop() {
  uint16_t raw = analogRead(A0);
  smooth.addValue(raw);

  Serial.printf("raw=%u avg1=%u avg2=%u avg3=%u\n",
    raw, smooth.getFirstAverage(), smooth.getSecondAverage(), smooth.getThirdAverage());

  delay(200);
}
```

---

## Notes & Best Practices

- Tune buffer sizes to your sampling rate and desired smoothing horizon.
- Averages are integer (rounded down); cast to float if you need fractions.
