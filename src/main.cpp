#include <Arduino.h>
#include "Constants.h"
#include "SensorManager.h"

SensorManager sensorMgr(PIN_DHT, DHT11);

// FEATURE B: Memory logging (volatile)
// 
const int MAX_LOGS = 2880; // 24 hours at 30s intervals
struct LogRecord {
    float temp;
    float hum;
};
LogRecord logBuffer[MAX_LOGS];
int logHead = 0; // Tracks where we its writting in the array

// timing variables
 
unsigned long previousSensorRead = 0;
unsigned long previousSerialOutput = 0;
unsigned long previousDataLog = 0;

void setup() {
    Serial.begin(115200);
    sensorMgr.begin();

    pinMode(PIN_RGB_R, OUTPUT);
    pinMode(PIN_RGB_G, OUTPUT);
    pinMode(PIN_RGB_B, OUTPUT);

    Serial.println("System Initialized. Starting Feature A & B.");

    // Warm up the DHT11 sensor
    delay(2000); 
}

// Function for Feature B: Checks if value is above/below/within 
String getRangeStatus(float value, float minVal, float maxVal) {
    if (value < minVal) return "BELOW";
    if (value > maxVal) return "ABOVE";
    return "WITHIN";
}

void loop() {
    // Get the current "stopwatch" time
    unsigned long currentMillis = millis();

    // ---------------------------------------------------------
    // 1. FEATURE A: Core Loop (Runs every 2 seconds)
    // ---------------------------------------------------------
    if (currentMillis - previousSensorRead >= 2000) {
        previousSensorRead = currentMillis;
        
        if (sensorMgr.readSensors()) {
            if (sensorMgr.isEnvironmentSafe(STD_MODE)) {
                digitalWrite(PIN_RGB_R, LOW);
                digitalWrite(PIN_RGB_G, HIGH); 
                digitalWrite(PIN_RGB_B, LOW);
            } else {
                digitalWrite(PIN_RGB_R, HIGH);
                digitalWrite(PIN_RGB_G, LOW);
                digitalWrite(PIN_RGB_B, LOW);
            }
        } else {
            // Blue LED for sensor error
            digitalWrite(PIN_RGB_R, LOW);
            digitalWrite(PIN_RGB_G, LOW);
            digitalWrite(PIN_RGB_B, HIGH);
        }
    }

    // ---------------------------------------------------------
    // 2. FEATURE B: Serial Debug Output (Runs every 5 seconds) 
    // ---------------------------------------------------------
    if (currentMillis - previousSerialOutput >= INTERVAL_SERIAL) {
        previousSerialOutput = currentMillis;
        
        float t = sensorMgr.getTemperature();
        float h = sensorMgr.getHumidity();
        
        Serial.print("[5s DEBUG] Temp: ");
        Serial.print(t);
        Serial.print("C (");
        Serial.print(getRangeStatus(t, STD_MODE.tempMin, STD_MODE.tempMax));
        Serial.print(") | Hum: ");
        Serial.print(h);
        Serial.print("% (");
        Serial.print(getRangeStatus(h, STD_MODE.humMin, STD_MODE.humMax));
        Serial.println(")");
    }

    // ---------------------------------------------------------
    // 3. FEATURE B: Data Logging (Runs every 30 seconds) 
    // ---------------------------------------------------------
    if (currentMillis - previousDataLog >= INTERVAL_LOGGING) {
        previousDataLog = currentMillis;
        
        // Save current values to the buffer
        logBuffer[logHead].temp = sensorMgr.getTemperature();
        logBuffer[logHead].hum = sensorMgr.getHumidity();
        
        Serial.print("[30s LOG] Data saved to memory slot: ");
        Serial.println(logHead);

        // Move the head forward. If we hit 2880, loop back to 0 
        logHead++;
        if (logHead >= MAX_LOGS) {
            logHead = 0; 
        }
    }
}