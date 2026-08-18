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
    // The toggle switch is the master machine control.
    // OFF always wins over every other command.
    if (!ui.startOn())
    {
        if (state == MachineState::RUNNING)
            stopTestMotion();

        if (ui.modeLongPressed())
        {
            resetCurrentForce();
            toggleMode();
        }
        else
        {
            updateManualControl();
        }

        refreshDisplay();
        return;
    }

    // Toggle ON: automatic test motion takes over according to the mode.
    if (state != MachineState::RUNNING)
        startTestMotion();

    updateVirtualForce();
    refreshDisplay();
}

void MachineController::updateManualControl()
{
    // A manual button after STOP starts a fresh test cycle.
    if (ui.upPressed() || ui.downPressed() || ui.upLongHeld() || ui.downLongHeld())
    {
        if (state == MachineState::STOP)
            resetCurrentForce();

        state = MachineState::STOP;
    }

    // Both buttons together are reserved for mode selection.
    if (ui.upHeld() && ui.downHeld())
    {
        motor.stop();
        return;
    }

    // Short press = one fixed step. After the long-press threshold,
    // continuous motion continues until the button is released.
    if (ui.upHeld())
    {
        if (ui.upPressed())
            motor.manualStep(+1);
        else if (ui.upLongHeld())
            motor.manualHold(+1);
        else
            motor.stop();

        return;
    }

    if (ui.downHeld())
    {
        if (ui.downPressed())
            motor.manualStep(-1);
        else if (ui.downLongHeld())
            motor.manualHold(-1);
        else
            motor.stop();

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
        mode = MachineMode::PUSH;
    else
        mode = MachineMode::TENSILE;
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