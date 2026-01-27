#include <Arduino.h>
#include <ArduinoJson.h>
#include "network.h"
#include "controller_interface.h"

unsigned long lastPacketTime = 0;
const unsigned long FAILSAFE_TIMEOUT = 200; //ms
bool failsafeActive = false;

void setup() {
  Serial.begin(115200);
  network_begin("YOUR_WIFI", "YOUR_PASSWORD");
}

unsigned long lastDebug = 0;

void loop() {
  char buffer[512];
  int len = network_receive(buffer, sizeof(buffer));
  
  if (len > 0) {
    // Parse JSON
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, buffer);

    if (!err) {
      // Extract Values
      float throttle  = doc["throttle"] | 0.0f;
      float yaw       = doc["yaw"]      | 0.0f;
      float pitch     = doc["pitch"]    | 0.0f;
      float roll      = doc["roll"]     | 0.0f;

      // Apply to controller
      setThrottle(throttle);
      setYaw(yaw);
      setPitch(pitch);
      setRoll(roll);
      
      // Buttons
      if (doc["buttons"].is<JsonObject>()) {
        for (JsonPair kv : doc["buttons"].as<JsonObject>()) {
          if (kv.value().as<int>() == 1) {
            pressButton(kv.key().c_str());
          }
        }
      }

      // Rate-limited debug output
      if (millis() - lastDebug > 200) {
        Serial.println(" == ESP32 RECEIVED PACKET ==");
        Serial.print("Raw: ");
        Serial.println(buffer);

        Serial.print("Throttle: "); Serial.println(throttle);
        Serial.print("Yaw: ");      Serial.println(yaw);
        Serial.print("Pitch: ");    Serial.println(pitch);
        Serial.print("Roll: ");     Serial.println(roll);

        lastDebug = millis();
      }
    } else {
        if (millis() - lastDebug > 200) {
          Serial.print("JSON Parse Error: ");
          Serial.println(err.c_str());
      }
    }
  }  
}


