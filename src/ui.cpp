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

    // Individual long-hold state remains TRUE for the whole time the
    // button is held after the long-press threshold.
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

    // UP + DOWN held together = one mode-change event.
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

bool UIManager::upLongHeld() const
{
    return upLongState;
}

bool UIManager::downLongHeld() const
{
    return downLongState;
}

bool UIManager::modeLongPressed()
{
    return modeLongState;
}