#include "machine.h"
#include "ui.h"

MachineController machine;

void MachineController::begin()
{
    currentForce = INITIAL_CURRENT_FORCE;
    mode = MachineMode::TENSILE;
    state = MachineState::READY;

    display.setCurrentForce(currentForce);
    display.setMode("TENSILE");
    display.setMotorStatus("STOP");
    display.setMachineStatus("READY");
}

void MachineController::update()
{
    // Toggle switch is the master machine control.
    if (ui.startOn())
    {
        if (state != MachineState::RUNNING)
            startTestMotion();

        updateVirtualForce();
        refreshDisplay();
        return;
    }

    // OFF is an immediate stop. The current test value remains visible
    // until the next manual button interaction resets it to zero.
    if (state == MachineState::RUNNING)
        stopTestMotion();

    if (ui.modeLongPressed())
    {
        resetCurrentForce();
        toggleMode();
        motor.stop();
        state = MachineState::STOP;
        refreshDisplay();
        return;
    }

    updateManualControl();
    refreshDisplay();
}

void MachineController::updateManualControl()
{
    // Any manual button interaction after a test stop starts a fresh
    // software test cycle.
    if (ui.upPressed() || ui.downPressed() || ui.upHeld() || ui.downHeld())
    {
        if (state == MachineState::STOP)
            resetCurrentForce();

        state = MachineState::STOP;
    }

    // Do not jog while both buttons are being held for mode selection.
    if (ui.upHeld() && ui.downHeld())
    {
        motor.stop();
        return;
    }

    if (ui.upHeld())
    {
        if (ui.upPressed())
            motor.manualStep(+1);
        else
            motor.manualHold(+1);

        return;
    }

    if (ui.downHeld())
    {
        if (ui.downPressed())
            motor.manualStep(-1);
        else
            motor.manualHold(-1);

        return;
    }

    motor.stop();
}

void MachineController::startTestMotion()
{
    int direction = (mode == MachineMode::TENSILE) ? +1 : -1;

    motor.runContinuous(direction, MOTOR_NORMAL_SPEED);

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
    {
        mode = MachineMode::PUSH;
        display.setMode("PUSH");
    }
    else
    {
        mode = MachineMode::TENSILE;
        display.setMode("TENSILE");
    }
}

void MachineController::refreshDisplay()
{
    display.setCurrentForce(currentForce);
    display.setMode(mode == MachineMode::TENSILE ? "TENSILE" : "PUSH");

    if (state == MachineState::RUNNING)
    {
        display.setMachineStatus("RUNNING");
        display.setMotorStatus(mode == MachineMode::TENSILE ? "FORWARD" : "BACKWARD");
    }
    else if (state == MachineState::READY)
    {
        display.setMachineStatus("READY");
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