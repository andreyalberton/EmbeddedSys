#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

//  Pin Definitions 
const int PIN_DHT      = 26;   
const int PIN_RGB_R    = 18;  
const int PIN_RGB_G    = 21;  
const int PIN_RGB_B    = 19;  
const int PIN_BUZZER   = 22;  
const int PIN_BTN_MODE = 32;  
const int PIN_BTN_STOP = 33;  

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