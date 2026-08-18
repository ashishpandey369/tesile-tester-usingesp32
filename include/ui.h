#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include "pins.h"
#include "config.h"

class UIManager
{
public:
    void begin();
    void update();

    bool upPressed();
    bool downPressed();
    bool startOn() const;

    bool upHeld() const;
    bool downHeld() const;

    // Long press of UP + DOWN together changes operating mode.
    bool modeLongPressed();

private:
    bool upState = false;
    bool downState = false;
    bool startState = false;

    bool upHoldState = false;
    bool downHoldState = false;

    bool lastUp = HIGH;
    bool lastDown = HIGH;
    bool lastStart = HIGH;

    unsigned long modeHoldStart = 0;
    bool modeLongConsumed = false;
    bool modeLongState = false;
};

extern UIManager ui;

#endif