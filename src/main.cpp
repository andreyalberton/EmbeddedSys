#include <Arduino.h>
#include "Constants.h"
#include "SensorManager.h"
#include "AlertManager.h"
#include "DisplayManager.h"
#include "ModeManager.h"
#include "EditManager.h" 

bool wasEditing = false;

SensorManager sensorMgr(PIN_DHT, DHT11); 
AlertManager alertMgr;
DisplayManager displayMgr;
ModeManager modeMgr; 
EditManager editMgr; 

// Data Logging = Feature B
const int MAX_LOGS = 240;
struct LogRecord {
    float temp;
    float hum;
};
LogRecord logBuffer[MAX_LOGS];
int logHead = 0; 

// Timing and State Variables 
unsigned long previousSensorRead = 0;
unsigned long previousSerialOutput = 0;
unsigned long previousDataLog = 0;

float currentT = 0.0;
float currentH = 0.0;
bool isSafe = true;

// Track the old mode to know if the user pressed the standard Mode Button
SystemMode lastSysMode = STD; 

struct Peaks {
    float maxTemp;
    float maxHum;
};

// Helper Functions 
String getRangeStatus(float value, float minVal, float maxVal) {
    if (value < minVal) return "BELOW";
    if (value > maxVal) return "ABOVE";
    return "WITHIN";
}

Peaks getPeakValues() {
    float highT = -99.0;
    float highH = -99.0;
    int recordsToScan = (logHead < 240) ? logHead : 240;

    for (int i = 0; i < recordsToScan; i++) {
        int index = (logHead - 1 - i + MAX_LOGS) % MAX_LOGS;
        if (logBuffer[index].temp > highT) highT = logBuffer[index].temp;
        if (logBuffer[index].hum > highH) highH = logBuffer[index].hum;
    }
    return {highT, highH};
}

void setup() {
    Serial.begin(115200);
    
    sensorMgr.begin();
    alertMgr.begin();
    displayMgr.begin();
    modeMgr.begin(); 
    editMgr.begin(); // Boot the encoder

    Serial.println("System Initialized - Features A, B, C, D, and E Ready");
    delay(2000); 
}

void loop() {
    unsigned long currentMillis = millis();

    // Check all physical buttons continuously
    modeMgr.checkButtons();
    editMgr.update(); // Feature E: Reads the rotary knob

    if (modeMgr.getCurrentMode() != lastSysMode) {
        editMgr.clearUserMode();
        lastSysMode = modeMgr.getCurrentMode();
    }

    if (editMgr.isEditing()) {
        if (!wasEditing) {
            displayMgr.begin(); // Wipe screen black ONCE on entry
            wasEditing = true;
        }
        displayMgr.showEditScreen(editMgr.getActiveEditLabel(), editMgr.getActiveEditValue());
    } else if (wasEditing) {
        displayMgr.begin(); // Wipe screen black on exit
        wasEditing = false;
        
        // Force an immediate draw of the normal dashboard so it isn't blank
        Peaks p = getPeakValues();
        displayMgr.updateDashboard(currentT, currentH, isSafe, p.maxTemp, p.maxHum);
    }

    // 1 Sensor Reading and Dashboard 
    if (currentMillis - previousSensorRead >= 2000) {
        previousSensorRead = currentMillis;
        
        if (sensorMgr.readSensors()) {
            currentT = sensorMgr.getHumidity() + 8.5; 
            currentH = sensorMgr.getTemperature();    
            
            Thresholds activeLimits = editMgr.isUserModeActive() ? editMgr.getCustomLimits() : modeMgr.getActiveThresholds();
            
            isSafe = (currentT >= activeLimits.tempMin && currentT <= activeLimits.tempMax &&
                      currentH >= activeLimits.humMin && currentH <= activeLimits.humMax);

            modeMgr.updateSafetyState(isSafe);

            if (!editMgr.isEditing()) {
                Peaks p = getPeakValues();
                displayMgr.updateDashboard(currentT, currentH, isSafe, p.maxTemp, p.maxHum);
            }

            SystemMode currentSysMode = modeMgr.getCurrentMode();
            if (currentSysMode == LOCKOUT) {
                alertMgr.triggerUnsafeAlarm();
            } else if (isSafe) {
                alertMgr.setNormalState();
            } else {
                alertMgr.triggerUnsafeAlarm();
            }
        }
    }

    // 2 Debug Output
    if (currentMillis - previousSerialOutput >= INTERVAL_SERIAL) {
        previousSerialOutput = currentMillis;
        int light = analogRead(PIN_LDR);
        
        Thresholds activeLimits = editMgr.isUserModeActive() ? editMgr.getCustomLimits() : modeMgr.getActiveThresholds();

        Serial.print("[5s DEBUG] T: "); Serial.print(currentT);
        Serial.print("C ("); Serial.print(getRangeStatus(currentT, activeLimits.tempMin, activeLimits.tempMax));
        Serial.print(") | H: "); Serial.print(currentH);
        Serial.print("% ("); Serial.print(getRangeStatus(currentH, activeLimits.humMin, activeLimits.humMax));
        Serial.print(") | LDR: "); Serial.print(light);
        
        Serial.print(" | MODE: ");
        if (editMgr.isUserModeActive()) Serial.println("USER_CUSTOM");
        else if (modeMgr.getCurrentMode() == STD) Serial.println("STD");
        else if (modeMgr.getCurrentMode() == HIGH_SENS) Serial.println("HIGH_SENS");
        else Serial.println("LOCKOUT!!!");
    }

    // 3 Data Logging 
    if (currentMillis - previousDataLog >= INTERVAL_LOGGING) {
        previousDataLog = currentMillis;
        logBuffer[logHead].temp = currentT;
        logBuffer[logHead].hum = currentH;
        logHead = (logHead + 1) % MAX_LOGS;
    }
}