#include "machine.h"
#include "ui.h"

MachineController machine;

void MachineController::begin()
{
    currentForce = INITIAL_CURRENT_FORCE;
    mode = MachineMode::TENSILE;
    state = MachineState::READY;
    modeChangeLock = false;
    manualContinuousActive = false;

    display.setCurrentForce(currentForce);
    display.setMode("TENSILE");
    display.setMotorStatus("STOP");
    display.setMachineStatus("READY");
}

void MachineController::update()
{
    // Toggle OFF is the master stop. It always wins over movement.
    if (!ui.startOn())
    {
        if (state == MachineState::RUNNING)
            stopTestMotion();

        if (modeChangeLock)
        {
            modeChangeLock = false;
            resetCurrentForce();
            state = MachineState::READY;
        }
        else
        {
            updateManualControl();
        }

        refreshDisplay();
        return;
    }

    // Hold either button while turning the toggle ON to change mode.
    // The motor remains stopped until the switch is turned OFF again.
    if (ui.modeChangeRequested())
    {
        motor.stop();
        toggleMode();
        modeChangeLock = true;
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

    // Toggle ON: automatic test motion takes over according to the mode.
    if (state != MachineState::RUNNING)
        startTestMotion();

    updateVirtualForce();
    refreshDisplay();
}

void MachineController::updateManualControl()
{
    if (ui.upPressed() || ui.downPressed() || ui.upLongHeld() || ui.downLongHeld())
    {
        // A button interaction after STOP starts a fresh test cycle.
        if (state == MachineState::STOP)
            resetCurrentForce();

        state = MachineState::READY;
    }

    // Short press = one complete fixed step.
    // The move is allowed to finish by motor.update() even after release.
    if (ui.upPressed())
    {
        manualContinuousActive = false;
        motor.manualStep(+1);
        return;
    }

    if (ui.downPressed())
    {
        manualContinuousActive = false;
        motor.manualStep(-1);
        return;
    }

    // Long hold = continuous movement until release.
    if (ui.upLongHeld())
    {
        manualContinuousActive = true;
        motor.manualHold(+1);
        return;
    }

    if (ui.downLongHeld())
    {
        manualContinuousActive = true;
        motor.manualHold(-1);
        return;
    }

    // Only stop here when a continuous manual movement was active.
    // A short step must not be cancelled by the next loop iteration.
    if (manualContinuousActive && !ui.upHeld() && !ui.downHeld())
    {
        manualContinuousActive = false;
        motor.stop();
    }
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

    if (modeChangeLock)
    {
        display.setMachineStatus("TURN OFF");
        display.setMotorStatus("STOP");
    }
    else if (state == MachineState::RUNNING)
    {
        display.setMachineStatus("RUNNING");
        display.setMotorStatus(mode == MachineMode::TENSILE ? "FORWARD" : "BACKWARD");
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
