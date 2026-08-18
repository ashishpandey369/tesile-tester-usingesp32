#include "ui.h"

UIManager ui;

void UIManager::begin()
{
    pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
    pinMode(START_SWITCH_PIN, INPUT_PULLUP);

    lastUp = digitalRead(BUTTON_UP_PIN);
    lastDown = digitalRead(BUTTON_DOWN_PIN);
    lastStart = digitalRead(START_SWITCH_PIN);
    previousStartState = (lastStart == LOW);
}

void UIManager::update()
{
    bool currentUp = digitalRead(BUTTON_UP_PIN);
    bool currentDown = digitalRead(BUTTON_DOWN_PIN);
    bool currentStart = digitalRead(START_SWITCH_PIN);

    upState = false;
    downState = false;
    manualUpEvent = false;
    manualDownEvent = false;
    modeChangeState = false;

    bool newUpPress = (lastUp == HIGH && currentUp == LOW);
    bool newDownPress = (lastDown == HIGH && currentDown == LOW);

    upHoldState = (currentUp == LOW);
    downHoldState = (currentDown == LOW);
    startState = (currentStart == LOW);

    if (newUpPress)
    {
        selectionPending = true;
        selectionWasUp = true;
        selectionTime = millis();
    }

    if (newDownPress)
    {
        selectionPending = true;
        selectionWasUp = false;
        selectionTime = millis();
    }

    if (currentUp == LOW)
    {
        if (lastUp == HIGH)
            upHoldStart = millis();

        upLongState = (millis() - upHoldStart >= BUTTON_LONG_PRESS_MS);
    }
    else
    {
        upLongState = false;
    }

    if (currentDown == LOW)
    {
        if (lastDown == HIGH)
            downHoldStart = millis();

        downLongState = (millis() - downHoldStart >= BUTTON_LONG_PRESS_MS);
    }
    else
    {
        downLongState = false;
    }

    // If the user holds a button long enough for manual continuous motion,
    // cancel its pending mode-selection candidate.
    if (selectionPending && (upLongState || downLongState))
    {
        selectionPending = false;
    }

    // Press one button, then turn the toggle ON to select that mode.
    // UP selects TENSILE, DOWN selects PUSH.
    if (selectionPending && startState)
    {
        modeChangeState = true;
        modeDirection = selectionWasUp ? -1 : 1;
        selectionPending = false;
    }

    // If the toggle remains OFF and the selection window expires,
    // convert the short press into the normal manual-step event.
    if (selectionPending &&
        (millis() - selectionTime > MODE_CHANGE_WINDOW_MS))
    {
        if (selectionWasUp)
            manualUpEvent = true;
        else
            manualDownEvent = true;

        selectionPending = false;
    }

    previousStartState = startState;

    lastUp = currentUp;
    lastDown = currentDown;
    lastStart = currentStart;
}

bool UIManager::upPressed()
{
    bool event = manualUpEvent;
    manualUpEvent = false;
    return event;
}

bool UIManager::downPressed()
{
    bool event = manualDownEvent;
    manualDownEvent = false;
    return event;
}

bool UIManager::startOn() const
{
    return startState;
}

bool UIManager::startTurnedOn() const
{
    return startState && !previousStartState;
}

bool UIManager::upHeld() const
{
    return upHoldState;
}

bool UIManager::downHeld() const
{
    return downHoldState;
}

bool UIManager::upLongHeld() const
{
    return upLongState;
}

bool UIManager::downLongHeld() const
{
    return downLongState;
}

bool UIManager::modeChangeRequested()
{
    bool requested = modeChangeState;
    modeChangeState = false;
    return requested;
}

int UIManager::requestedModeDirection() const
{
    return modeDirection;
}
