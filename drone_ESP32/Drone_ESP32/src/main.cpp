#include <Arduino.h>
#include "Wifi_manage.h"
#include "telemetry.h"
#include "ultrasonic.h"
#include "gps.h"
#include "thermal.h"

// Loop timing
unsigned long lastLoop = 0;
const unsigned long LOOP_INTERVAL_MS = 100;  // 10 Hz loop

// Sensor modules
UltrasonicSensor ultrasonic;
GPSModule gps;
ThermalCamera thermal;

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println("\n === Drone ESP32 Sensor Hub Booting ===");

  // WifFi + UDP
  Wifi_manage_init();
  telemetry_init();

  // Sensors
  ultrasonic.init();
  gps.init();
  thermal.init();

  Serial.println("=== Initialization Complete ===\n");
}

void loop() {
  unsigned long now = millis();
  if (now - lastLoop < LOOP_INTERVAL_MS) return;
  lastLoop = now;

  // --- Read sensors ---
  UltrasonicData uData = ultrasonic.read();
  GPSData gData = gps.read();
  ThermalData tData = thermal.read();

  // --- Debug Output ---
  Serial.printf("[ULTRA] F=%d L=%d R=%d B=%d D=%d\n",
                uData.front, uData.left, uData.right, uData.back, uData.down);

  Serial.printf("[GPS] min=%.2f max=%.2f avg=%.2f\n",
                gData.lat, gData.lon, gData.alt, gData.fix, gData.satellites);
  Serial.printf("[THERM] min=%.2f max=%.2f avg=%.2f\n",
                tData.minTemp, tData.maxTemp, tData.avgTemp);
  
  // --- Send telemetry ---
  
  telemetry_send(uData, gData, tData);

  // --- Optional: handle config commands ---
  string cmd = telemetry_read_command();
  if (cmd.length() > 0 ){
    Serial.println("[CMD] " + cmd);
    // TODO: add config commands later
  }
}