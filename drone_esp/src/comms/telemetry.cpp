#include "telemetry.h"
#include "wifi_link.h"

// Sensor modules
#include "sensors/gps.h"
#include "sensors/ultrasonic.h"
#include "sensors/battery_main.h"
#include "sensors/battery_aux.h"
#include "sensors/sensor_instances.h"

// Utilities
#include "utils/timers.h"
#include <ArduinoJson.h>

namespace Telemetry {

static Timer telemetryTimer(100);   // 10 Hz
static bool initialized = false;

void begin() {
    initialized = true;
}

void update() {
    if (!initialized) return;

    // Only send at fixed rate
    if (!telemetryTimer.ready()) return;

    // Create a small JSON document
    StaticJsonDocument<256> doc;

    // ============================
    // GPS
    // ============================
    GPSData gps = GPS::get();
    JsonObject gpsObj = doc.createNestedObject("gps");
    gpsObj["lat"] = gps.lat;
    gpsObj["lon"] = gps.lon;
    gpsObj["alt"] = gps.alt;
    gpsObj["sats"] = gps.sats;
    gpsObj["fix"] = gps.fix;

    // ============================
    // Ultrasonic
    // ============================
    UltrasonicData left = leftUS.get();
    UltrasonicData right = rightUS.get();

    JsonObject usObj = doc.createNestedObject("ultrasonic");
    usObj["left"] = left.valid ? left.distance_cm : -1.0f;
    usObj["right"] = right.valid ? right.distance_cm : -1.0f;

    // ============================
    // Battery
    // ============================
    BatteryMainData mainBatt = batteryMain.get();
    BatteryAuxData  auxBatt  = batteryAux.get();

    JsonObject battObj = doc.createNestedObject("battery");
    battObj["main"] = mainBatt.valid ? mainBatt.voltage : 0.0f;
    battObj["aux"]  = auxBatt.valid  ? auxBatt.voltage  : 0.0f;

    // ============================
    // Timestamp
    // ============================
    doc["timestamp"] = millis();

    // ============================
    // Serialize + Send
    // ============================
    char buffer[256];
    size_t len = serializeJson(doc, buffer, sizeof(buffer));

    WiFiLink::sendTelemetry(buffer, len);
}

} // namespace Telemetry