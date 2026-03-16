#include "DisplayManager.h"

DisplayManager::DisplayManager() : tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST) {}

void DisplayManager::begin() {
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(0); 
    tft.fillScreen(ST77XX_BLACK);
}

void DisplayManager::updateDashboard(float temp, float hum, bool isSafe, float peakTemp, float peakHum) {
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); 
    tft.setTextSize(1);
    tft.println("CNC MONITOR - STD");
    
    // Current Values
    tft.setCursor(0, 20);
    tft.setTextSize(2);
    tft.print("T: "); tft.print(temp); tft.println("C   ");
    tft.print("H: "); tft.print(hum); tft.println("%   ");
    
    // Peak Values
    tft.setCursor(0, 70);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.println("--- 2H PEAKS ---");
    tft.print("Max T: "); tft.print(peakTemp); tft.println("C   ");
    tft.print("Max H: "); tft.print(peakHum); tft.println("%   ");

    // Status 
    if (isSafe) {
        tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
        tft.println("\nSTATUS: NORMAL   "); 
    } else {
        tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
        tft.println("\nSTATUS: UNSAFE   ");
    }
}