#include "AlertManager.h"

AlertManager::AlertManager() {}

void AlertManager::begin() {
    pinMode(PIN_RGB_R, OUTPUT);
    pinMode(PIN_RGB_G, OUTPUT);
    pinMode(PIN_RGB_B, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    
    // All off from start
    digitalWrite(PIN_RGB_R, LOW);
    digitalWrite(PIN_RGB_G, LOW);
    digitalWrite(PIN_RGB_B, LOW);
    digitalWrite(PIN_BUZZER, LOW);
}

// Helper function 
void AlertManager::playTone(int frequency, int durationMs) {
    long halfPeriod = 1000000L / frequency / 2; 
    long cycles = (long)frequency * durationMs / 1000L;
    for (long i = 0; i < cycles; i++) {
        digitalWrite(PIN_BUZZER, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(PIN_BUZZER, LOW);
        delayMicroseconds(halfPeriod);
    }
}

void AlertManager::setNormalState() {
    digitalWrite(PIN_RGB_G, HIGH);
    digitalWrite(PIN_RGB_R, LOW);
    digitalWrite(PIN_RGB_B, LOW);
    digitalWrite(PIN_BUZZER, LOW);
}

void AlertManager::triggerUnsafeAlarm() {
    digitalWrite(PIN_RGB_R, HIGH);
    digitalWrite(PIN_RGB_G, LOW);
    digitalWrite(PIN_RGB_B, LOW);
    playTone(1000, 200); 
}