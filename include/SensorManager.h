/*
 * SensorManager.h
 * Wraps the DHT library to read temperature and humidity.
 * Note: the actual temp/humidity swap and offset are handled in main.cpp
 * because that's a wiring quirk, not a sensor library issue.
 */

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
    bool readSensors();
    bool isEnvironmentSafe(Thresholds limits);
    float getTemperature() { return currentTemp; }
    float getHumidity() { return currentHum; }
};

#endif
