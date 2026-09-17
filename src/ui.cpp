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
    startState = (lastStart == LOW);
    previousStartState = startState;

    Serial.println("[UI] Inputs initialized (INPUT_PULLUP)");
    Serial.print("[UI] UP=");
    Serial.print(lastUp);
    Serial.print(" DOWN=");
    Serial.print(lastDown);
    Serial.print(" RESET/MODE=");
    Serial.print(lastResetMode);
    Serial.print(" START=");
    Serial.println(lastStart);
    Serial.print("[START] Initial state: ");
    Serial.println(startState ? "ON" : "OFF");
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

    // Capture the previous START state before updating the current state.
    bool oldStartState = startState;
    startState = (currentStart == LOW);
    previousStartState = oldStartState;

    // Report every START transition immediately.
    if (startState != oldStartState)
    {
        if (startState)
            Serial.println("[START] SWITCH ON -> automatic motion ENABLED");
        else
            Serial.println("[START] SWITCH OFF -> automatic motion DISABLED");
    }

    upHoldState = (currentUp == LOW);
    downHoldState = (currentDown == LOW);

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

    // UP/DOWN are manual motor controls only when START is OFF.
    // START ON is reserved for automatic motion in the selected mode.
    if (newUpPress && !startState)
    {
        manualUpEvent = true;
        Serial.println("[UI] UP press -> manual +1 step");
    }

    if (newDownPress && !startState)
    {
        manualDownEvent = true;
        Serial.println("[UI] DOWN press -> manual -1 step");
    }

    if (newResetModePress)
    {
        resetModeState = true;
        Serial.println("[UI] RESET/MODE press detected");
    }

    lastUp = currentUp;
    lastDown = currentDown;
    lastResetMode = currentResetMode;
    lastStart = currentStart;

    static uint32_t lastDebug = 0;
    if (millis() - lastDebug >= 1000)
    {
        lastDebug = millis();
        Serial.print("[UI RAW] UP=");
        Serial.print(currentUp);
        Serial.print(" DOWN=");
        Serial.print(currentDown);
        Serial.print(" RESET=");
        Serial.print(currentResetMode);
        Serial.print(" START=");
        Serial.print(currentStart);
        Serial.print(" | START_STATE=");
        Serial.println(startState ? "ON" : "OFF");
    }
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

bool UIManager::startTurnedOff() const
{
    return !startState && previousStartState;
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
