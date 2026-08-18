#include "ui.h"

UIManager ui;

void UIManager::begin()
{
    pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
    pinMode(START_SWITCH_PIN, INPUT_PULLUP);
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

    // UP + DOWN held together = mode change.
    if (currentUp == LOW && currentDown == LOW)
    {
        if (lastUp == HIGH || lastDown == HIGH)
        {
            modeHoldStart = millis();
            modeLongConsumed = false;
        }

        if (!modeLongConsumed &&
            (millis() - modeHoldStart >= BUTTON_LONG_PRESS_MS))
        {
            modeLongState = true;
            modeLongConsumed = true;
        }
        else
        {
            modeLongState = false;
        }
    }
    else
    {
        modeLongState = false;
        modeLongConsumed = false;
    }

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

bool UIManager::upHeld() const
{
    return upHoldState;
}

bool UIManager::downHeld() const
{
    return downHoldState;
}

bool UIManager::modeLongPressed()
{
    return modeLongState;
}