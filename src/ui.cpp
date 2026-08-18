#include "ui.h"
#include "config.h"

UIManager ui;

//====================================================
// Initialization
//====================================================

void UIManager::begin()
{
    pinMode(BUTTON_UP_PIN, INPUT_PULLUP);

    pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);

    pinMode(START_SWITCH_PIN, INPUT_PULLUP);
}


//====================================================
// Update
//====================================================

void UIManager::update()
{
    bool currentUp = digitalRead(BUTTON_UP_PIN);
    bool currentDown = digitalRead(BUTTON_DOWN_PIN);
    bool currentStart = digitalRead(START_SWITCH_PIN);

    upState = (lastUp == HIGH && currentUp == LOW);

    downState = (lastDown == HIGH && currentDown == LOW);

    startState = (currentStart == LOW);

    //------------------------------------------------
    // DOWN long-press tracking
    //------------------------------------------------

    if (currentDown == LOW)
    {
        // Just pressed -> start the hold timer
        if (lastDown == HIGH)
        {
            downHoldStart = millis();
            downLongConsumed = false;
        }

        if (!downLongConsumed && (millis() - downHoldStart >= BUTTON_LONG_PRESS_MS))
        {
            downLongState = true;
            downLongConsumed = true; // fire once per hold
        }
        else
        {
            downLongState = false;
        }
    }
    else
    {
        downLongState = false;
    }

    lastUp = currentUp;
    lastDown = currentDown;
    lastStart = currentStart;
}

//====================================================
// UP Button
//====================================================

bool UIManager::upPressed()
{
    return upState;
}

//====================================================
// DOWN Button
//====================================================

bool UIManager::downPressed()
{
    return downState;
}

//====================================================
// START Switch
//====================================================

bool UIManager::startPressed()
{
    return startState;
}

//====================================================
// DOWN Long Press
//====================================================

bool UIManager::downLongPressed()
{
    return downLongState;
}