#ifndef UI_H
#define UI_H

#include <Arduino.h>

#include "pins.h"

class UIManager
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
    // Inputs
    //------------------------------------------------

    bool upPressed();

    bool downPressed();

    bool startPressed();

    // True exactly once, when DOWN has been held continuously for
    // BUTTON_LONG_PRESS_MS. Independent of downPressed() (which still
    // fires on the initial press edge).
    bool downLongPressed();

private:
    bool upState = false;
    bool downState = false;
    bool startState = false;

    bool lastUp = HIGH;
    bool lastDown = HIGH;
    bool lastStart = HIGH;

    //------------------------------------------------
    // Long press tracking (DOWN button)
    //------------------------------------------------

    unsigned long downHoldStart = 0;
    bool downLongConsumed = false;
    bool downLongState = false;
};

extern UIManager ui;

#endif