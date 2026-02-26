#include <Arduino.h>

#include <Arduino.h>
#include "config/pins.h"
#include "comms/wifi_link.h"
#include "utils/timers.h"
#include "utils/logger.h"
#include "sensors/gps.h"
#include "sensors/ultrasonic.h"
#include "sensors/battery_main.h"
#include "sensors/battery_aux.h"
#include "comms/Telemetry.h"
#include "sensors/sensor_instances.h"

Timer heartbeatTimer(100); // 100 ms heartbeat
Timer gpsPrintTimer(1000); // 1 second GPS print
Timer usPrintTimer(500); // 500 ms ultrasonic print
Timer battPrintTimer(1000); // 10 second battery print

Ultrasonic leftUS(US_LEFT_TRIG, US_LEFT_ECHO);
Ultrasonic rightUS(US_RIGHT_TRIG, US_RIGHT_ECHO);
BatteryMain batteryMain(BATTERY_MAIN_PIN);
BatteryAux batteryAux(BATTERY_AUX_PIN);


void setup() {
    Serial.begin(115200);
    Logger::info("Booting Drone ESP32...");

    WiFiLink::begin();
    Logger::info("Initalizing Wifi module...");

    Telemetry::begin();
    Logger::info("Telemetry system initialized");

    GPS::begin();
    delay(200);
    Logger::info("GPS initialized");

    batteryMain.begin();
    batteryAux.begin();
    Logger::info("Battery monitors initialized");

    leftUS.begin();
    rightUS.begin();
    Logger::info("Ultrasonic sensors initialized");

};


void loop() {
    WiFiLink::update();
    GPS::update();
    leftUS.update();
    rightUS.update();
    batteryMain.update();
    batteryAux.update();

    Telemetry::update();

    if (heartbeatTimer.ready()) {
      Logger::info("Heartbeat tick");
        WiFiLink::sendHeartbeat();
    }

    if (gpsPrintTimer.ready()) {
      GPSData g = GPS::get();
      Serial.printf("[GPS] lat: %.6f, lon: %.6f, alt: %.2f, sats: %d, fix: %s\n",
                    g.lat, g.lon, g.alt, g.sats, g.fix? "true" : "false");
   }

   if (usPrintTimer.ready()) {
    UltrasonicData L = leftUS.get();
    UltrasonicData R = rightUS.get();

    Serial.printf("[US] Left: %.1f cm (valid=%d), Right: %.1f cm (valid=%d)\n",
                  L.distance_cm, L.valid,
                  R.distance_cm, R.valid);
   }

   if (battPrintTimer.ready()) {
    BatteryMainData m = batteryMain.get();
    BatteryAuxData a = batteryAux.get();

    Serial.printf("[BAT] Main: %.2f V (valid=%d), Aux: %.2f V (valid=%d)\n",
                  m.voltage, m.valid,
                  a.voltage, a.valid);
   }
}


