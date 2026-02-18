#pragma once
#include <Arduino.h>

class Timer {
public:
    Timer(uint32_t interval) : interval(interval), last(millis()) {}

    bool ready() {
        uint32_t now = millis();
        if (now - last >= interval) {
            last = now;
            return true;
        }
        return false;
    }

private:
    uint32_t interval;
    uint32_t last;
};