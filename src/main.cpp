#include <Arduino.h>
#include "Constants.h"
#include "SensorManager.h"
#include "AlertManager.h"
#include "DisplayManager.h"

// 1 startup
SensorManager sensorMgr(PIN_DHT, DHT22); 
AlertManager alertMgr;
DisplayManager displayMgr;

// Data Logging = Feature B
const int MAX_LOGS = 240; // 2 hours of data
struct LogRecord {
    float temp;
    float hum;
};
LogRecord logBuffer[MAX_LOGS];
int logHead = 0; 

//  Timing and State Variables 
unsigned long previousSensorRead = 0;
unsigned long previousSerialOutput = 0;
unsigned long previousDataLog = 0;

// Store values globally so all timers can use them
float currentT = 0.0;
float currentH = 0.0;
bool isSafe = true;

struct Peaks {
    float maxTemp;
    float maxHum;
};

//  Helper Functions 
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
    
    // Boot up all our separate hardware managers
    sensorMgr.begin();
    alertMgr.begin();
    displayMgr.begin();

    Serial.println("System Initialized - Modular Architecture Ready");
    delay(2000); 
}

void loop() {
    unsigned long currentMillis = millis();

    // 1 Feature A and C: Sensor Reading and Dashboard
    if (currentMillis - previousSensorRead >= 2000) {
        previousSensorRead = currentMillis;
        
        if (sensorMgr.readSensors()) {
            currentT = sensorMgr.getHumidity() + 8.5; 
            currentH = sensorMgr.getTemperature();    
            
            isSafe = (currentT >= STD_MODE.tempMin && currentT <= STD_MODE.tempMax &&
                      currentH >= STD_MODE.humMin && currentH <= STD_MODE.humMax);

            Peaks p = getPeakValues();
            
            displayMgr.updateDashboard(currentT, currentH, isSafe, p.maxTemp, p.maxHum);

            if (isSafe) {
                alertMgr.setNormalState();
            } else {
                alertMgr.triggerUnsafeAlarm();
            }
        }
    }

    // 2 Feature B: Serial Debug 
    if (currentMillis - previousSerialOutput >= INTERVAL_SERIAL) {
        previousSerialOutput = currentMillis;
        int light = analogRead(PIN_LDR);

        Serial.print("[5s DEBUG] T: "); Serial.print(currentT);
        Serial.print("C ("); Serial.print(getRangeStatus(currentT, STD_MODE.tempMin, STD_MODE.tempMax));
        Serial.print(") | H: "); Serial.print(currentH);
        Serial.print("% ("); Serial.print(getRangeStatus(currentH, STD_MODE.humMin, STD_MODE.humMax));
        Serial.print(") | LDR: "); Serial.println(light);
    }

    // 3 FEATURE B: Data Logging 
    if (currentMillis - previousDataLog >= INTERVAL_LOGGING) {
        previousDataLog = currentMillis;
        
        // Save values to memory
        logBuffer[logHead].temp = currentT;
        logBuffer[logHead].hum = currentH;
        
        logHead = (logHead + 1) % MAX_LOGS;
    }
}