#include "button_map.h"

// -------------------------------------------
// Define your button GPIO pins here
//(Replace these with your actual wiring)
//-------------------------------------------
#define BTN_AUTO_START_LAND       12
#define BTN_AUTO_LAND             13
#define BTN_SPEED                 33
#define BTN_STUNT                 32
#define BTN_PHOTO                 2


// -------------------------------------------
// Button lookup table
// -------------------------------------------
ButtonMap buttons[] = {
    {"power",               12},
    {"takeoff_land",        13},
    {"speed",               33},
    {"stunt",               32},
    {"photo",               2},
    // Add more as needed
};

// Total number of buttons
const int BUTTON_COUNT = sizeof(buttons) / sizeof(ButtonMap);

// -------------------------------------------
// Lookup function
// -------------------------------------------
int getButtonPin(const char* name) {
    for (auto &b : buttons) {
        if (strcmp(b.name, name) == 0 ) {
            Serial.print("[DEBUG] getButtonPin(): ");
            Serial.print(name);
            Serial.print(" -> ");
            Serial.println(b.pin);
            return b.pin;
        }
    }

    Serial.print("[DEBUG] getButtonPin(): UNKNOWN BUTTON ");
    Serial.println(name);
    return -1; // Not found
}

// --------------------------------------------
//pinMode setupt
// --------------------------------------------

void initButtonPins() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        pinMode(buttons[i].pin, OUTPUT); 
        digitalWrite(buttons[i].pin, LOW); //active-low idle state
    }
}

