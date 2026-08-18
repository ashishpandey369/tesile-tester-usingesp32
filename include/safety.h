#ifndef SAFETY_H
#define SAFETY_H

#include <Arduino.h>

#include "display.h"
#include "loadcell.h"
#include "motor.h"
#include "buzzer.h"
#include "config.h"

class SafetyManager
{
public:
    //------------------------------------------------
    // Initialization
    //------------------------------------------------

    void begin();

    //------------------------------------------------
    // Update
    //------------------------------------------------

    void update();

    //------------------------------------------------
    // Safety Checks
    //------------------------------------------------

    bool isSafe() const;

    void emergencyStop();

private:
    bool safe = true;
};

extern SafetyManager safety;

#endif