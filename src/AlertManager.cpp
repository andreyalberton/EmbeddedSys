#include "AlertManager.h"

// Start in normal state — green LED, no buzzer
AlertManager::AlertManager() {
    currentState = ALERT_NORMAL;
    lastOutputTime = 0;
    flashToggle = false;
}

void AlertManager::begin() {
    pinMode(PIN_RGB_R, OUTPUT);
    pinMode(PIN_RGB_G, OUTPUT);
    pinMode(PIN_RGB_B, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    allOff();
}

void AlertManager::allOff() {
    digitalWrite(PIN_RGB_R, LOW);
    digitalWrite(PIN_RGB_G, LOW);
    digitalWrite(PIN_RGB_B, LOW);
    digitalWrite(PIN_BUZZER, LOW);
}

// Generates a tone via PWM bit-banging on the buzzer pin
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

// Short beep for inspection 
void AlertManager::playChirp() {
    playTone(2000, 50);
}

// Only resets LEDs if the state actually changed — avoids flicker
void AlertManager::setState(AlertState state) {
    if (state != currentState) {
        currentState = state;
        allOff();
        lastOutputTime = 0;   // Force immediate output on state change
        flashToggle = false;
    }
}

// Must be called every loop() — handles timed flashing and sound repeats
void AlertManager::update() {
    unsigned long now = millis();

    switch (currentState) {

        case ALERT_NORMAL:
            // Steady green LED, no sound
            digitalWrite(PIN_RGB_G, HIGH);
            digitalWrite(PIN_RGB_R, LOW);
            digitalWrite(PIN_RGB_B, LOW);
            break;

        case ALERT_UNSAFE:
            // Red LED + tone every 2 seconds
            digitalWrite(PIN_RGB_R, HIGH);
            digitalWrite(PIN_RGB_G, LOW);
            digitalWrite(PIN_RGB_B, LOW);

            if (now - lastOutputTime >= INTERVAL_UNSAFE) {
                lastOutputTime = now;
                playTone(1000, 200);
            }
            break;

        case ALERT_INSPECTION:
            // Flashing Blue LED + 2 chirps every 3 seconds
            if (now - lastOutputTime >= INTERVAL_INSPECTION) {
                lastOutputTime = now;
                flashToggle = !flashToggle;

                // Flash blue LED
                digitalWrite(PIN_RGB_B, flashToggle ? HIGH : LOW);
                digitalWrite(PIN_RGB_R, LOW);
                digitalWrite(PIN_RGB_G, LOW);

                // 2 chirps
                playChirp();
                delay(100);
                playChirp();
            }
            break;

        case ALERT_LOCKOUT:
            // Flashing Red/Blue LED + tone every 30 seconds
            // Flash alternates every 500ms for visibility
            if (now - lastOutputTime >= 500) {
                lastOutputTime = now;
                flashToggle = !flashToggle;

                digitalWrite(PIN_RGB_R, flashToggle ? HIGH : LOW);
                digitalWrite(PIN_RGB_B, flashToggle ? LOW : HIGH);
                digitalWrite(PIN_RGB_G, LOW);
            }

            // Sounder tone every 30 seconds (use a separate static tracker)
            {
                static unsigned long lastLockoutTone = 0;
                if (now - lastLockoutTone >= INTERVAL_LOCKOUT_ALERT) {
                    lastLockoutTone = now;
                    playTone(800, 500);
                }
            }
            break;
    }
}
