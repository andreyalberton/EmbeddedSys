#include "SensorManager.h"

SensorManager::SensorManager(int pin, int type) : dht(pin, type) {}

void SensorManager::begin() {
    dht.begin();
}

bool SensorManager::readSensors() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Check if any reads failed
    if (isnan(h) || isnan(t)) {
        return false;
    }

    currentHum = h;
    currentTemp = t;
    return true;
}

bool SensorManager::isEnvironmentSafe(Thresholds limits) {
    // Check if values are within the bounds
    if (currentTemp < limits.tempMin || currentTemp > limits.tempMax) return false;
    if (currentHum < limits.humMin || currentHum > limits.humMax) return false;
    return true;
}