#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>
#include "Constants.h"

class DisplayManager {
private:
    Adafruit_ST7735 tft;

public:
    // Initialise the TFT 
    DisplayManager();
    void begin();
    
    // Passing the data it needs to draw the dashboard
    void updateDashboard(float temp, float hum, bool isSafe, float peakTemp, float peakHum);
    
    //  FEATURE E: The Edit Screen 
    void showEditScreen(String label, float value);
};

#endif