#include "DisplayManager.h"

DisplayManager::DisplayManager() : tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST) {}

void DisplayManager::begin() {
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(0); 
    tft.fillScreen(ST77XX_BLACK);
}

void DisplayManager::updateDashboard(float temp, float hum, bool isSafe, float peakTemp, float peakHum) {
    // Reset to top left
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

//  FEATURE E: The Edit Screen 
void DisplayManager::showEditScreen(String label, float value) {

    // Header
    tft.setCursor(5, 10);
    tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK); 
    tft.setTextSize(2);
    tft.println("USER MODE      "); 

    // Labels
    tft.setCursor(5, 50);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.print(label);
    tft.println("          "); // Blank spaces to erase leftover letters

    // The changing value 
    tft.setCursor(5, 75);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(3); 
    tft.print(value, 1); 
    tft.print("   "); // Blank spaces to erase leftover numbers

    // Instructions at the bottom
    tft.setCursor(5, 140);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    tft.print("Turn -> Adjust ");
}