#include "SystemConfiguration.h"

SystemConfiguration::SystemConfiguration() {
  FlashStatus = 0;
}

void SystemConfiguration::flash_led(uint8_t repeat, uint8_t blinks) {
  int8_t i = 0;
  int8_t j = 0;
  pinMode(ONBOARD_LED, OUTPUT);
  for (i=0; i <= repeat; i++) {
    for (j=0; j < blinks; j++) {
      digitalWrite(ONBOARD_LED, 0);
      delay(2);
      digitalWrite(ONBOARD_LED, 1);
      delay(150);
    }
    delay(500);
  }
}

void SystemConfiguration::load() {
  Serial.printf("\r\nLoading EEPROM configuration: ");
  EEPROM.begin(sizeof(SystemConfiguration));
  EEPROM.get(0, *this);
  EEPROM.end();
  
  if((FlashStatus != FLASH_INITIALIZED)) {
    Serial.printf("Initialization !\r\n");
    reset();
    flash_led(5,5);
    ESP.restart();
  } else {
    Serial.printf("OK !\r\n");
  }
}

void SystemConfiguration::save() {
  EEPROM.begin(sizeof(SystemConfiguration));
  EEPROM.put(0, *this);
  EEPROM.commit();
  EEPROM.end();
}

void SystemConfiguration::reset() {
   FlashStatus = FLASH_INITIALIZED;
   strcpy(wifi_ssid, "");
   strcpy(wifi_password, "");
   strcpy(old_wifi_ssid, "");
   strcpy(old_wifi_password, "");
   strcpy(ota_password, ""); // MD5 as in legacy code
   strcpy(hostname, WiFi.macAddress().c_str());
   strcpy(http_login, "admin");
   strcpy(http_password, "admin");
   strcpy(mqtt_login, "");
   strcpy(mqtt_password, "");
   strcpy(mqtt_host, "127.0.0.1");
   mqtt_port = 1883;
   save();
}

SystemConfiguration config;
