/*
 * EditManager.h
 * Handles the rotary encoder for user-custom threshold editing (Feature E).
 * Push the encoder button to step through each field (T min, T max, H min, H max),
 * then turn the knob to adjust. After the last field it saves and activates USER MODE.
 */

#ifndef EDIT_MANAGER_H
#define EDIT_MANAGER_H

#include <Arduino.h>
#include "Constants.h"

// Tracks which field the user is currently editing
enum EditState {
    NOT_EDITING,
    EDIT_T_MIN,
    EDIT_T_MAX,
    EDIT_H_MIN,
    EDIT_H_MAX
};

class EditManager {
private:
    int lastClkState;
    unsigned long lastButtonPress;
    EditState currentState;
    Thresholds customLimits;
    bool userModeActive;
    int lastBtnState;

public:
    EditManager();
    void begin();

    // Reads the encoder pins every loop
    void update();

    // State checkers
    bool isEditing();
    bool isUserModeActive();
    void clearUserMode();  // Revert back to Standard/High Sens

    // Getters
    Thresholds getCustomLimits();
    String getActiveEditLabel();
    float getActiveEditValue();
};

#endif
