# IntervalTimer — Non‑Blocking Two‑Phase Timer

A tiny stateful timer that:
- runs `function1()` once at the beginning of each interval
- runs `function2()` when the interval elapses
- repeats (`Recurring`) or runs once (`Once`).

Great for periodic tasks without blocking `loop()`.

---

## API

```cpp
class IntervalTimer {
public:
  enum RepeatType { Recurring, Once };

  IntervalTimer(unsigned long intervalMs,
                void (*onStart)(),
                void (*onEnd)(),
                RepeatType mode = Recurring);

  void run();                     // Call frequently (e.g., in loop())
  void updateInterval(unsigned long newMs);
  unsigned long getRemainingTime();
  void stop();
  void start();
};
```

---

## Example (generic LED blinker)

```cpp
#include <Arduino.h>
#include "IntervalTimer.h"

const int LED = 2; // Adjust to your board

void ledOn()  { digitalWrite(LED, LOW);  } // Active-low LED
void ledOff() { digitalWrite(LED, HIGH); }

IntervalTimer blinker(1000, ledOn, ledOff, IntervalTimer::Recurring);

void setup() {
  pinMode(LED, OUTPUT);
  blinker.start();
}

void loop() {
  blinker.run();  // toggles: on at t=0, off at t=1s, repeat
}
```

---

## Notes & Best Practices

- `run()` is cheap and non‑blocking; call it every loop iteration for best precision.
- You can pass `nullptr` for one of the callbacks if you only need the other phase.
