#pragma once
#include <NTPClient.h>

class ScheduledTimer {
  public:
    enum RepeatType { Recurring, Once };

    ScheduledTimer(NTPClient& ntp, int startHour, int startMin, int startSec,
                   int endHour, int endMin, int endSec, RepeatType repeat, void (*callback)())
      : timeClient(ntp), startHour(startHour), startMin(startMin), startSec(startSec),
        endHour(endHour), endMin(endMin), endSec(endSec),
        originalStartHour(startHour), originalStartMin(startMin), originalStartSec(startSec),
        repeat(repeat), callback(callback), isCallbackExecuted(false) {}

    void run() {
        int currHour = timeClient.getHours();
        int currMin  = timeClient.getMinutes();
        int currSec  = timeClient.getSeconds();
        int startTime = startHour * 3600 + startMin * 60 + startSec;
        int endTime   = endHour   * 3600 + endMin   * 60 + endSec;
        int currTime  = currHour  * 3600 + currMin  * 60 + currSec;
        if ((startTime <= endTime && currTime >= startTime && currTime <= endTime) ||
            (startTime > endTime && (currTime >= startTime || currTime <= endTime))) {
            if(!isCallbackExecuted) {
                if (callback) callback();
                isCallbackExecuted = true;
            }
        } else {
            if(isCallbackExecuted) { isCallbackExecuted = false; }
        }
    }
    void reset() { isCallbackExecuted = false; }
    void update(int sH, int sM, int sS, int eH, int eM, int eS){
        startHour = sH; startMin = sM; startSec = sS;
        endHour = eH; endMin = eM; endSec = eS;
        originalStartHour = sH; originalStartMin = sM; originalStartSec = sS;
        isCallbackExecuted = false;
    }
  private:
    NTPClient& timeClient;
    int startHour, startMin, startSec;
    int endHour, endMin, endSec;
    int originalStartHour, originalStartMin, originalStartSec;
    RepeatType repeat;
    void (*callback)();
    bool isCallbackExecuted;
};
