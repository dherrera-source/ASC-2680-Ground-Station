#pragma once

#include "gps.h"
#include "ultrasonic.h"
#include "battery_main.h"
#include "battery_aux.h"

// Declare global sensor instances so other modules can access them
extern Ultrasonic leftUS;
extern Ultrasonic rightUS;

extern BatteryMain batteryMain;
extern BatteryAux  batteryAux;