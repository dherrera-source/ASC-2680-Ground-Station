#pragma once
#include <Arduino.h>

struct GPSData {
    float lat;
    float lon;
    float alt;
    int   sats;
    bool  fix;
};

namespace GPS {
    void begin();
    void update();
    GPSData get();
}