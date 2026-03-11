#include <Arduino.h>
#include "Constants.h"
#include "SensorManager.h"

// Instantiate your sensor manager 
SensorManager sensorMgr(PIN_DHT, DHT11);

void setup() {
    // Start serial communication 
    Serial.begin(115200);
    
    // Start the DHT sensor
    sensorMgr.begin();
    
    // Configure the LED pins as outputs
    pinMode(PIN_RGB_R, OUTPUT);
    pinMode(PIN_RGB_G, OUTPUT);
    pinMode(PIN_RGB_B, OUTPUT);

    Serial.println("System Initialized. Starting Feature A:");
}

void loop() {
    // Read the sensor
    if (sensorMgr.readSensors()) {
        
        // Check if the environment is safe 
        if (sensorMgr.isEnvironmentSafe(STD_MODE)) {
            // SAFE: Turn light GREEN
            digitalWrite(PIN_RGB_R, LOW);
            digitalWrite(PIN_RGB_G, HIGH); 
            digitalWrite(PIN_RGB_B, LOW);
        } else {
            // UNSAFE: Turn light RED
            digitalWrite(PIN_RGB_R, HIGH);
            digitalWrite(PIN_RGB_G, LOW);
            digitalWrite(PIN_RGB_B, LOW);
        }
    } else {
            digitalWrite(PIN_RGB_R, LOW);
            digitalWrite(PIN_RGB_G, LOW);
            digitalWrite(PIN_RGB_B, HIGH);
        Serial.println("Failed to read from DHT sensor!");
    }
    
    // Small delay 
    delay(2000); 
}