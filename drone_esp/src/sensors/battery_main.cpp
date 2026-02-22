#include "battery_main.h"
#include "config/pins.h"

// ESP32 ADC characteristics
static constexpr float ADC_MAX = 4095.0f;     // 12-bit ADC
static constexpr float ADC_REF = 3.3f;        // reference voltage

// Voltage divider ratio (example: 100k / 33k)
static constexpr float DIVIDER_RATIO = (100.0f + 33.0f) / 33.0f;

// Simple low-pass filter factor
static constexpr float FILTER_ALPHA = 0.10f;  // 10% new, 90% old


BatteryMain::BatteryMain(int adcPin)
    : pin(adcPin) {}


void BatteryMain::begin() {
    // ADC1 pins do not need pinMode()
    initialized = true;
    filteredVoltage = 0.0f;
}


float BatteryMain::readRawVoltage() {
    // Take multiple samples to reduce noise
    const int samples = 8;
    uint32_t total = 0;

    for (int i = 0; i < samples; i++) {
        int raw = analogRead(pin);
        total += raw;
    }

    float avg = total / float(samples);

    // Convert ADC reading → voltage at ADC pin
    float adcVoltage = (avg / ADC_MAX) * ADC_REF;

    // Scale up using the divider ratio to get actual LiPo voltage
    return adcVoltage * DIVIDER_RATIO;
}


void BatteryMain::update() {
    if (!initialized) return;

    float rawVoltage = readRawVoltage();

    // HARDENING: reject impossible values
    if (rawVoltage < 0.5f || rawVoltage > 25.0f) {
        // Pin floating or divider disconnected
        filteredVoltage = 0.0f;
        return;
    }

    // Low-pass filter
    filteredVoltage = (filteredVoltage * (1.0f - FILTER_ALPHA)) +
                      (rawVoltage * FILTER_ALPHA);
}


BatteryMainData BatteryMain::get() const {
    BatteryMainData out;

    out.voltage = filteredVoltage;
    out.valid = (filteredVoltage > 0.5f);  // simple sanity check

    return out;
}