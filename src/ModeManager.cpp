#include "ModeManager.h"

ModeManager::ModeManager() {
    currentMode = STD;
    activeThresholds = STD_MODE;
    lastModeBtnPress = 0;
    lastStopBtnPress = 0;
    unsafeStartTime = 0;
    currentlyUnsafe = false;
}

void ModeManager::begin() {
    // Both buttons need INPUT_PULLUP to work
    pinMode(PIN_BTN_MODE, INPUT_PULLUP);
    pinMode(PIN_BTN_STOP, INPUT_PULLUP);
}

void ModeManager::checkButtons() {
    unsigned long currentMillis = millis();

    // 1 Check Mode Button (Pin 32)
    if (digitalRead(PIN_BTN_MODE) == LOW) {
        // 300ms debounce to prevent double-clicking
        if (currentMillis - lastModeBtnPress > 300) {
            
            // Only allow swapping if we aren't locked out
            if (currentMode != LOCKOUT) {
                if (currentMode == STD) {
                    currentMode = HIGH_SENS;
                    activeThresholds = HIGH_SENS_MODE;
                    Serial.println("[MODE] Swapped to HIGH SENSITIVITY");
                } else {
                    currentMode = STD;
                    activeThresholds = STD_MODE;
                    Serial.println("[MODE] Swapped to STANDARD");
                }
            }
            lastModeBtnPress = currentMillis;
        }
    }

    // 2 Check Stop Button (Pin 33) - Used to clear the Lockout
    if (digitalRead(PIN_BTN_STOP) == LOW) {
        if (currentMillis - lastStopBtnPress > 300) {
            
            if (currentMode == LOCKOUT) {
                // Reset back to standard when stopped
                currentMode = STD;
                activeThresholds = STD_MODE;
                currentlyUnsafe = false;
                Serial.println("[LOCKOUT CLEARED] Returning to Standard Mode");
            }
            lastStopBtnPress = currentMillis;
        }
    }
}

void ModeManager::updateSafetyState(bool isSafe) {
    unsigned long currentMillis = millis();

    // If locked out, do nothing until the Stop button is pressed
    if (currentMode == LOCKOUT) {
        return; 
    }

    if (!isSafe) {
        // Just became unsafe, start the timer
        if (!currentlyUnsafe) {
            currentlyUnsafe = true;
            unsafeStartTime = currentMillis;
        } 
        // Been unsafe for longer than the lockout interval (30 seconds)
        else if (currentMillis - unsafeStartTime >= INTERVAL_LOCKOUT) {
            currentMode = LOCKOUT;
            Serial.println("!!! SYSTEM LOCKOUT ACTIVATED !!!");
        }
    } else {
        // Environment is safe again, reset the unsafe tracking
        currentlyUnsafe = false;
    }
}

SystemMode ModeManager::getCurrentMode() {
    return currentMode;
}

Thresholds ModeManager::getActiveThresholds() {
    return activeThresholds;
}