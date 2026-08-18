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

    // Manual movement events. A short press is released to the machine
    // only after the mode-selection window expires without a toggle ON.
    bool upPressed();
    bool downPressed();

    bool startOn() const;
    bool startTurnedOn() const;

    bool upHeld() const;
    bool downHeld() const;
    bool upLongHeld() const;
    bool downLongHeld() const;

    // Mode selection: press UP or DOWN, then turn the toggle ON.
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

    // A button press becomes a mode-selection candidate. If the toggle
    // is not turned ON during the window, it becomes a normal manual step.
    bool selectionPending = false;
    bool selectionWasUp = false;
    unsigned long selectionTime = 0;

    bool manualUpEvent = false;
    bool manualDownEvent = false;

    bool modeChangeState = false;
    int modeDirection = 0;
};

extern UIManager ui;

#endif