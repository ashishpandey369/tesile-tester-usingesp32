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
    bool upLongHeld() const;
    bool downLongHeld() const;

    // Long press of UP + DOWN together changes operating mode.
    bool modeLongPressed();

private:
    bool upState = false;
    bool downState = false;
    bool startState = false;

    bool upHoldState = false;
    bool downHoldState = false;
    bool upLongState = false;
    bool downLongState = false;

    bool lastUp = HIGH;
    bool lastDown = HIGH;
    bool lastStart = HIGH;

    unsigned long upHoldStart = 0;
    unsigned long downHoldStart = 0;
    unsigned long modeHoldStart = 0;

    bool upLongConsumed = false;
    bool downLongConsumed = false;
    bool modeLongConsumed = false;
    bool modeLongState = false;
};

extern UIManager ui;

#endif