#include "ModeManager.h"

// Constructor — everything starts in STD mode with no pending presses
ModeManager::ModeManager() {
    currentMode      = STD;
    previousUserMode = STD;
    activeThresholds = STD_MODE;

    btnPressStartTime  = 0;
    lastReleaseTime    = 0;
    lastBtnState       = HIGH;
    pendingSinglePress = false;
    inspectionActive   = false;

    episodeCount      = 0;
    currentlyUnsafe   = false;
    currentUnsafeStart = 0;
}

void ModeManager::begin() {
    pinMode(PIN_BTN_MODE, INPUT_PULLUP);
}

// Sets the mode and updates the thresholds to match
void ModeManager::applyMode(SystemMode mode) {
    currentMode = mode;
    if (mode == STD)       activeThresholds = STD_MODE;
    if (mode == HIGH_SENS) activeThresholds = HIGH_SENS_MODE;
}

// ─── Button Handling ────────────────────────────────────────────
// Single short press  -> toggle STD <-> HIGH_SENS
// Double-click        -> toggle Inspection mode on/off
// 5-second hold       -> clear Lockout
void ModeManager::checkButtons() {
    unsigned long now = millis();
    bool currentBtnState = digitalRead(PIN_BTN_MODE);

    // ── Detect falling edge (button pressed) ──
    if (currentBtnState == LOW && lastBtnState == HIGH) {
        btnPressStartTime = now;
    }

    // ── Detect rising edge (button released) ──
    if (currentBtnState == HIGH && lastBtnState == LOW) {
        unsigned long pressDuration = now - btnPressStartTime;

        // Ignore bounces under 50ms
        if (pressDuration < 50) {
            lastBtnState = currentBtnState;
            return;
        }

        // ── 5-second long press: clear lockout ──
        if (pressDuration >= LOCKOUT_RESET_HOLD) {
            if (currentMode == LOCKOUT) {
                applyMode(previousUserMode);
                currentlyUnsafe   = false;
                episodeCount      = 0;
                inspectionActive  = false;
                Serial.println("[LOCKOUT CLEARED] Returning to previous mode via 5s hold");
            }
            lastReleaseTime    = 0;
            pendingSinglePress = false;
            lastBtnState = currentBtnState;
            return;
        }

        // ── Short press: check for double-click ──
        if (pressDuration >= 50 && pressDuration < LOCKOUT_RESET_HOLD) {
            if (currentMode == LOCKOUT) {
                // Ignore short presses during lockout
                lastBtnState = currentBtnState;
                return;
            }

            // Was there a recent release? -> double-click
            if (pendingSinglePress && (now - lastReleaseTime) <= DOUBLE_CLICK_WINDOW) {
                // ── Double-click: toggle inspection mode ──
                pendingSinglePress = false;
                if (!inspectionActive) {
                    // Enter inspection: remember current user mode
                    previousUserMode = currentMode;
                    inspectionActive = true;
                    currentMode      = INSPECTION;
                    Serial.println("[MODE] INSPECTION MODE activated via double-click");
                } else {
                    // Exit inspection: return to remembered mode
                    inspectionActive = false;
                    applyMode(previousUserMode);
                    Serial.print("[MODE] Inspection ended. Returning to: ");
                    Serial.println((currentMode == HIGH_SENS) ? "HIGH_SENS" : "STD");
                }
            } else {
                // No recent press yet — mark as pending, wait for possible second click
                pendingSinglePress = true;
                lastReleaseTime    = now;
            }
        }
    }

    // ── Resolve pending single press after double-click window expires ──
    if (pendingSinglePress && (now - lastReleaseTime) > DOUBLE_CLICK_WINDOW) {
        pendingSinglePress = false;

        if (currentMode == LOCKOUT || currentMode == INSPECTION) {
            // No mode toggle during lockout or inspection
            lastBtnState = currentBtnState;
            return;
        }

        // ── Single press: toggle STD <-> HIGH_SENS ──
        if (currentMode == STD) {
            applyMode(HIGH_SENS);
            previousUserMode = HIGH_SENS;
            Serial.println("[MODE] Swapped to HIGH SENSITIVITY");
        } else if (currentMode == HIGH_SENS) {
            applyMode(STD);
            previousUserMode = STD;
            Serial.println("[MODE] Swapped to STANDARD");
        }
    }

    lastBtnState = currentBtnState;
}

// ───────────── Lockout Logic ─────────────────────────────

// Removes episodes older than 15 minutes so only recent ones count
void ModeManager::pruneOldEpisodes(unsigned long now) {
    int writeIdx = 0;
    for (int i = 0; i < episodeCount; i++) {
        if ((now - episodes[i].startTime) < LOCKOUT_WINDOW) {
            if (writeIdx != i) episodes[writeIdx] = episodes[i];
            writeIdx++;
        }
    }
    episodeCount = writeIdx;
}

// Checks if lockout should activate: 2+ triggers AND >5min cumulative in the window
void ModeManager::checkLockoutCondition(unsigned long now) {
    pruneOldEpisodes(now);

    int triggerCount = 0;
    unsigned long cumulativeDuration = 0;

    for (int i = 0; i < episodeCount; i++) {
        triggerCount++;
        cumulativeDuration += (episodes[i].duration > 0)
            ? episodes[i].duration
            : (now - episodes[i].startTime);
    }

    if (triggerCount >= LOCKOUT_MIN_TRIGGERS && cumulativeDuration >= LOCKOUT_CUMULATIVE) {
        currentMode = LOCKOUT;
        Serial.println("!!! SYSTEM LOCKOUT ACTIVATED !!!");
        Serial.print("[LOCKOUT] Triggers: "); Serial.print(triggerCount);
        Serial.print(" | Cumulative unsafe: "); Serial.print(cumulativeDuration / 1000);
        Serial.println("s in 15-min window");
    }
}

// Called every sensor read — tracks when the environment goes unsafe/safe
// and logs each episode for the lockout condition check
void ModeManager::updateSafetyState(bool isSafe) {
    unsigned long now = millis();

    if (currentMode == LOCKOUT || currentMode == INSPECTION) return;

    if (!isSafe) {
        if (!currentlyUnsafe) {
            currentlyUnsafe     = true;
            currentUnsafeStart  = now;

            // Record new episode (shift buffer if full)
            if (episodeCount >= MAX_EPISODES) {
                for (int i = 1; i < MAX_EPISODES; i++) episodes[i - 1] = episodes[i];
                episodeCount = MAX_EPISODES - 1;
            }
            episodes[episodeCount].startTime = now;
            episodes[episodeCount].duration  = 0;
            episodeCount++;
        }
        checkLockoutCondition(now);
    } else {
        if (currentlyUnsafe) {
            currentlyUnsafe = false;
            for (int i = episodeCount - 1; i >= 0; i--) {
                if (episodes[i].duration == 0) {
                    episodes[i].duration = now - episodes[i].startTime;
                    break;
                }
            }
        }
    }
}

// ─── Getters ────────────────────────────────────────────────────
SystemMode ModeManager::getCurrentMode() { return currentMode; }

// In inspection mode, thresholds are widened so nothing triggers alerts
Thresholds ModeManager::getActiveThresholds() {
    if (currentMode == INSPECTION) return {0.0, 99.0, 0.0, 100.0};
    return activeThresholds;
}
