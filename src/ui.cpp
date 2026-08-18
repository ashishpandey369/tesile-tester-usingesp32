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

    upState = (lastUp == HIGH && currentUp == LOW);
    downState = (lastDown == HIGH && currentDown == LOW);

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

    // Mode selection: press one button, then the opposite button quickly.
    if (upState && !downState)
    {
        if (sequenceWaiting && !firstSequenceWasUp &&
            (millis() - firstSequenceTime <= MODE_CHANGE_WINDOW_MS))
        {
            modeChangeState = true;
            sequenceWaiting = false;
        }
        else
        {
            sequenceWaiting = true;
            firstSequenceWasUp = true;
            firstSequenceTime = millis();
        }
    }
    else if (downState && !upState)
    {
        if (sequenceWaiting && firstSequenceWasUp &&
            (millis() - firstSequenceTime <= MODE_CHANGE_WINDOW_MS))
        {
            modeChangeState = true;
            sequenceWaiting = false;
        }
        else
        {
            sequenceWaiting = true;
            firstSequenceWasUp = false;
            firstSequenceTime = millis();
        }
    }

    if (sequenceWaiting &&
        (millis() - firstSequenceTime > MODE_CHANGE_WINDOW_MS))
    {
        sequenceWaiting = false;
    }

    previousStartState = startState;

    lastUp = currentUp;
    lastDown = currentDown;
    lastStart = currentStart;
}

bool UIManager::upPressed()
{
    return upState;
}

bool UIManager::downPressed()
{
    return downState;
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
