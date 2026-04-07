/*
 * AlertManager.h
 * Controls the RGB LED and piezo buzzer based on the current system state.
 * Each alert state has its own LED colour and sound pattern.
 */

#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include <Arduino.h>
#include "Constants.h"

// Each state maps to a different LED + buzzer behaviour
enum AlertState {
    ALERT_NORMAL,       // Green LED, no sound
    ALERT_UNSAFE,       // Red LED + tone every 2s
    ALERT_INSPECTION,   // Flashing Blue LED + 2 chirps every 3s
    ALERT_LOCKOUT       // Flashing Red/Blue LED + tone every 30s
};

class AlertManager {
private:
    AlertState currentState;
    unsigned long lastOutputTime;   // Last time a timed output occurred
    bool flashToggle;               // Toggles for flashing LEDs

    void playTone(int frequency, int durationMs);
    void playChirp();
    void allOff();

public:
    AlertManager();
    void begin();
    void setState(AlertState state);
    void update();  // Must be called every loop iteration for flashing
};

#endif
