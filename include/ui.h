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
    bool resetModePressed();

    bool startOn() const;
    bool startTurnedOn() const;
    bool startTurnedOff() const;

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
    bool resetModeState = false;
    bool startState = false;
    bool previousStartState = false;

    bool upHoldState = false;
    bool downHoldState = false;
    bool upLongState = false;
    bool downLongState = false;

    // Debounced/stable input states.
    bool lastUp = HIGH;
    bool lastDown = HIGH;
    bool lastResetMode = HIGH;
    bool lastStart = HIGH;

    // Last raw samples and the time each raw input changed.
    bool rawUp = HIGH;
    bool rawDown = HIGH;
    bool rawResetMode = HIGH;
    bool rawStart = HIGH;

    uint32_t upRawChangedAt = 0;
    uint32_t downRawChangedAt = 0;
    uint32_t resetRawChangedAt = 0;
    uint32_t startRawChangedAt = 0;

    unsigned long upHoldStart = 0;
    unsigned long downHoldStart = 0;

    bool manualUpEvent = false;
    bool manualDownEvent = false;

    bool modeChangeState = false;
    int modeDirection = 0;
};

extern UIManager ui;

#endif
