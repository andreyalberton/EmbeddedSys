#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include <Arduino.h>
#include "Constants.h"

class AlertManager {
private:
    void playTone(int frequency, int durationMs);

public:
    AlertManager();
    void begin();
    void setNormalState();
    void triggerUnsafeAlarm();
};

#endif