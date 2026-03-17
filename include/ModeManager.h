#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include <Arduino.h>
#include "Constants.h"

// states the system can be in
enum SystemMode {
    STD,
    HIGH_SENS,
    LOCKOUT
};

class ModeManager {
private:
    SystemMode currentMode;
    Thresholds activeThresholds;

    // Timing for buttons and lockout
    unsigned long lastModeBtnPress;
    unsigned long lastStopBtnPress;
    unsigned long unsafeStartTime;
    
    bool currentlyUnsafe;

public:
    ModeManager();
    
    void begin();
    
    // Feature D functions
    void checkButtons();
    void updateSafetyState(bool isSafe);
    
    // Getters 
    SystemMode getCurrentMode();
    Thresholds getActiveThresholds();
};

#endif