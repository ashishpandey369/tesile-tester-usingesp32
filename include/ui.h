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
    bool startTurnedOn() const;

    bool upHeld() const;
    bool downHeld() const;
    bool upLongHeld() const;
    bool downLongHeld() const;

    // While toggle is ON, a button press selects the operating mode.
    bool modeChangeRequested();
    int requestedModeDirection() const;

private:
    bool upState = false;
    bool downState = false;
    bool startState = false;
    bool previousStartState = false;

    bool upHoldState = false;
    bool downHoldState = false;
    bool upLongState = false;
    bool downLongState = false;

    bool lastUp = HIGH;
    bool lastDown = HIGH;
    bool lastStart = HIGH;

    unsigned long upHoldStart = 0;
    unsigned long downHoldStart = 0;

    // A short press is always an immediate manual movement event.
    bool manualUpEvent = false;
    bool manualDownEvent = false;

    // Button press while toggle is ON selects the mode.
    bool modeChangeState = false;
    int modeDirection = 0;
};

extern UIManager ui;

#endif