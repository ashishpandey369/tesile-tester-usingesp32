#include "machine.h"
#include "ui.h"

MachineController machine;

void MachineController::begin()
{
    currentForce = INITIAL_CURRENT_FORCE;
    mode = MachineMode::TENSILE;
    state = MachineState::READY;
    modeChangeLock = false;

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

        // Turning the switch OFF after a mode change releases the lock
        // and resets the test value for the next manual positioning cycle.
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

    // While the switch is ON, holding either button during switch-on
    // selects the next mode. The motor must remain stopped until the
    // user turns the switch OFF again.
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
    // Any manual button after STOP starts a fresh test cycle.
    if (ui.upPressed() || ui.downPressed() || ui.upLongHeld() || ui.downLongHeld())
    {
        if (state == MachineState::STOP)
            resetCurrentForce();

        if (state != MachineState::READY && currentForce != INITIAL_CURRENT_FORCE)
            resetCurrentForce();

        state = MachineState::READY;
    }

    // A short press commands one complete fixed step. Do not stop it on
    // the following loop; AccelStepper must be allowed to finish the move.
    if (ui.upPressed())
    {
        motor.manualStep(+1);
        return;
    }

    if (ui.downPressed())
    {
        motor.manualStep(-1);
        return;
    }

    // Long hold begins continuous movement after the short-press window.
    if (ui.upLongHeld())
    {
        motor.manualHold(+1);
        return;
    }

    if (ui.downLongHeld())
    {
        motor.manualHold(-1);
        return;
    }

    // Release stops continuous manual movement. A completed short step
    // is allowed to finish in motor.update().
    if (!ui.upHeld() && !ui.downHeld())
    {
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
