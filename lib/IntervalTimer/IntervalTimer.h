#pragma once
#include <Arduino.h>

class IntervalTimer {
  public:
    enum RepeatType { Recurring, Once };

    IntervalTimer(unsigned long _interval, void (*_function1)(), void (*_function2)(), RepeatType _recurring = Recurring)
        : interval(_interval), previousMillis(0), function1(_function1), function2(_function2), recurring(_recurring){}

    void run() {
        unsigned long currentMillis = millis();

        if (recurring == Once) {
          if (!runOnceFlag) {
            if (!function1Executed) {
              if (function1) function1();
              function1Executed = true;
            }
            if ((currentMillis - previousMillis) >= interval) {
              if(function2) { function2(); }
              function1Executed = false;
              runOnceFlag = true;
            }    
          }
        }
        else if(recurring == Recurring) {
            if (!function1Executed) {
              if (function1) function1();
              function1Executed = true;
            }
            if ((currentMillis - previousMillis) >= interval) {
              if(function2) { function2(); }
              previousMillis = currentMillis;
              function1Executed = false;
            }
        } 
    }

    void updateInterval(unsigned long newInterval) { interval = newInterval; }
    unsigned long getRemainingTime() {
        if (recurring == Once && runOnceFlag) return 0;
        return (interval - (millis() - previousMillis));
    }
    void stop() { runOnceFlag = true; function1Executed = false; }
    void start() { runOnceFlag = false; previousMillis = millis(); }

  private:
    unsigned long interval;
    unsigned long previousMillis;
    void (*function1)();
    void (*function2)();
    RepeatType recurring;
    bool function1Executed = false;
    bool runOnceFlag = true;
};
