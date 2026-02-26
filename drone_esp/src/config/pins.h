#pragma once

// Ultrasonic sensors

#define US_LEFT_TRIG    14
#define US_LEFT_ECHO    27

#define US_RIGHT_TRIG   22
#define US_RIGHT_ECHO   23

#define GPS_RX_PIN 3  // ESP32 RX  <- GPS TX
#define GPS_TX_PIN 1  // ESP32 TX  -> GPS RX

// =========================
// Battery Monitoring Pins
// =========================

// Main drone LiPo battery (via voltage divider)
#define BATTERY_MAIN_PIN 34   // ADC1_CH6

// ESP32 auxiliary/button battery
#define BATTERY_AUX_PIN  35   // ADC1_CH7


// Add more as needed:
// #define US_BACK_TRIG ...
// #define US_BACK_ECHO ...
// #define US_DOWN_TRIG ...
// #define US_DOWN_ECHO ...