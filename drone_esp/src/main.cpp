#include <Arduino.h>

#include <Arduino.h>
#include "comms/wifi_link.h"
#include "utils/timers.h"
#include "utils/logger.h"

Timer heartbeatTimer(100); // 100 ms heartbeat

void setup() {
    Serial.begin(115200);
    Logger::info("Booting Drone ESP32...");

    WiFiLink::begin();
}

void loop() {
    WiFiLink::update();

    if (heartbeatTimer.ready()) {
        WiFiLink::sendHeartbeat();
    }
}

