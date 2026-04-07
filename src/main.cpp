/*
 * main.cpp
 * Entry point for the CNC Machine Monitoring System.
 * Uses a non-blocking cyclic executive (millis-based, no delay) to:
 *   - Read sensors every 2s
 *   - Log data every 30s into a 24h circular buffer
 *   - Print debug info every 5s
 */

#include <Arduino.h>
#include "Constants.h"
#include "SensorManager.h"
#include "AlertManager.h"
#include "DisplayManager.h"
#include "ModeManager.h"
#include "EditManager.h"

// ─── Manager Instances ──────────────────────────────────────────
// Each manager handles one responsibility (OOP separation of concerns)
SensorManager sensorMgr(PIN_DHT, DHT11);
AlertManager alertMgr;
DisplayManager displayMgr;
ModeManager modeMgr;
EditManager editMgr;

// ─── Data Logging (Feature B) ───────────────────────────────────
// 24 hours of data at 30-second intervals = 2880 records
const int MAX_LOGS = 2880;
struct LogRecord {
    float temp;
    float hum;
};
LogRecord logBuffer[MAX_LOGS];
int logHead = 0;

// ─── Timing Variables ───────────────────────────────────────────
unsigned long previousSensorRead  = 0;
unsigned long previousSerialOutput = 0;
unsigned long previousDataLog     = 0;

// ─── Live State ─────────────────────────────────────────────────
float currentT = 0.0;
float currentH = 0.0;
bool isSafe = true;
bool wasEditing = false;
bool wasInspecting = false;

// Track mode changes for display refresh
SystemMode lastSysMode = STD;

// ─── Peak Value Tracking ────────────────────────────────────────
struct Peaks {
    float maxTemp;
    float maxHum;
};

// ─── Helper Functions ───────────────────────────────────────────

// Returns "BELOW", "WITHIN", or "ABOVE" for serial debug output
String getRangeStatus(float value, float minVal, float maxVal) {
    if (value < minVal) return "BELOW";
    if (value > maxVal) return "ABOVE";
    return "WITHIN";
}

// Converts the current mode enum to a display-friendly string
String getModeString(SystemMode sysMode, bool userCustom) {
    if (userCustom) return "USER CUSTOM";
    switch (sysMode) {
        case STD:        return "STANDARD";
        case HIGH_SENS:  return "HIGH SENS";
        case INSPECTION: return "INSPECT";
        case LOCKOUT:    return "LOCKOUT!";
        default:         return "STANDARD";
    }
}

// Scans the last 240 log entries (= 2 hours) for the highest temp and humidity
Peaks getPeakValues() {
    float highT = -99.0;
    float highH = -99.0;
    // Scan the last 240 records (2 hours at 30s intervals) for peaks
    int recordsToScan = (logHead < 240) ? logHead : 240;

    for (int i = 0; i < recordsToScan; i++) {
        int index = (logHead - 1 - i + MAX_LOGS) % MAX_LOGS;
        if (logBuffer[index].temp > highT) highT = logBuffer[index].temp;
        if (logBuffer[index].hum > highH)  highH = logBuffer[index].hum;
    }
    return {highT, highH};
}

// ─── Setup ──────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);

    sensorMgr.begin();
    alertMgr.begin();
    displayMgr.begin();
    modeMgr.begin();
    editMgr.begin();

    Serial.println("System Initialized - Features A, B, C, D, E Ready");
    delay(2000);
}

// ─── Main Loop ──────────────────────────────────────────────────
void loop() {
    unsigned long currentMillis = millis();

    // ── Continuous Input Polling (every iteration) ──────────────
    modeMgr.checkButtons();
    editMgr.update();

    // Update alert outputs (handles LED flashing at correct intervals)
    alertMgr.update();

    SystemMode sysMode = modeMgr.getCurrentMode();

    // ── Mode Change Detection ───────────────────────────────────
    if (sysMode != lastSysMode) {
        // If mode changed externally, cancel any in-progress editing
        if (lastSysMode != INSPECTION && lastSysMode != LOCKOUT) {
            editMgr.clearUserMode();
        }
        // Refresh screen on any mode transition
        displayMgr.begin();
        lastSysMode = sysMode;
    }

    // ── Inspection Mode Screen ──────────────────────────────────
    if (sysMode == INSPECTION) {
        if (!wasInspecting) {
            displayMgr.begin();
            wasInspecting = true;
        }
        displayMgr.showInspectionScreen();
        alertMgr.setState(ALERT_INSPECTION);
        return;  // Skip all other processing during inspection
    } else if (wasInspecting) {
        displayMgr.begin();
        wasInspecting = false;
    }

    // ── Edit Mode Screen (Feature E) ────────────────────────────
    if (editMgr.isEditing()) {
        // Don't allow editing during lockout
        if (sysMode == LOCKOUT) {
            editMgr.clearUserMode();
        } else {
            if (!wasEditing) {
                displayMgr.begin();
                wasEditing = true;
            }
            displayMgr.showEditScreen(editMgr.getActiveEditLabel(), editMgr.getActiveEditValue());
        }
    } else if (wasEditing) {
        displayMgr.begin();
        wasEditing = false;

        // Force immediate dashboard redraw after exiting edit
        String modeStr = getModeString(sysMode, editMgr.isUserModeActive());
        Peaks p = getPeakValues();
        displayMgr.updateDashboard(currentT, currentH, isSafe, p.maxTemp, p.maxHum, modeStr);
    }

    // ── Sensor Reading (every 2 seconds) ────────────────────────
    if (currentMillis - previousSensorRead >= 2000) {
        previousSensorRead = currentMillis;

        // LDR auto-dimming
        int currentLight = analogRead(PIN_LDR);
        displayMgr.setBrightness(currentLight);

        if (sensorMgr.readSensors()) {
            // NOTE: DHT11 data and header pins are swapped on this specific
            // board assembly. getHumidity() returns the temperature reading
            // and getTemperature() returns humidity. The +8.5 offset corrects
            // for the observed calibration drift on this particular sensor.
            currentT = sensorMgr.getHumidity() + 8.5;
            currentH = sensorMgr.getTemperature();

            // Get active thresholds based on current mode
            Thresholds activeLimits = editMgr.isUserModeActive()
                ? editMgr.getCustomLimits()
                : modeMgr.getActiveThresholds();

            isSafe = (currentT >= activeLimits.tempMin && currentT <= activeLimits.tempMax &&
                      currentH >= activeLimits.humMin  && currentH <= activeLimits.humMax);

            // Feed safety state to the lockout tracker
            modeMgr.updateSafetyState(isSafe);

            // Refresh the mode in case lockout just triggered
            sysMode = modeMgr.getCurrentMode();

            // Update dashboard (unless editing)
            if (!editMgr.isEditing()) {
                String modeStr = getModeString(sysMode, editMgr.isUserModeActive());
                Peaks p = getPeakValues();
                displayMgr.updateDashboard(currentT, currentH, isSafe, p.maxTemp, p.maxHum, modeStr);
            }

            // Set alert state based on current system mode
            if (sysMode == LOCKOUT) {
                alertMgr.setState(ALERT_LOCKOUT);
            } else if (isSafe) {
                alertMgr.setState(ALERT_NORMAL);
            } else {
                alertMgr.setState(ALERT_UNSAFE);
            }
        }
    }

    // ── Serial Debug Output (every 5 seconds) ───────────────────
    if (currentMillis - previousSerialOutput >= INTERVAL_SERIAL) {
        previousSerialOutput = currentMillis;
        int light = analogRead(PIN_LDR);

        Thresholds activeLimits = editMgr.isUserModeActive()
            ? editMgr.getCustomLimits()
            : modeMgr.getActiveThresholds();

        Serial.print("[5s DEBUG] T: "); Serial.print(currentT);
        Serial.print("C ("); Serial.print(getRangeStatus(currentT, activeLimits.tempMin, activeLimits.tempMax));
        Serial.print(") | H: "); Serial.print(currentH);
        Serial.print("% ("); Serial.print(getRangeStatus(currentH, activeLimits.humMin, activeLimits.humMax));
        Serial.print(") | LDR: "); Serial.print(light);

        Serial.print(" | MODE: ");
        Serial.println(getModeString(sysMode, editMgr.isUserModeActive()));
    }

    // ── Data Logging (every 30 seconds) ─────────────────────────
    if (currentMillis - previousDataLog >= INTERVAL_LOGGING) {
        previousDataLog = currentMillis;
        logBuffer[logHead].temp = currentT;
        logBuffer[logHead].hum  = currentH;
        logHead = (logHead + 1) % MAX_LOGS;
    }
}
