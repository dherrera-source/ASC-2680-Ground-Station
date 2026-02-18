#include "wifi_link.h"
#include <WiFi.h>
#include "../utils/logger.h"

static const char* SSID = "Cornuta";
static const char* PASS = "15418Cornuta";

WiFiClient client;

namespace WiFiLink {

void begin() {
    Logger::info("Connecting to WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASS);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 50) {
        delay(100);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Logger::info("WiFi connected.");
    } else {
        Logger::error("WiFi failed to connect.");
    }
}

void update() {
    // Future: reconnect logic, packet receive, etc.
}

void sendHeartbeat() {
    Logger::info("Heartbeat sent.");
    // Future: send actual packet to ground station
}

}