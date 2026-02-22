#pragma once
#include <Arduino.h>

struct BatteryMainData {
    float voltage = 0.0f;  // filtered LiPo voltage
    bool valid = false;    // true if reading looks sane
};

class BatteryMain {
public:
    BatteryMain(int adcPin);

    void begin();      // configure ADC
    void update();     // take samples, filter
    BatteryMainData get() const;

private:
    int pin;
    float filteredVoltage = 0.0f;
    bool initialized = false;

    float readRawVoltage();   // single ADC read → volts
};