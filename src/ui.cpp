#include "ui.h"

UIManager ui;

void UIManager::begin()
{
    pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
    pinMode(RESET_MODE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(START_SWITCH_PIN, INPUT_PULLUP);

    const uint32_t now = millis();

    lastUp = digitalRead(BUTTON_UP_PIN);
    lastDown = digitalRead(BUTTON_DOWN_PIN);
    lastResetMode = digitalRead(RESET_MODE_BUTTON_PIN);
    lastStart = digitalRead(START_SWITCH_PIN);

    rawUp = lastUp;
    rawDown = lastDown;
    rawResetMode = lastResetMode;
    rawStart = lastStart;

    upRawChangedAt = now;
    downRawChangedAt = now;
    resetRawChangedAt = now;
    startRawChangedAt = now;

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
    const uint32_t now = millis();

    const bool sampledUp = digitalRead(BUTTON_UP_PIN);
    const bool sampledDown = digitalRead(BUTTON_DOWN_PIN);
    const bool sampledResetMode = digitalRead(RESET_MODE_BUTTON_PIN);
    const bool sampledStart = digitalRead(START_SWITCH_PIN);

    // Track raw changes first. A raw state is accepted only after it
    // remains unchanged for BUTTON_DEBOUNCE_MS.
    if (sampledUp != rawUp)
    {
        rawUp = sampledUp;
        upRawChangedAt = now;
    }

    if (sampledDown != rawDown)
    {
        rawDown = sampledDown;
        downRawChangedAt = now;
    }

    if (sampledResetMode != rawResetMode)
    {
        rawResetMode = sampledResetMode;
        resetRawChangedAt = now;
    }

    if (sampledStart != rawStart)
    {
        rawStart = sampledStart;
        startRawChangedAt = now;
    }

    bool newUpPress = false;
    bool newDownPress = false;
    bool newResetModePress = false;

    if (rawUp != lastUp && (now - upRawChangedAt) >= BUTTON_DEBOUNCE_MS)
    {
        const bool oldUp = lastUp;
        lastUp = rawUp;
        newUpPress = (oldUp == HIGH && lastUp == LOW);
    }

    if (rawDown != lastDown && (now - downRawChangedAt) >= BUTTON_DEBOUNCE_MS)
    {
        const bool oldDown = lastDown;
        lastDown = rawDown;
        newDownPress = (oldDown == HIGH && lastDown == LOW);
    }

    if (rawResetMode != lastResetMode && (now - resetRawChangedAt) >= BUTTON_DEBOUNCE_MS)
    {
        const bool oldResetMode = lastResetMode;
        lastResetMode = rawResetMode;
        newResetModePress = (oldResetMode == HIGH && lastResetMode == LOW);
    }

    if (rawStart != lastStart && (now - startRawChangedAt) >= BUTTON_DEBOUNCE_MS)
    {
        lastStart = rawStart;
    }

    const bool currentUp = lastUp;
    const bool currentDown = lastDown;
    const bool currentResetMode = lastResetMode;
    const bool currentStart = lastStart;

    upState = false;
    downState = false;
    resetModeState = false;
    manualUpEvent = false;
    manualDownEvent = false;
    modeChangeState = false;

    // Capture the previous START state before updating the current state.
    const bool oldStartState = startState;
    startState = (currentStart == LOW);
    previousStartState = oldStartState;

    // Report every debounced START transition immediately.
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
        if (lastUp == LOW && upHoldStart == 0)
            upHoldStart = now;

        upLongState = (now - upHoldStart >= BUTTON_LONG_PRESS_MS);
    }
    else
    {
        upLongState = false;
        upHoldStart = 0;
    }

    if (currentDown == LOW)
    {
        if (lastDown == LOW && downHoldStart == 0)
            downHoldStart = now;

        downLongState = (now - downHoldStart >= BUTTON_LONG_PRESS_MS);
    }
    else
    {
        downLongState = false;
        downHoldStart = 0;
    }

    // UP/DOWN are manual motor controls only when START is OFF.
    // START ON is reserved for automatic motion in the selected mode.
    if (newUpPress && !startState)
    {
        manualUpEvent = true;
        Serial.println("[UI] UP press -> manual step");
    }

    if (newDownPress && !startState)
    {
        manualDownEvent = true;
        Serial.println("[UI] DOWN press -> manual step");
    }

    if (newResetModePress)
    {
        resetModeState = true;
        Serial.println("[UI] RESET/MODE press detected");
    }

    static uint32_t lastDebug = 0;
    if (now - lastDebug >= 1000)
    {
        lastDebug = now;
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
