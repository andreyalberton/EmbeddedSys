#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <DHT.h>
#include "Constants.h" 

class SensorManager {
private:
    DHT dht;
    float currentTemp;
    float currentHum;

public:
    SensorManager(int pin, int type);
    void begin();
    bool readSensors(); // Returns true if successful
    bool isEnvironmentSafe(Thresholds limits); // Validates it
    float getTemperature() { return currentTemp; }
    float getHumidity() { return currentHum; }
};

#endif