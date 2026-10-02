#include "machine.h"
#include "ui.h"

MachineController machine;

void MachineController::begin()
{
    currentForce = INITIAL_CURRENT_FORCE;
    mode = MachineMode::TENSILE;
    state = MachineState::READY;
    modeChangeLock = false;
    resetPending = false;
    manualContinuousActive = false;
    forceLastUpdateMillis = millis();
    testStartMillis = millis();

    display.setCurrentForce(currentForce);
    display.setMode("PULL");
    display.setMotorStatus("STOP");
    display.setMachineStatus("READY");
}

void MachineController::update()
{
    // START toggle is the master automatic-motion control.
    // OFF: stop automatic motion immediately and allow manual UP/DOWN.
    if (ui.startTurnedOff())
    {
        motor.stop();
        manualContinuousActive = false;
        state = MachineState::READY;
        forceLastUpdateMillis = millis();
        Serial.println("[MACHINE] START OFF -> motor stopped");
        refreshDisplay();
        return;
    }

    // Handle RESET/MODE exactly as before. With START OFF it toggles
    // between PULL/TENSILE and PUSH. With START ON it resets the force.
    if (ui.resetModePressed())
    {
        if (currentForce > INITIAL_CURRENT_FORCE)
        {
            motor.stop();
            manualContinuousActive = false;
            resetCurrentForce();
            state = ui.startOn() ? MachineState::STOP : MachineState::READY;
            refreshDisplay();
            return;
        }

        if (!ui.startOn())
        {
            motor.stop();
            manualContinuousActive = false;
            toggleMode();
            state = MachineState::READY;
            refreshDisplay();
            return;
        }
    }

    // START OFF: manual control only.
    if (!ui.startOn())
    {
        updateManualControl();
        refreshDisplay();
        return;
    }

    // START ON: automatically run according to the selected mode.
    // PUSH = UP, PULL/TENSILE = DOWN.
    if (ui.startTurnedOn() || state != MachineState::RUNNING || !motor.isRunning())
        startTestMotion();

    updateVirtualForce();
    refreshDisplay();
}

void MachineController::updateManualControl()
{
    bool upPressed = ui.upPressed();
    bool downPressed = ui.downPressed();

    if (resetPending && (upPressed || downPressed))
    {
        resetCurrentForce();
        resetPending = false;
        state = MachineState::READY;
    }

    // Short press uses the configured 0.3 second manual step.
    // UP is -1 and DOWN is +1, matching the motor direction convention.
    if (upPressed)
    {
        manualContinuousActive = false;
        motor.manualStep(-1);
        state = MachineState::READY;
        return;
    }

    if (downPressed)
    {
        manualContinuousActive = false;
        motor.manualStep(+1);
        state = MachineState::READY;
        return;
    }

    // Long press runs continuously while the button remains held.
    if (ui.upLongHeld())
    {
        manualContinuousActive = true;
        state = MachineState::READY;
        motor.manualHold(-1);
        return;
    }

    if (ui.downLongHeld())
    {
        manualContinuousActive = true;
        state = MachineState::READY;
        motor.manualHold(+1);
        return;
    }

    if (manualContinuousActive && !ui.upHeld() && !ui.downHeld())
    {
        manualContinuousActive = false;
        motor.stop();
    }
}

void MachineController::startTestMotion()
{
    // PUSH         = motor UP.
    // PULL/TENSILE = motor DOWN.
    int direction = (mode == MachineMode::TENSILE) ? +1 : -1;

    motor.runContinuous(direction, MOTOR_NORMAL_SPEED_PERCENT);
    state = MachineState::RUNNING;
    forceLastUpdateMillis = millis();
    testStartMillis = millis();

    Serial.print("[MACHINE] START ON -> ");
    Serial.print(mode == MachineMode::TENSILE ? "PULL/DOWN" : "PUSH/UP");
    Serial.println(" at configured automatic power");
}

void MachineController::stopTestMotion()
{
    motor.stop();
    state = MachineState::STOP;
    forceLastUpdateMillis = millis();
}

void MachineController::resetCurrentForce()
{
    currentForce = INITIAL_CURRENT_FORCE;
    forceLastUpdateMillis = millis();
    testStartMillis = millis();
}

void MachineController::updateVirtualForce()
{
    if (!motor.isRunning() || state != MachineState::RUNNING)
    {
        forceLastUpdateMillis = millis();
        return;
    }

    uint32_t now = millis();
    uint32_t elapsedMs = now - forceLastUpdateMillis;
    if (elapsedMs == 0)
        return;

    // Virtual force ramp:
    // First 3 seconds: +500 g/s (0.5 kg/s)
    // Next 3 seconds:  +10 g/s (0.01 kg/s)
    // After 6 seconds: +1 g/s (0.001 kg/s)
    //
    // The ramp is based on elapsed automatic test time, not on motor speed.
    uint32_t testElapsedMs = now - testStartMillis;

    float increasePerMsKg;
    if (testElapsedMs < 3000UL)
    {
        increasePerMsKg = 0.0005f; // 500 g/s = 0.5 kg/s
    }
    else if (testElapsedMs < 6000UL)
    {
        increasePerMsKg = 0.00001f; // 10 g/s = 0.01 kg/s
    }
    else
    {
        increasePerMsKg = 0.000001f; // 1 g/s = 0.001 kg/s
    }

    currentForce += elapsedMs * increasePerMsKg;

    // Saturate the displayed virtual force without stopping the motor.
    if (currentForce >= MAX_VIRTUAL_FORCE_KG)
        currentForce = MAX_VIRTUAL_FORCE_KG;

    forceLastUpdateMillis = now;
}

void MachineController::toggleMode()
{
    if (mode == MachineMode::TENSILE)
        mode = MachineMode::PUSH;
    else
        mode = MachineMode::TENSILE;
}

void MachineController::refreshDisplay()
{
    display.setCurrentForce(currentForce);
    display.setMode(mode == MachineMode::TENSILE ? "PULL" : "PUSH");

    int motorDirection = motor.getDirection();

    // Direction convention for the automatic test:
    // +1 = PUSH/UP
    // -1 = PULL/DOWN
    if (motorDirection > 0)
        display.setMotorStatus("UP");
    else if (motorDirection < 0)
        display.setMotorStatus("DOWN");
    else
        display.setMotorStatus("STOP");

    if (state == MachineState::RUNNING)
        display.setMachineStatus("RUNNING");
    else if (state == MachineState::READY)
        display.setMachineStatus("READY");
    else
        display.setMachineStatus("STOP");
}

MachineMode MachineController::getMode() const
{
    return mode;
}

MachineState MachineController::getState() const
{
    return state;
}
