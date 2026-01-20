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

float smoothedThrottle = 0.0f;
float smoothedYaw = 0.0f;
float smoothedPitch = 0.0f;
float smoothedRoll = 0.0f;

float smooth(float current, float target, float factor = 0.2f) {
    return current + (target - current) * factor;
}
float mapInputToVoltage(float input) {
    const float Vcenter = 1.65f;
    const float Vrange = 1.65; // 3.3 / 2
    return Vcenter + (input * Vrange);
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

    //Smoothed Values
    smoothedThrottle = 0.0f;
    smoothedYaw      = 0.0f;
    smoothedPitch    = 0.0f;
    smoothedRoll     = 0.0f;

    // Center all axes on startup
    writeVoltageDAC(THROTTLE_PIN, 1.65f);
    writeVoltageDAC(PITCH_PIN, 1.65f);
    writeVoltagePWM((0), YAW_PIN, 1.65f);
    writeVoltagePWM((1), ROLL_PIN, 1.65f);

    // PWM channels
    ledcSetup(0, PWM_FREQ, PWM_RES);
    ledcAttachPin(YAW_PIN, 0);
    
    ledcSetup(1, PWM_FREQ, PWM_RES);
    ledcAttachPin(ROLL_PIN, 1);
    
}

// Throttle
void setThrottle(float v) {
    smoothedThrottle = smooth(smoothedThrottle, v);
    writeVoltageDAC(THROTTLE_PIN, mapInputToVoltage(smoothedThrottle));
}
 // Yaw
void setYaw(float v) {
    smoothedYaw = smooth(smoothedYaw, v);
    writeVoltagePWM(0, YAW_PIN, mapInputToVoltage(smoothedYaw));
}
 // Pitch
void setPitch(float v) {
    smoothedPitch = smooth(smoothedPitch, v);
    writeVoltageDAC(PITCH_PIN, mapInputToVoltage(smoothedPitch));
}

// Roll
void setRoll(float v) {
    smoothedRoll = smooth(smoothedRoll, v);
    writeVoltagePWM(1, ROLL_PIN, mapInputToVoltage(smoothedRoll));
}

// Button press
void pressButton(const char* name) {
    int pin = getButtonPin(name);
    if (pin < 0) return;  // Unknown button

    digitalWrite(pin, LOW);
    delay(120);
    digitalWrite(pin, HIGH);
    
}


