#include "button_map.h"

// -------------------------------------------
// Define your button GPIO pins here
//(Replace these with your actual wiring)
//-------------------------------------------
#define BTN_AUTO_START       32
#define BTN_AUTO_LAND        33
#define BTN_SPEED            25
#define BTN_STUNT            26
#define BTN_PHOTO            27
#define BTN_VIDEO            14

// Trim buttons (example place holders)
#define BTN_TRIM_THR_UP      12
#define BTN_TRIM_THR_DOWN    13
#define BTN_TRIM_YAW_LEFT    15
#define BTN_TRIM_YAW_RIGHT   22
#define BTN_TRIM_PITCH_UP    23
#define BTN_TRIM_PITCH_DOWN  19
#define BTN_TRIM_ROLL_LEFT   21
#define BTN_TRIM_ROLL_RIGHT  18

// -------------------------------------------
// Button lookup table
// -------------------------------------------
ButtonMap buttons[] = {
    {"auto_start",          BTN_AUTO_START},
    {"auto_land",           BTN_AUTO_LAND},
    {"speed",               BTN_SPEED},
    {"stunt",               BTN_STUNT},
    {"photo",               BTN_PHOTO},
    {"video",               BTN_VIDEO},

    {"trim_throttle_up",    BTN_TRIM_THR_UP},
    {"trim_throttle_down",  BTN_TRIM_THR_DOWN},
    {"trim_yaw_left",       BTN_TRIM_YAW_LEFT},
    {"trim_yaw_right",      BTN_TRIM_YAW_RIGHT},
    {"trim_pitch_up",       BTN_TRIM_PITCH_UP},
    {"trim_pitch_down",     BTN_TRIM_PITCH_DOWN},
    {"trim_roll_left",      BTN_TRIM_ROLL_LEFT},
    {"trim_roll_right",     BTN_TRIM_ROLL_RIGHT},
    // Add more as needed
};

// Total number of buttons
const int BUTTON_COUNT = sizeof(buttons) / sizeof(ButtonMap);

// -------------------------------------------
// Lookup function
// -------------------------------------------
int getButtonPin(const char* name) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (strcmp(buttons[i].name, name) == 0) {
            return buttons[i].pin;
        }
    }
    return -1; // Not found
}

// --------------------------------------------
//pinMode setupt
// --------------------------------------------

void initButtonPins() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        pinMode(buttons[i].pin, OUTPUT); 
        digitalWrite(buttons[i].pin, HIGH); //active-low idle state
    }
}

