#include "EditManager.h"

EditManager::EditManager() {
    currentState = NOT_EDITING;
    userModeActive = false;
    customLimits = {18.0, 35.0, 20.0, 60.0}; 
    lastButtonPress = 0;
    lastClkState = HIGH;
}

void EditManager::begin() {
    pinMode(PIN_ENC_CLK, INPUT_PULLUP);
    pinMode(PIN_ENC_DT, INPUT_PULLUP);
    pinMode(PIN_ENC_SW, INPUT_PULLUP);
    lastClkState = digitalRead(PIN_ENC_CLK);
}

void EditManager::update() {
    unsigned long currentMillis = millis();

    // 1 Check button of encoder
    if (digitalRead(PIN_ENC_SW) == LOW) {
        if (currentMillis - lastButtonPress > 300) {
            if (currentState == NOT_EDITING) currentState = EDIT_T_MIN;
            else if (currentState == EDIT_T_MIN) currentState = EDIT_T_MAX;
            else if (currentState == EDIT_T_MAX) currentState = EDIT_H_MIN;
            else if (currentState == EDIT_H_MIN) currentState = EDIT_H_MAX;
            else if (currentState == EDIT_H_MAX) {
                currentState = NOT_EDITING;
                userModeActive = true; 
                Serial.println("[EDIT] Saved. USER MODE ACTIVATED.");
            }
            lastButtonPress = currentMillis;
        }
    }

    if (currentState == NOT_EDITING) return;

    // 2 Check rotation 
    int currentClkState = digitalRead(PIN_ENC_CLK);

    // register on the moment the pin drops low
    if (currentClkState == LOW && lastClkState == HIGH) {
        
        float change = 0.0;
        
        // Read the other pin to see which way we are turning
        if (digitalRead(PIN_ENC_DT) == HIGH) {
            change = 1.0;  // Right (+)
        } else {
            change = -1.0; // Left (-)
        }

        switch (currentState) {
            case EDIT_T_MIN:
                customLimits.tempMin += change;
                if (customLimits.tempMin < 0) customLimits.tempMin = 0;
                if (customLimits.tempMin >= customLimits.tempMax) customLimits.tempMin = customLimits.tempMax - 1;
                break;
            case EDIT_T_MAX:
                customLimits.tempMax += change;
                if (customLimits.tempMax > 50) customLimits.tempMax = 50;
                if (customLimits.tempMax <= customLimits.tempMin) customLimits.tempMax = customLimits.tempMin + 1;
                break;
            case EDIT_H_MIN:
                customLimits.humMin += change;
                if (customLimits.humMin < 0) customLimits.humMin = 0;
                if (customLimits.humMin >= customLimits.humMax) customLimits.humMin = customLimits.humMax - 1;
                break;
            case EDIT_H_MAX:
                customLimits.humMax += change;
                if (customLimits.humMax > 100) customLimits.humMax = 100;
                if (customLimits.humMax <= customLimits.humMin) customLimits.humMax = customLimits.humMin + 1;
                break;
            default: break;
        }
        
        // A 5ms freeze for the mechanical bouncing inside the knob
        delay(5); 
    }
    
    lastClkState = currentClkState; 
}

bool EditManager::isEditing() { return currentState != NOT_EDITING; }
bool EditManager::isUserModeActive() { return userModeActive; }

void EditManager::clearUserMode() { 
    userModeActive = false; 
    currentState = NOT_EDITING; 
}

Thresholds EditManager::getCustomLimits() { return customLimits; }

String EditManager::getActiveEditLabel() {
    if (currentState == EDIT_T_MIN) return "Temp Min (C)";
    if (currentState == EDIT_T_MAX) return "Temp Max (C)";
    if (currentState == EDIT_H_MIN) return "Hum Min (%)";
    if (currentState == EDIT_H_MAX) return "Hum Max (%)";
    return "";
}

float EditManager::getActiveEditValue() {
    if (currentState == EDIT_T_MIN) return customLimits.tempMin;
    if (currentState == EDIT_T_MAX) return customLimits.tempMax;
    if (currentState == EDIT_H_MIN) return customLimits.humMin;
    if (currentState == EDIT_H_MAX) return customLimits.humMax;
    return 0.0;
}