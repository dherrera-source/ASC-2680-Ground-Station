#pragma once
#include <Arduino.h>

struct UltrasonicData {
    float distance_cm = -1.0f;   // -1 means invalid or no reading
    bool valid = false;          // true only when a good echo was received
};

class Ultrasonic {
public:
    Ultrasonic(int trigPin, int echoPin);

    void begin();     // configure pins
    void update();    // non-blocking state machine
    UltrasonicData get() const;

private:
    int trig;
    int echo;

    unsigned long lastTriggerTime = 0;
    unsigned long echoStart = 0;
    unsigned long echoEnd = 0;

    bool waitingForEcho = false;
    UltrasonicData data;
};