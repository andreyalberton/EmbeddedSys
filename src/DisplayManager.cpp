#include "DisplayManager.h"

// Software SPI constructor — pins defined in Constants.h
DisplayManager::DisplayManager() : tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST) {}

// Initialise the TFT with the green-tab variant and clear the screen
void DisplayManager::begin() {
    pinMode(TFT_BL, OUTPUT);

    tft.initR(INITR_144GREENTAB);  
    tft.setRotation(0);
    tft.fillScreen(ST77XX_BLACK);
}

// Main screen — shows live temp/humidity, 2-hour peaks, mode, and safety status
void DisplayManager::updateDashboard(float temp, float hum, bool isSafe, float peakTemp, float peakHum, String modeName) {
    // Top Header: Mode indicator
    tft.setCursor(0, 2);
    tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.print("MODE: ");

    if (modeName == "STANDARD") tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    else if (modeName == "HIGH SENS") tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    else if (modeName == "USER CUSTOM") tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
    else if (modeName == "LOCKOUT!") tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
    else tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

    tft.println(modeName + "       ");

    // Live sensor values
    tft.setCursor(0, 20);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(2);
    tft.print("T: "); tft.print(temp, 1); tft.println("C  ");
    tft.setCursor(0, 40);
    tft.print("H: "); tft.print(hum, 1); tft.println("%  ");

    // 2-hour peak values
    tft.setCursor(0, 68);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.println("--- 2H PEAKS ---");
    tft.setCursor(0, 80);
    tft.print("Max T: "); tft.print(peakTemp, 1); tft.println("C  ");
    tft.setCursor(0, 92);
    tft.print("Max H: "); tft.print(peakHum, 1); tft.println("%  ");

    // Safety status
    tft.setCursor(0, 110);
    if (isSafe) {
        tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
        tft.println("STATUS: NORMAL    ");
    } else {
        tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
        tft.println("STATUS: UNSAFE    ");
    }
}

// Displayed when the machine is in inspection mode (double-click activated)
void DisplayManager::showInspectionScreen() {
    tft.setCursor(5, 10);
    tft.setTextColor(ST77XX_BLUE, ST77XX_BLACK);
    tft.setTextSize(2);
    tft.println("INSPECTION");

    tft.setCursor(5, 45);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    tft.println("Machine Stopped     ");

    tft.setCursor(5, 65);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.println("No thresholds active");
    tft.println("                    ");

    tft.setCursor(5, 95);
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.println("Restart machine to  ");
    tft.println("resume monitoring.  ");
}

// Edit screen — shows which threshold field the user is adjusting + current value
void DisplayManager::showEditScreen(String label, float value) {
    tft.setCursor(5, 10);
    tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
    tft.setTextSize(2);
    tft.println("USER MODE");

    tft.setCursor(5, 45);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.print(label);
    tft.println("          ");

    tft.setCursor(5, 65);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(3);
    tft.print(value, 1);
    tft.print("  ");

    tft.setCursor(5, 110);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    tft.print("Turn -> Adjust");
}

// Maps the LDR reading (0-4095) to a PWM brightness value (20-255)
// Floor of 20 keeps the backlight visible even in total darkness
void DisplayManager::setBrightness(int ldrValue) {
    int pwmValue = map(ldrValue, 0, 4095, 20, 255);
    pwmValue = constrain(pwmValue, 20, 255);
    analogWrite(TFT_BL, pwmValue);
}
