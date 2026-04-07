/*
 * ModeManager.h
 * Handles the system's operating mode (STD, HIGH_SENS, INSPECTION, LOCKOUT).
 * Reads the mode button for single press, double-click, and 5s hold.
 * Also tracks unsafe states and triggers lockout when the spec conditions are met.
 */

#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include <Arduino.h>
#include "Constants.h"

// The four possible system states
enum SystemMode {
    STD,
    HIGH_SENS,
    INSPECTION,
    LOCKOUT
};

class ModeManager {
private:
    SystemMode currentMode;
    SystemMode previousUserMode;    // Remembered for returning from inspection

    Thresholds activeThresholds;

    // Button state tracking
    unsigned long btnPressStartTime;   // When current press began
    unsigned long lastReleaseTime;     // When last release happened (double-click window)
    bool lastBtnState;
    bool pendingSinglePress;           // True if waiting to confirm single vs double-click

    // Inspection state
    bool inspectionActive;

    // Lockout tracking — records each unsafe episode so we can check
    // the spec condition: 2+ episodes totalling >5min within a 15-min window
    struct UnsafeEpisode {
        unsigned long startTime;
        unsigned long duration;   // 0 means this episode is still going
    };
    static const int MAX_EPISODES = 20;
    UnsafeEpisode episodes[MAX_EPISODES];
    int episodeCount;
    bool currentlyUnsafe;
    unsigned long currentUnsafeStart;

    // Internal helpers
    void pruneOldEpisodes(unsigned long now);
    void checkLockoutCondition(unsigned long now);
    void applyMode(SystemMode mode);

public:
    ModeManager();
    void begin();

    // Called every loop iteration
    void checkButtons();
    void updateSafetyState(bool isSafe);

    // Getters
    SystemMode getCurrentMode();
    Thresholds getActiveThresholds();
};

#endif
