#pragma once
#include <Arduino.h>

namespace Telemetry {

    // Initialize telemetry system (timers, buffers, etc.)
    void begin();

    // Called every loop; sends telemetry at a fixed rate
    void update();

}