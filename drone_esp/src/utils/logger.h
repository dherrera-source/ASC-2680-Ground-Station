#pragma once
#include <Arduino.h>

namespace Logger {
    void info(const String& msg);
    void error(const String& msg);
}