#pragma once
#include <Arduino.h>

namespace WiFiLink {
    
    void begin();
    void update();
    
    void sendHeartbeat();
    void sendTelemetry(const char* data, size_t len);
}