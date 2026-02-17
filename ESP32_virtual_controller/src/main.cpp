#include <Arduino.h>
#include <ArduinoJson.h>
#include "network.h"
#include "controller_interface.h"

unsigned long lastPacketTime = 0;
const unsigned long FAILSAFE_TIMEOUT = 200; //ms
bool failsafeActive = false;

void setup() {
  Serial.begin(115200);

  initControllerInterface();
  Serial.println("Boot Setupt");
  network_begin("ADSS", "ADSS69420");
}

unsigned long lastDebug = 0;

void loop() {
  //Serial.println("Loop alive");
  char buffer[512];
  int len = network_receive(buffer, sizeof(buffer));
  
  if (len > 0) {
    // Parse JSON
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, buffer);

    if (err) {
      Serial.print("[DEBUG] JSON parse failed: ");
      Serial.println(err.c_str()); 
    }
    else {
    Serial.print("[DEBUG] Receive packet: ");
    serializeJson(doc, Serial);
    Serial.println();

    // Extract Values
    float throttle  = doc["throttle"] | 0.0f;
    float yaw       = doc["yaw"]      | 0.0f;
    float pitch     = doc["pitch"]    | 0.0f;
    float roll      = doc["roll"]     | 0.0f;
      
    // Extract type
    const char* type = doc["type"] | "<none>";
      
    // Apply to controller
    setThrottle(throttle);
    setYaw(yaw);
    setPitch(pitch);
    setRoll(roll);
      
    if (doc["type"] == "trim") {
      const char* axis = doc["axis"];
      float delta = doc["delta"];

      if (strcmp(axis, "throttle") == 0) adjustThrottleTrim(delta);
      else if (strcmp(axis, "yaw") == 0) adjustYawTrim(delta);
      else if (strcmp(axis, "pitch") == 0) adjustPitchTrim(delta);
      else if (strcmp(axis, "roll") == 0) adjustRollTrim(delta); 
    }

      if (doc["type"] == "trim_set") {
        const char* axis = doc["axis"];
        float value  = doc["value"];

        if (strcmp(axis, "throttle") == 0) setThrottleTrim(value);
        else if (strcmp(axis, "yaw") == 0) setYawTrim(value);
        else if (strcmp(axis, "pitch") == 0) setPitchTrim(value);
        else if (strcmp(axis, "roll") == 0) setRollTrim(value);
      }
      
    // NEW Buttons
    if (type && strcmp(type, "button") == 0) {
      const char* name = doc["name"];
      Serial.print("[DEBUG] Button command received: ");
      Serial.println(name);

      pressButton(name);
    }


    // OLD Buttons
    if (doc["buttons"].is<JsonObject>()) {
      for (JsonPair kv : doc["buttons"].as<JsonObject>()) {
        if (kv.value().as<int>() == 1) {
          pressButton(kv.key().c_str());
        }
      }
    }

    
    
    Serial.printf("Packet type: %s\n", type);
      
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
    if (millis() - lastDebug > 200) {
      Serial.print("JSON Parse Error: ");
      Serial.println(err.c_str());
    }
    }   
  }

  delay(1);

}