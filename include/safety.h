#ifndef SAFETY_H
#define SAFETY_H

#include <Arduino.h>
#include "motor.h"
#include "config.h"

class SafetyManager
{
public:
    void begin();
    void update();

    bool isSafe() const;
    void emergencyStop();

private:
    bool safe = true;
};

extern SafetyManager safety;

#endif