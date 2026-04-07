#include "EditManager.h"

// Defaults match STD_MODE so user starts from sensible values
EditManager::EditManager() {
    currentState = NOT_EDITING;
    userModeActive = false;
    customLimits = {18.0, 35.0, 20.0, 60.0};
    lastButtonPress = 0;
    lastClkState = HIGH;
    lastBtnState = LOW;
}

void EditManager::begin() {
    // INPUT_PULLUP: ties floating pins to stable 3.3V HIGH until grounded by user action
    pinMode(PIN_ENC_CLK, INPUT_PULLUP);
    pinMode(PIN_ENC_DT, INPUT_PULLUP);
    pinMode(PIN_ENC_SW, INPUT_PULLUP);

    lastClkState = digitalRead(PIN_ENC_CLK);
    lastBtnState = digitalRead(PIN_ENC_SW);
}

// Called every loop — reads encoder button and rotation
void EditManager::update() {
    unsigned long currentMillis = millis();
    int currentBtnState = digitalRead(PIN_ENC_SW);

    // 1. Button press cycles through edit fields (with 300ms debounce)
    if (currentBtnState == HIGH && lastBtnState == LOW) {
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

    lastBtnState = currentBtnState;

    if (currentState == NOT_EDITING) return;

    // 2. Rotation logic — reads CLK/DT phase shift to detect direction
    int currentClkState = digitalRead(PIN_ENC_CLK);

    if (currentClkState == LOW && lastClkState == HIGH) {

        static unsigned long lastTurnTime = 0;

        // Shield: ignore all bounces for 100ms after a tick
        if (currentMillis - lastTurnTime > 100) {

            float change = 0.0;

            if (digitalRead(PIN_ENC_DT) == HIGH) {
                change = 1.0;   // Clockwise  (+)
            } else {
                change = -1.0;  // Counter-CW (-)
            }

            switch (currentState) {
                case EDIT_T_MIN:
                    customLimits.tempMin += change;
                    if (customLimits.tempMin < 0) customLimits.tempMin = 0;
                    if (customLimits.tempMin > 50) customLimits.tempMin = 50;
                    if (customLimits.tempMin >= customLimits.tempMax) customLimits.tempMin = customLimits.tempMax - 1;
                    break;
                case EDIT_T_MAX:
                    customLimits.tempMax += change;
                    if (customLimits.tempMax < 0) customLimits.tempMax = 0;
                    if (customLimits.tempMax > 50) customLimits.tempMax = 50;
                    if (customLimits.tempMax <= customLimits.tempMin) customLimits.tempMax = customLimits.tempMin + 1;
                    break;
                case EDIT_H_MIN:
                    customLimits.humMin += change;
                    if (customLimits.humMin < 0) customLimits.humMin = 0;
                    if (customLimits.humMin > 100) customLimits.humMin = 100;
                    if (customLimits.humMin >= customLimits.humMax) customLimits.humMin = customLimits.humMax - 1;
                    break;
                case EDIT_H_MAX:
                    customLimits.humMax += change;
                    if (customLimits.humMax < 0) customLimits.humMax = 0;
                    if (customLimits.humMax > 100) customLimits.humMax = 100;
                    if (customLimits.humMax <= customLimits.humMin) customLimits.humMax = customLimits.humMin + 1;
                    break;
                default: break;
            }

            lastTurnTime = currentMillis;
        }
    }

    lastClkState = currentClkState;
}

bool EditManager::isEditing() { return currentState != NOT_EDITING; }
bool EditManager::isUserModeActive() { return userModeActive; }

// Reverts to preset mode — called on mode changes or lockout
void EditManager::clearUserMode() {
    userModeActive = false;
    currentState = NOT_EDITING;
}

Thresholds EditManager::getCustomLimits() { return customLimits; }

// Returns the label string for whichever field is being edited right now
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
