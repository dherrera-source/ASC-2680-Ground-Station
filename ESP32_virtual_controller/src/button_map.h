#pragma once
#include <Arduino.h>

// Represents a single button mapping
struct ButtonMap {
        const char* name;   // Names used in JSON / Python
        int pin;            // ESP32 GPIO pin connected to the button pad
};

// Expose the button table + count to other modules
extern ButtonMap buttons[];
extern const int BUTTON_COUNT;

//Hellper to look up a button pin by name
int getButtonPin(const char* name);

void initButtonPins();