#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

//  Pin Definitions 
const int PIN_DHT      = 4;   //humidity/temp sensor
const int PIN_RGB_R    = 15;  // red
const int PIN_RGB_G    = 21;  // green
const int PIN_RGB_B    = 19;  // blue
const int PIN_BUZZER   = 25;  // buzzer
const int PIN_BTN_MODE = 32;  // button mode switch
const int PIN_BTN_STOP = 33;  // button state
const int PIN_LDR      = 34;  // Photoresistor

// Screen Labels (already attached to board)
const int TFT_CS       = 5;   // Chip Select
const int TFT_DC       = 2;   // Data/Command
const int TFT_RST      = 12;  // reset
const int TFT_MOSI     = 23;  // SI (Serial In)
const int TFT_SCLK     = 18;  // SCK (Clock)

//  Struct Def
struct Thresholds {
    float tempMin;
    float tempMax;
    float humMin;
    float humMax;
};

// Monitoring Thresholds 
const Thresholds STD_MODE = {18.0, 35.0, 20.0, 60.0};
const Thresholds HIGH_SENS_MODE = {15.0, 25.0, 30.0, 55.0};

// Intervals 
const unsigned long INTERVAL_UNSAFE     = 2000;  
const unsigned long INTERVAL_INSPECTION = 3000;  
const unsigned long INTERVAL_LOCKOUT    = 30000; 
const unsigned long INTERVAL_LOGGING    = 30000; 
const unsigned long INTERVAL_SERIAL     = 5000;  

#endif