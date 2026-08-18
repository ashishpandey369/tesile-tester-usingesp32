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
                modeChangeLock = true;
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

    if (!ui.startOn())
    {
        if (state == MachineState::RUNNING)
            stopTestMotion();

        if (modeChangeLock)
        {
            modeChangeLock = false;
            resetPending = true;
            manualContinuousActive = false;
            motor.stop();
            state = MachineState::STOP;
        }

        updateManualControl();
        refreshDisplay();
        return;
    }

    if (ui.modeChangeRequested())
    {
        motor.stop();
        manualContinuousActive = false;

        if (ui.requestedModeDirection() < 0)
            mode = MachineMode::TENSILE;
        else
            mode = MachineMode::PUSH;

        modeChangeLock = true;
        resetPending = false;
        state = MachineState::STOP;
        refreshDisplay();
        return;
    }

    if (modeChangeLock)
    {
        motor.stop();
        state = MachineState::STOP;
        refreshDisplay();
        return;
    }

    if (state != MachineState::RUNNING)
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

    motor.runContinuous(direction, MANUAL_HOLD_SPEED);
    lastRunPosition = motor.getCurrentPosition();
    state = MachineState::RUNNING;
}

void MachineController::stopTestMotion()
{
    motor.stop();
    state = MachineState::STOP;
}

void MachineController::resetCurrentForce()
{
    currentForce = INITIAL_CURRENT_FORCE;
    lastRunPosition = motor.getCurrentPosition();
}

void MachineController::updateVirtualForce()
{
    long position = motor.getCurrentPosition();
    long deltaSteps = labs(position - lastRunPosition);

    if (deltaSteps > 0)
    {
        currentForce += deltaSteps * FORCE_PER_MOTOR_STEP_KG;

        if (currentForce > MAX_VIRTUAL_FORCE_KG)
            currentForce = MAX_VIRTUAL_FORCE_KG;

        lastRunPosition = position;
    }
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

    // Always read the actual motor controller state.
    // +1 = UP, -1 = DOWN, 0 = STOP.
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
