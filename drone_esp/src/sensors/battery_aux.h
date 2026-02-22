#pragma once
#include <Arduino.h>

struct BatteryAuxData {
    float voltage = 0.0f;  // filtered aux/button voltage
    bool valid = false;
};

class BatteryAux {
public:
    BatteryAux(int adcPin);

    void begin();
    void update();
    BatteryAuxData get() const;

private:
    int pin;
    float filteredVoltage = 0.0f;
    bool initialized = false;

    float readRawVoltage();
};