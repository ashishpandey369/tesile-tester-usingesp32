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

    display.setCurrentForce(currentForce);
    display.setMode("PULL");
    display.setMotorStatus("STOP");
    display.setMachineStatus("READY");
}

void MachineController::update()
{
    if (ui.resetModePressed())
    {
        if (currentForce > INITIAL_CURRENT_FORCE)
        {
            motor.stop();
            manualContinuousActive = false;
            resetCurrentForce();

            if (ui.startOn())
            {
                state = MachineState::STOP;
            }
            else
            {
                resetPending = false;
                state = MachineState::READY;
            }

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

    // START is the master ON/OFF control. When OFF, automatic motion
    // is stopped immediately. When ON, the automatic motor is kept
    // running continuously unless a safety/reset action stops it.
    if (!ui.startOn())
    {
        if (motor.isRunning() && state == MachineState::RUNNING)
            stopTestMotion();

        modeChangeLock = false;
        resetPending = false;
        updateManualControl();
        refreshDisplay();
        return;
    }

    // UP/DOWN while START is ON select the requested test mode.
    // Changing mode no longer requires cycling the START toggle.
    if (ui.modeChangeRequested())
    {
        if (ui.requestedModeDirection() < 0)
            mode = MachineMode::TENSILE;
        else
            mode = MachineMode::PUSH;

        motor.stop();
        manualContinuousActive = false;
        state = MachineState::READY;
        forceLastUpdateMillis = millis();
    }

    // START ON always ensures automatic motion is active.
    if (state != MachineState::RUNNING || !motor.isRunning())
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

    if (upPressed)
    {
        manualContinuousActive = false;
        motor.manualStep(+1);
        state = MachineState::READY;
        return;
    }

    if (downPressed)
    {
        manualContinuousActive = false;
        motor.manualStep(-1);
        state = MachineState::READY;
        return;
    }

    if (ui.upLongHeld())
    {
        manualContinuousActive = true;
        state = MachineState::READY;
        motor.manualHold(+1);
        return;
    }

    if (ui.downLongHeld())
    {
        manualContinuousActive = true;
        state = MachineState::READY;
        motor.manualHold(-1);
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
    // TENSILE/PULL = anticlockwise (-1) and displayed as UP.
    // PUSH         = clockwise    (+1) and displayed as DOWN.
    int direction = (mode == MachineMode::TENSILE) ? -1 : +1;

    motor.runContinuous(direction, MOTOR_NORMAL_SPEED_PERCENT);
    state = MachineState::RUNNING;
    forceLastUpdateMillis = millis();
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

    // 12 g is added for every elapsed millisecond while the automatic
    // test motor is running. This remains stable even if the main loop
    // does not execute exactly once every millisecond.
    currentForce += elapsedMs * FORCE_INCREASE_PER_MS_KG;

    // The virtual force display saturates at the configured maximum,
    // but reaching that value does not stop the motor. The START toggle
    // remains the master control for continuous automatic motion.
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
