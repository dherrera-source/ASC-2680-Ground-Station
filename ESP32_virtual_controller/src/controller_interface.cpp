#include "controller_interface.h"
#include "button_map.h"
#include <Arduino.h>

// --- Pin assignments ---
#define THROTTLE_PIN  25  // DAC1
#define PITCH_PIN     26  // DAC2

#define YAW_PIN       14  // PWM channel 0
#define ROLL_PIN      27  // PWM channel 1

// --- PWM settings ---
#define PWM_FREQ       5000
#define PWM_RES        13    // 13-bit resolution

// --- Neutral Voltages ---
float DAC_NEUTRAL = 1.65f;
uint16_t  PWM_NEUTRAL = 2.2f;

// --- Neutral values ---
float THROTTLE_CENTER   = 1.65f;
float PITCH_CENTER      = 1.65f;
float YAW_CENTER        = 2.2f;
float ROLL_CENTER       = 2.2f;


// -- Trim Offsets ---
float trim_throttle = 0.0f;
float trim_pitch   = 0.0f;
float trim_yaw    = 0.0f;    
float trim_roll   = 0.0f;    

float smoothedThrottle = 0.0f;
float smoothedYaw = 0.0f;
float smoothedPitch = 0.0f;
float smoothedRoll = 0.0f;

float smooth(float current, float target, float factor = 0.2f) {
    return current + (target - current) * factor;
}

float mapInputToVoltage(float input, float center, float trim) {
    const float Vrange = 1.65; // 3.3 / 2
    return center + trim + (input * Vrange);
}

void adjustThrottleTrim(float delta) {
    trim_throttle += delta;
}
void adjustPitchTrim(float delta) {
    trim_pitch += delta;
}
void adjustYawTrim(float delta) {
    trim_yaw += delta;
}
void adjustRollTrim(float delta) {
    trim_roll += delta;
}

void setThrottleTrim(float value) {
    trim_throttle = value;
}
void setPitchTrim(float value) {
    trim_pitch = value;
}
void setYawTrim(float value) {
    trim_yaw = value;
}
void setRollTrim(float value) {
    trim_roll = value;
}

void writeVoltageDAC(int pin, float voltage) {
    voltage = constrain(voltage, 0.0f, 3.3f);
    int dacValue = (voltage / 3.3f) * 255;  // 8-bit DAC
    dacWrite(pin, dacValue);
}

void writeVoltagePWM(int channel, int pin, float voltage) {
    voltage = constrain(voltage, 0.0f, 3.3f);
    int duty = (voltage / 3.3f) * 8191;  //13-bit PWM
    ledcWrite(channel, duty);
}

void initControllerInterface() {
    //Initialize buttons
    initButtonPins();

    // PWM channels
    ledcSetup(0, PWM_FREQ, PWM_RES);
    ledcAttachPin(YAW_PIN, 0);
    
    ledcSetup(1, PWM_FREQ, PWM_RES);
    ledcAttachPin(ROLL_PIN, 1);
    
    //Smoothed Values
    smoothedThrottle = 0.0f;
    smoothedYaw      = 0.0f;
    smoothedPitch    = 0.0f;
    smoothedRoll     = 0.0f;

    // Center all axes on startup
    writeVoltageDAC(THROTTLE_PIN, mapInputToVoltage(smoothedThrottle, THROTTLE_CENTER, trim_throttle));
    writeVoltageDAC(PITCH_PIN, mapInputToVoltage(smoothedPitch, PITCH_CENTER, trim_pitch));
    writeVoltagePWM((0), YAW_PIN, mapInputToVoltage(smoothedYaw, YAW_CENTER, trim_yaw));
    writeVoltagePWM((1), ROLL_PIN, mapInputToVoltage(smoothedRoll, ROLL_CENTER, trim_roll));

    
}
// --- Stick control functions ---
// --- DAC ---
// Throttle
void setThrottle(float v) {
    smoothedThrottle = smooth(smoothedThrottle, v);
    writeVoltageDAC(THROTTLE_PIN, mapInputToVoltage(smoothedThrottle, THROTTLE_CENTER, trim_throttle));
}
 // Pitch
void setPitch(float v) {
    smoothedPitch = smooth(smoothedPitch, v);
    writeVoltageDAC(PITCH_PIN, mapInputToVoltage(smoothedPitch, PITCH_CENTER, trim_pitch));
}
// --- PWM ---
// Yaw
void setYaw(float v) {
    smoothedYaw = smooth(smoothedYaw, v);
    writeVoltagePWM(0, YAW_PIN, mapInputToVoltage(smoothedYaw, YAW_CENTER, trim_yaw));
}
// Roll
void setRoll(float v) {
    smoothedRoll = smooth(smoothedRoll, v);
    writeVoltagePWM(1, ROLL_PIN, mapInputToVoltage(smoothedRoll, ROLL_CENTER, trim_roll));
}

// Button press
void pressButton(const char* name) {
    int pin = getButtonPin(name);

    Serial.print("[DEBUG] pressButton(): ");
    Serial.print(name);
    Serial.print(" -> pin ");
    Serial.println(pin);

    if (pin < 0) {
        Serial.println("[DEBUG] ERROR: Unknown button name");
        return;  // Unknown button
    }
    
    Serial.println("[DEBUG] Pin LOW");
    digitalWrite(pin, LOW);
    delay(120);

    Serial.println("[DEBUG] Pin HIGH");
    digitalWrite(pin, HIGH);
    
}


