#pragma once
#include <Arduino.h>

namespace WiFiLink {
    void begin();
    void update();
    void sendHeartbeat();
}