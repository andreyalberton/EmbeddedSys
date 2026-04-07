/*
 * Constants.h
 * All pin assignments, threshold values, and timing intervals.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// ── Pin Definitions ─────────────────────────────────────────────
const int PIN_DHT      = 4;   // DHT11 Temperature & Humidity Sensor 
const int PIN_RGB_R    = 15;  // RGB LED - Red Anode
const int PIN_RGB_G    = 21;  // RGB LED - Green Anode
const int PIN_RGB_B    = 19;  // RGB LED - Blue Anode
const int PIN_BUZZER   = 25;  // Piezo Buzzer (PWM-driven)
const int PIN_BTN_MODE = 32;  // Mode Button (Active-Low, INPUT_PULLUP)
                               //   Short press  = toggle STD <-> HIGH_SENS
                               //   Double-click = toggle Inspection mode on/off
                               //   5s hold      = clear Lockout
const int PIN_LDR      = 34;  // Photoresistor / LDR 

#define PIN_ENC_CLK 26        // Rotary Encoder Clock
#define PIN_ENC_DT  27        // Rotary Encoder Data
#define PIN_ENC_SW  14        // Rotary Encoder Button

// TFT Display Pin Mapping (SPI)
const int TFT_CS       = 5;   // Chip Select
const int TFT_DC       = 2;   // Data/Command
const int TFT_RST      = 12;  // Reset
const int TFT_MOSI     = 23;  // SI (Serial In) - VSPI MOSI
const int TFT_SCLK     = 18;  // SCK (Clock) - VSPI CLK
const int TFT_BL       = 22;  // Backlight (PWM)

// ── Threshold Struct ────────────────────────────────────────────
// Holds min/max for temp and humidity — used by every mode
struct Thresholds {
    float tempMin;
    float tempMax;
    float humMin;
    float humMax;
};

// Default thresholds for the two preset operating modes (from the spec)
const Thresholds STD_MODE       = {18.0, 35.0, 20.0, 60.0};   // Standard ranges
const Thresholds HIGH_SENS_MODE = {15.0, 25.0, 30.0, 55.0};   // Tighter ranges for sensitive work

// Timing Intervals (milliseconds)
const unsigned long INTERVAL_UNSAFE          = 2000;   // Unsafe alarm repeat (2s)
const unsigned long INTERVAL_INSPECTION      = 3000;   // Inspection flash + chirps (3s)
const unsigned long INTERVAL_LOCKOUT_ALERT   = 30000;  // Lockout alert repeat (30s)
const unsigned long INTERVAL_LOGGING         = 30000;  // Data log interval (30s)
const unsigned long INTERVAL_SERIAL          = 5000;   // Serial debug output (5s)

// Double-click detection window
const unsigned long DOUBLE_CLICK_WINDOW      = 400;    // Max ms between two presses = double-click

// Lockout Trigger Conditions (per specification)
const unsigned long LOCKOUT_WINDOW           = 900000;  // 15-minute rolling window
const unsigned long LOCKOUT_CUMULATIVE       = 300000;  // 5 minutes cumulative unsafe
const int           LOCKOUT_MIN_TRIGGERS     = 2;       // Minimum 2 separate unsafe episodes
const unsigned long LOCKOUT_RESET_HOLD       = 5000;    // 5-second button hold to clear lockout

#endif
