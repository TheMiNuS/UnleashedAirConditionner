#pragma once
#include <Arduino.h>

class RollingBuffer {
    private:
        uint16_t bufferSize;
        uint16_t* buffer;
        uint16_t currentIndex;
        uint16_t secondBufferSize;
        uint16_t* secondBuffer;
        uint16_t secondCurrentIndex;
        uint16_t thirdBufferSize;
        uint16_t* thirdBuffer;
        uint16_t thirdCurrentIndex;

        uint16_t getBufferAverage(uint16_t* buffer, uint16_t bufferSize) {
            float sum = 0;
            uint16_t count = 0;
            for (uint16_t i = 0; i < bufferSize; i++) {
                if (buffer[i] != 0xFFFF) {
                    sum += (int16_t)buffer[i]; // keep signed averaging behavior
                    count++;
                }
            }
            return count == 0 ? 0 : sum / count;
        }

    public:
        RollingBuffer(uint16_t size, uint16_t secondSize, uint16_t thirdSize)
            : bufferSize(size), currentIndex(0),
              secondBufferSize(secondSize), secondCurrentIndex(0),
              thirdBufferSize(thirdSize), thirdCurrentIndex(0) {
            buffer = new uint16_t[size];
            memset(buffer, 0xFF, bufferSize * sizeof(uint16_t));
            secondBuffer = new uint16_t[secondSize];
            memset(secondBuffer, 0xFF, secondBufferSize * sizeof(uint16_t));
            thirdBuffer = new uint16_t[thirdSize];
            memset(thirdBuffer, 0xFF, thirdBufferSize * sizeof(uint16_t));
        }

        ~RollingBuffer() {
            delete[] buffer;
            delete[] secondBuffer;
            delete[] thirdBuffer;
        }

        void addValue(uint16_t value) {
            buffer[currentIndex] = value;
            currentIndex = (currentIndex + 1) % bufferSize;
            if (currentIndex == 0) {
                uint16_t average = getBufferAverage(buffer, bufferSize);
                secondBuffer[secondCurrentIndex] = average;
                secondCurrentIndex = (secondCurrentIndex + 1) % secondBufferSize;
                if (secondCurrentIndex == 0) {
                    uint16_t secondAverage = getBufferAverage(secondBuffer, secondBufferSize);
                    thirdBuffer[thirdCurrentIndex] = secondAverage;
                    thirdCurrentIndex = (thirdCurrentIndex + 1) % thirdBufferSize;
                }
            }
        }

        uint16_t getFirstAverage()  { return getBufferAverage(buffer, bufferSize); }
        uint16_t getSecondAverage() { return getBufferAverage(secondBuffer, secondBufferSize); }
        uint16_t getThirdAverage()  { return getBufferAverage(thirdBuffer, thirdBufferSize); }

        uint16_t getFirstLastValue() {
          if (currentIndex == 0) return buffer[0];
          return buffer[(currentIndex - 1) % bufferSize];
        }
        uint16_t getSecondLastValue() {
          if (secondCurrentIndex == 0) return secondBuffer[0];
          return secondBuffer[(secondCurrentIndex - 1) % secondBufferSize];
        }
        uint16_t getThirdLastValue() {
          if (thirdCurrentIndex == 0) return thirdBuffer[0];
          return thirdBuffer[(thirdCurrentIndex - 1) % thirdBufferSize];
        }
};
