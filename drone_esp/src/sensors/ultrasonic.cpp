#include "ultrasonic.h"

Ultrasonic::Ultrasonic(int trigPin, int echoPin)
    : trig(trigPin), echo(echoPin) {}


void Ultrasonic::begin() {
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);   // prevents floating input when sensor unplugged
    digitalWrite(trig, LOW);

    // Initialize safe defaults
    data.distance_cm = -1.0f;
    data.valid = false;
}


void Ultrasonic::update() {
    unsigned long now = micros();

    // Step 1: Trigger pulse every 60ms
    if (!waitingForEcho && (now - lastTriggerTime > 60000)) {

        // Safety: ensure trig pin is valid
        digitalWrite(trig, LOW);
        delayMicroseconds(2);

        digitalWrite(trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig, LOW);

        lastTriggerTime = now;
        waitingForEcho = true;
        echoStart = 0;
        echoEnd = 0;
    }

    // Step 2: Non-blocking echo capture
    if (waitingForEcho ) {
        int state = digitalRead(echo);

        // Rising edge
        if (state == HIGH && echoStart == 0) {
            echoStart = micros();
        }

        // Falling edge
        if (state == LOW && echoStart != 0 && echoEnd == 0) {
            echoEnd = micros();
            waitingForEcho = false;

            unsigned long duration = echoEnd - echoStart;

            // HARDENING: reject impossible durations
            if (duration == 0 || duration > 30000) {
                data.distance_cm = -1.0f;
                data.valid = false;
                return;
            }

            float dist = duration * 0.0343f / 2.0f;

            // HARDENING: reject out-of-range values
            if (dist > 2 && dist < 400) {
                data.distance_cm = dist;
                data.valid = true;
            } else {
                data.distance_cm = -1.0f;
                data.valid = false;
                }
        }

        // HARDENING: Timeout (no echo received)
        if (waitingForEcho && (now - lastTriggerTime > 40000)) {
            waitingForEcho = false;
            data.distance_cm = -1.0f;
            data.valid = false;
        }
    }
}


UltrasonicData Ultrasonic::get() const {
    return data;
}
