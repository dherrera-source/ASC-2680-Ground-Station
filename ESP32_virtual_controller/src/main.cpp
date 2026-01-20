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

void loop() {
  char buffer[512];
  int len = network_receive(buffer, sizeof(buffer));
  
  if (len > 0) {
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, buffer);
    
    if (!err) {
      // Reset failsafe timer
      lastPacketTime = millis();
      failsafeActive = false;

      //Stick controls
      setThrottle(doc["throttle"] | 0.0);
      setYaw(doc["yaw"] | 0.0);
      setPitch(doc["pitch"] | 0.0);
      setRoll(doc["roll"] | 0.0);
      
      
      // Buttons
      if (doc["buttons"].is<JsonObject>()) {
        for (JsonPair kv : doc["buttons"].as<JsonObject>()) {
          if (kv.value().as<int>() == 1) {
            pressButton(kv.key().c_str());
          }
        }
      }  
    }  
  }
}

