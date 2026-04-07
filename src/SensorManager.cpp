#include "SensorManager.h"

SensorManager::SensorManager(int pin, int type) : dht(pin, type) {}

void SensorManager::begin() {
    dht.begin();
}

// Reads both values from the DHT11 — returns false if the sensor gives NaN
bool SensorManager::readSensors() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Check if any reads failed (NaN = sensor error)
    if (isnan(h) || isnan(t)) {
        return false;
    }

    currentHum = h;
    currentTemp = t;
    return true;
}

// Returns true only if both temp and humidity are within the given limits
bool SensorManager::isEnvironmentSafe(Thresholds limits) {
    if (currentTemp < limits.tempMin || currentTemp > limits.tempMax) return false;
    if (currentHum < limits.humMin || currentHum > limits.humMax) return false;
    return true;
}
