#pragma once
#include <Arduino.h>
void initControllerInterface();

// --------------------------------------------
// Initialization
// --------------------------------------------
void initControllerInterface();

// --------------------------------------------
// Stick control functions
// Input range: -1.0 to +1.0
// These map to 0.0V to 3.3V outputs on the controller
// --------------------------------------------

void setThrottle(float v);
void setYaw(float v);
void setPitch(float v);
void setRoll(float v);

// --------------------------------------------
// Button control
// Buttons are active-low pulses
// Example names: "takeoff", "Land"
// --------------------------------------------
void pressButton(const char* name);


// --- Trim adjustment functions ---
void adjustThrottleTrim(float delta);
void adjustYawTrim(float delta);
void adjustPitchTrim(float delta);
void adjustRollTrim(float delta);

void setThrottleTrim(float value);
void setYawTrim(float value);
void setPitchTrim(float value);
void setRollTrim(float value);