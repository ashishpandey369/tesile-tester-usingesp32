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

    // Mode changes when opposite buttons are pressed in quick succession.
    bool modeChangeRequested();

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

    bool modeChangeState = false;
    bool sequenceWaiting = false;
    bool firstSequenceWasUp = false;
    unsigned long firstSequenceTime = 0;
};

extern UIManager ui;

#endif