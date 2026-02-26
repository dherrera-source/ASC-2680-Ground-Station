#include "wifi_link.h"
#include <WiFi.h>
#include "../utils/logger.h"
#include <WiFiUdp.h>

static const char* SSID = "Verizon_P7CDZW";
static const char* PASS = "wise5haw8deify";

static WiFiUDP udp;

// Ground Station Destination
static IPAddress groundstationIP(192, 168, 0, 50);
static const uint16_t TELEMETRY_PORT = 14550;

namespace WiFiLink {

static bool connected = false;

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

void sendTelemetry(const char* data, size_t len) {
    if (!WL_CONNECTED) return;

    udp.beginPacket(groundstationIP, TELEMETRY_PORT);
    udp.write((const uint8_t*)data, len);
    udp.endPacket();
    }
}