/*
 * DisplayManager.h
 * Three screens: main dashboard, inspection notice, and the edit UI.
 * Also handles auto-dimming the backlight via the LDR reading.
 */

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
    DisplayManager();
    void begin();

    // Main dashboard with live data
    void updateDashboard(float temp, float hum, bool isSafe, float peakTemp, float peakHum, String modeName);

    // Feature D: Inspection mode screen
    void showInspectionScreen();

    // Feature E: The edit screen
    void showEditScreen(String label, float value);

    // Auto-dimming via LDR
    void setBrightness(int ldrValue);
};

#endif
