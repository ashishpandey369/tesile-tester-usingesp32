#include "ui.h"

UIManager ui;

void UIManager::begin()
{
    pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
    pinMode(RESET_MODE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(START_SWITCH_PIN, INPUT_PULLUP);

    lastUp = digitalRead(BUTTON_UP_PIN);
    lastDown = digitalRead(BUTTON_DOWN_PIN);
    lastResetMode = digitalRead(RESET_MODE_BUTTON_PIN);
    lastStart = digitalRead(START_SWITCH_PIN);
    previousStartState = (lastStart == LOW);
}

void UIManager::update()
{
    bool currentUp = digitalRead(BUTTON_UP_PIN);
    bool currentDown = digitalRead(BUTTON_DOWN_PIN);
    bool currentResetMode = digitalRead(RESET_MODE_BUTTON_PIN);
    bool currentStart = digitalRead(START_SWITCH_PIN);

    upState = false;
    downState = false;
    resetModeState = false;
    manualUpEvent = false;
    manualDownEvent = false;
    modeChangeState = false;

    bool newUpPress = (lastUp == HIGH && currentUp == LOW);
    bool newDownPress = (lastDown == HIGH && currentDown == LOW);
    bool newResetModePress = (lastResetMode == HIGH && currentResetMode == LOW);

    upHoldState = (currentUp == LOW);
    downHoldState = (currentDown == LOW);
    startState = (currentStart == LOW);

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

    // Toggle ON changes button meaning: UP selects TENSILE,
    // DOWN selects PUSH. Toggle OFF keeps buttons for manual motion.
    if (newUpPress)
    {
        if (startState)
        {
            modeChangeState = true;
            modeDirection = -1;
        }
        else
        {
            manualUpEvent = true;
        }
    }

    if (newDownPress)
    {
        if (startState)
        {
            modeChangeState = true;
            modeDirection = +1;
        }
        else
        {
            manualDownEvent = true;
        }
    }

    if (newResetModePress)
        resetModeState = true;

    previousStartState = startState;
    lastUp = currentUp;
    lastDown = currentDown;
    lastResetMode = currentResetMode;
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

bool UIManager::resetModePressed()
{
    bool event = resetModeState;
    resetModeState = false;
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
