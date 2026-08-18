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
    display.setMode("TENSILE");
    display.setMotorStatus("STOP");
    display.setMachineStatus("READY");
}

void MachineController::update()
{
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

    // Toggle ON + button selects the mode. It does not start motion.
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

    // Stay stopped until the user turns the toggle OFF.
    if (modeChangeLock)
    {
        motor.stop();
        state = MachineState::STOP;
        refreshDisplay();
        return;
    }

    // Toggle ON starts the selected test mode.
    if (state != MachineState::RUNNING)
        startTestMotion();

    updateVirtualForce();
    refreshDisplay();
}

void MachineController::updateManualControl()
{
    // Each one-shot event must be read exactly once.
    bool upPressed = ui.upPressed();
    bool downPressed = ui.downPressed();

    // After mode selection and switch OFF, the next button press resets
    // the virtual force to 00.000 and also performs its normal manual step.
    if (resetPending && (upPressed || downPressed))
    {
        resetCurrentForce();
        resetPending = false;
        state = MachineState::READY;
    }

    // Immediate 150-step manual movement.
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

    // Hold for 1 second, then smooth continuous movement until release.
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
    // TENSILE/PULL = anticlockwise (-1)
    // PUSH         = clockwise    (+1)
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
    display.setMode(mode == MachineMode::TENSILE ? "TENSILE" : "PUSH");

    if (modeChangeLock)
    {
        display.setMachineStatus("TURN OFF");
        display.setMotorStatus("STOP");
    }
    else if (resetPending)
    {
        display.setMachineStatus("STOP");
        display.setMotorStatus("PRESS BUTTON");
    }
    else if (state == MachineState::RUNNING)
    {
        display.setMachineStatus("RUNNING");
        display.setMotorStatus(mode == MachineMode::TENSILE ? "ANTICLOCKWISE" : "CLOCKWISE");
    }
    else if (state == MachineState::READY)
    {
        display.setMachineStatus("READY");

        if (motor.isRunning())
            display.setMotorStatus("MANUAL");
        else
            display.setMotorStatus("STOP");
    }
    else
    {
        display.setMachineStatus("STOP");
        display.setMotorStatus("STOP");
    }
}

MachineMode MachineController::getMode() const
{
    return mode;
}

MachineState MachineController::getState() const
{
    return state;
}
