#include "logger.h"

namespace Logger {

void info(const String& msg) {
    Serial.println("[INFO] " + msg);
}

void error(const String& msg) {
    Serial.println("[ERROR] " + msg);
}

}