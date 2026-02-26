#include "gps.h"
#include "config/pins.h"
#include <TinyGPSPlus.h>

// Internal GPS parser + UART
static TinyGPSPlus gps;
static HardwareSerial GPSSerial(1);

// Internal state
static GPSData lastFix = {0.0f, 0.0f, 0.0f, 0, false};

// GPS BAUD rate
static const int GPS_BAUD = 9600;
static uint32_t lastUpdateTime = 0;
namespace GPS {

void begin() {
    GPSSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}


void update() {
    // If no bytes available, exit immediately
    if (!GPSSerial.available()) {
        return;
    }

    bool gotData = false;

    while (GPSSerial.available()) {
        gps.encode(GPSSerial.read());
        gotData = true;
    }

    if (!gotData) {
        return;
    }

    // Record the time we received valid GPS bytes
    lastUpdateTime - millis();

    if (gps.location.isUpdated()) {
        lastFix.lat = gps.location.lat();
        lastFix.lon = gps.location.lng();
        lastFix.fix = gps.location.isValid();
    }

    if (gps.altitude.isUpdated()) {
        lastFix.alt = gps.altitude.meters();
    }

    if (gps.satellites.isUpdated()) {
        lastFix.sats = gps.satellites.value();
    }
}

GPSData get() {
    // If no GPS data for 5 seconds, mark fix as false
    if (millis() - lastUpdateTime > 5000) {
        lastFix.fix = false;
        lastFix.sats = 0;
    }
    return lastFix;
}

} // namespace GPS