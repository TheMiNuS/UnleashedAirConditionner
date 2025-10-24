#include "Utils.h"

void getMacAddressBlocks(ESP8266WiFiClass& wifi, uint8_t macBlocks[6]) {
    String macAddress = wifi.macAddress();
    size_t macLength = macAddress.length();

    int blockIndex = 0;
    uint8_t currentBlock = 0;

    for (size_t i = 0; i < macLength; i++) {
        char c = macAddress.charAt(i);
        if (c != ':') {
            int hexValue;
            if (c >= '0' && c <= '9')       hexValue = c - '0';
            else if (c >= 'A' && c <= 'F')  hexValue = 10 + (c - 'A');
            else if (c >= 'a' && c <= 'f')  hexValue = 10 + (c - 'a');
            else continue;

            if (blockIndex % 2 == 0) currentBlock = static_cast<uint8_t>(hexValue << 4);
            else {
                currentBlock |= static_cast<uint8_t>(hexValue);
                macBlocks[blockIndex / 2] = currentBlock;
            }
            blockIndex++;
        }
    }
}

void calculateMD5(const char *input, uint8_t *md5Array) {
    MD5Builder md5;
    md5.begin();
    md5.add(input);
    md5.calculate();
    md5.getBytes(md5Array);
}

bool isDST(unsigned long epoch) {
    TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // UTC+2
    TimeChangeRule CET  = {"CET ", Last, Sun, Oct, 3,  60}; // UTC+1
    Timezone tz(CEST, CET);
    time_t local = tz.toLocal(epoch);
    return tz.locIsDST(local);
}

void updateNTPClientOffset(NTPClient& timeClient) {
    int timeZoneOffset = 3600; // CET
    if (isDST(timeClient.getEpochTime())) {
        timeZoneOffset = 7200; // CEST
    }
    timeClient.setTimeOffset(timeZoneOffset);
}

String byteToHexString(char value) {
    String hexString = String((uint8_t)value, HEX);
    if (hexString.length() < 2) hexString = "0" + hexString;
    return hexString;
}

String arrayToHexString(char *arr, int length) {
    String hexStr = "";
    for (int i = 0; i < length; i++) {
        hexStr += byteToHexString(arr[i]) + " ";
    }
    return hexStr;
}
