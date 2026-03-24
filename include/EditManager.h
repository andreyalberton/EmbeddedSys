#ifndef EDIT_MANAGER_H
#define EDIT_MANAGER_H

#include <Arduino.h>
#include "Constants.h"

// The different stages of the Distinct Edit Screen
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

public:
    EditManager();
    void begin();
    
    // Reads the encoder pins 
    void update(); 
    
    // State checkers for main.cpp
    bool isEditing();
    bool isUserModeActive();
    void clearUserMode(); // Used to revert back to Standard/High Sens
    
    // Getters
    Thresholds getCustomLimits();
    String getActiveEditLabel(); 
    float getActiveEditValue();  
};

#endif