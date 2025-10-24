#include "InternalTimers.h"

InternalTimers::InternalTimers(PubSubClient& pubSubClient, ESP8266WiFiClass& wifiClient, NTPClient& timeClient,
                               const char* topicPtr, onFunction setOnFuncPtr, offFunction setOffFuncPtr,
                               uint16_t eepromTimerStartAddressBase)
  : mqtt(pubSubClient), wifi(wifiClient), ntpClient(timeClient), topic_base(topicPtr),
    switchOn(setOnFuncPtr), switchOff(setOffFuncPtr), eepromTimerStartAddress(eepromTimerStartAddressBase) {
    memset(isCallbackExecuted, 0, sizeof(isCallbackExecuted));
}

InternalTimers::~InternalTimers() {}

void InternalTimers::setOnFunction(onFunction func)  { this->switchOn  = func; }
void InternalTimers::setOffFunction(offFunction func){ this->switchOff = func; }

void InternalTimers::begin() {
  Serial.printf("\r\nLoading Timers from EEPROM: ");
  EEPROM.begin(4096);
  EEPROM.get(eepromTimerStartAddress, MyEEPROMTimers);
  EEPROM.end();

  if (MyEEPROMTimers.areTimersConfigured != 0x55) {
    for (int i=0; i < timersParameters::NUM_TIMERS; i++) {
            MyEEPROMTimers.bank[i] = 0;
    }
    MyEEPROMTimers.areTimersConfigured = 0x55;
    EEPROM.begin(4096);
    EEPROM.put(eepromTimerStartAddress, MyEEPROMTimers);
    EEPROM.commit();
    EEPROM.end();
    Serial.printf(" Initialized !\r\n");
  } else {
    Serial.printf(" Loaded\r\n");
  } 
}

void InternalTimers::subscribeToTopics() {
  String topic = String(topic_base) + wifi.macAddress() + "/timers_config/#";
  mqtt.subscribe(topic.c_str());
}

void InternalTimers::publishTimerStates() {
  String topic;
  String payload;

  for (int i = 0; i < timersParameters::NUM_TIMERS; i++) {
      topic = String(topic_base) + wifi.macAddress() + "/timers_status/" + String(i) + "/enable_time";
      payload = String( (uint32_t)(MyEEPROMTimers.bank[i] >> 24) & 0x00FFFFFF ); 
      mqtt.publish(topic.c_str(), payload.c_str());

      topic = String(topic_base) + wifi.macAddress() + "/timers_status/" + String(i) + "/disable_time";
      payload = String( (uint32_t)(MyEEPROMTimers.bank[i]) & 0x00FFFFFF);
      mqtt.publish(topic.c_str(), payload.c_str());

      topic = String(topic_base) + wifi.macAddress() + "/timers_status/" + String(i) + "/activation";
      payload = String( (uint32_t)(MyEEPROMTimers.bank[i] >> 55) & 0x00000001);                                
      mqtt.publish(topic.c_str(), payload.c_str());          

      topic = String(topic_base) + wifi.macAddress() + "/timers_status/" + String(i) + "/linked_program_ID";
      payload = String( (uint32_t)(MyEEPROMTimers.bank[i] >> 48) & 0x0000000F);
      mqtt.publish(topic.c_str(), payload.c_str());      

      const char* daysOfWeek[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
      for (int j = 0; j < 7; j++) {
        topic = String(topic_base) + wifi.macAddress() + "/timers_status/" + String(i) + "/" + daysOfWeek[j];
        payload = String( (uint32_t)((MyEEPROMTimers.bank[i]) >> (56 + j)) & 0x00000001 );
        mqtt.publish(topic.c_str(), payload.c_str());
      }
  }
}

void InternalTimers::updateConfiguration(String targetParameter, uint64_t value, uint16_t bank) {
  if (bank >= timersParameters::NUM_TIMERS) return;

  if(targetParameter == "enable_time") {
    MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xFFFF000000FFFFFF) | ((value << 24) & 0x0000FFFFFF000000);
  } else if(targetParameter == "disable_time") {
    MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xFFFFFFFFFF000000) | ((value) & 0x0000000000FFFFFF);
  } else if(targetParameter == "activation") {
    MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xFF7FFFFFFFFFFFFF) | ((value << 55) & 0x0080000000000000);
  } else if(targetParameter == "linked_program_ID") {
    MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xFFF0FFFFFFFFFFFF) | ((value << 48) & 0x000F000000000000);
  } else if (targetParameter == "Sunday") {
    MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xFEFFFFFFFFFFFFFF) | ((value << 56) & 0x0100000000000000);
  } else if (targetParameter == "Monday") {
      MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xFDFFFFFFFFFFFFFF) | ((value << 57) & 0x0200000000000000);
  } else if (targetParameter == "Tuesday") {
      MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xFBFFFFFFFFFFFFFF) | ((value << 58) & 0x0400000000000000);
  } else if (targetParameter == "Wednesday") {
      MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xF7FFFFFFFFFFFFFF) | ((value << 59) & 0x0800000000000000);
  } else if (targetParameter == "Thursday") {
      MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xEFFFFFFFFFFFFFFF) | ((value << 60) & 0x1000000000000000);
  } else if (targetParameter == "Friday") {
      MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xDFFFFFFFFFFFFFFF) | ((value << 61) & 0x2000000000000000);
  } else if (targetParameter == "Saturday") {
      MyEEPROMTimers.bank[bank] = (MyEEPROMTimers.bank[bank] & 0xBFFFFFFFFFFFFFFF) | ((value << 62) & 0x4000000000000000);
  }

  EEPROM.begin(4096);
  EEPROM.put(eepromTimerStartAddress, MyEEPROMTimers);
  EEPROM.commit();
  EEPROM.end();
}

void InternalTimers::run() {
  uint32_t currentSeconds = (ntpClient.getHours() * 3600) + (ntpClient.getMinutes() * 60) + ntpClient.getSeconds();
  uint8_t currentDayOfWeek = ntpClient.getDay();

  for (int i = 0; i < timersParameters::NUM_TIMERS; i++) {
      bool isTimerEnabled = (MyEEPROMTimers.bank[i] >> 55) & 0x01;
      bool isTimerSetForToday = (MyEEPROMTimers.bank[i] >> (56 + currentDayOfWeek)) & 0x01;
      uint32_t enableTime = (MyEEPROMTimers.bank[i] >> 24) & 0xFFFFFF;
      uint32_t disableTime = MyEEPROMTimers.bank[i] & 0xFFFFFF;
      uint8_t linkedProgramID = (MyEEPROMTimers.bank[i] >> 48) & 0x0F;

      if (!isTimerEnabled || !isTimerSetForToday || currentSeconds < enableTime || currentSeconds >= disableTime) {
          if (isCallbackExecuted[i]) {
              if (switchOff != nullptr) {
                  int resultCode = switchOff(linkedProgramID, i);
                  if (resultCode == 0) isCallbackExecuted[i] = false;
              }
          }
          continue;
      }

      if (currentSeconds >= enableTime && currentSeconds < disableTime) {
          if (!isCallbackExecuted[i]) {
              if (switchOn != nullptr) {
                  int resultCode = switchOn(linkedProgramID, i);
                  if (resultCode == 0) isCallbackExecuted[i] = true;
              }
          }
      }
  }
}
