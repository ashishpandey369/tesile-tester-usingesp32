#include "machine.h"
#include "ui.h"

MachineController machine;

//====================================================
// Speed profile for the pull (see rampToSpeed() in motor.cpp for the
// actual smoothing -- this just picks WHERE we want to be heading)
//====================================================

static float computeTargetSpeed(float progressPct)
{
    if (progressPct < 70.0f)
        return MOTOR_MAX_SPEED;

    if (progressPct < 90.0f)
    {
        // 70% -> 90%: glide from MAX down to NORMAL
        float t = (progressPct - 70.0f) / (90.0f - 70.0f);
        return MOTOR_MAX_SPEED - t * (MOTOR_MAX_SPEED - MOTOR_NORMAL_SPEED);
    }

    // 90% -> 100%: glide from NORMAL down to PRECISION
    float t = (progressPct - 90.0f) / (100.0f - 90.0f);

    if (t > 1.0f)
        t = 1.0f;

    return MOTOR_NORMAL_SPEED - t * (MOTOR_NORMAL_SPEED - MOTOR_PRECISION_SPEED);
}

//====================================================
// Initialization
//====================================================

void MachineController::begin()
{
    display.begin();

    loadCell.begin();

    motor.begin();

    buzzer.begin();

    buzzer.bootTone();

    currentState = MachineState::READY;

    display.showHomeScreen();

    display.setTargetForce(targetForce);

    buzzer.readyTone();
}

//====================================================
// Any Key (UP or DOWN edge press only -- see machine.h)
//====================================================

bool MachineController::anyKeyPressed()
{
    return ui.upPressed() || ui.downPressed();
}

//====================================================
// Main Update
//====================================================

void MachineController::update()
{
    loadCell.update();

    display.setCurrentForce(loadCell.getForce());

    //------------------------------------------------
    // READY: target adjust, start test, enter calibration
    //------------------------------------------------

    if (currentState == MachineState::READY)
    {
        if (ui.downLongPressed())
        {
            calStep = CalStep::WAIT_WEIGHT;

            setState(MachineState::CALIBRATION);
        }
        else
        {
            if (ui.upPressed())
            {
                targetForce += TARGET_STEP;

                if (targetForce > MAX_FORCE_KG)
                    targetForce = MAX_FORCE_KG;

                display.setTargetForce(targetForce);
            }

            if (ui.downPressed())
            {
                targetForce -= TARGET_STEP;

                if (targetForce < MIN_FORCE_KG)
                    targetForce = MIN_FORCE_KG;

                display.setTargetForce(targetForce);
            }

            if (ui.startPressed())
            {
                motor.moveForward();

                lastRunningForce = loadCell.getForce();

                setState(MachineState::RUNNING);
            }
        }
    }

    //------------------------------------------------
    // RUNNING: smooth variable speed, progress %, pass/fail detection
    //------------------------------------------------

    else if (currentState == MachineState::RUNNING)
    {
        float force = loadCell.getForce();

        float progressPct = 0.0f;

        if (targetForce > 0.0f)
            progressPct = (force / targetForce) * 100.0f;

        if (progressPct < 0.0f)
            progressPct = 0.0f;

        if (progressPct > 100.0f)
            progressPct = 100.0f;

        display.setProgress((uint8_t)progressPct);

        // Sudden drop = sample broke. Only treated as a failure once
        // there was meaningful load on it (ignores noise near zero at
        // the very start of the pull).
        bool suddenDrop = (lastRunningForce - force) > FAIL_DROP_KG &&
                          lastRunningForce > (targetForce * 0.2f);

        if (suddenDrop)
        {
            motor.stop();

            setState(MachineState::FAILED);
        }
        else if (force >= (targetForce - PASS_TOLERANCE_KG))
        {
            // Target reached -- stop INSTANTLY, no ramp-down, so there's
            // no overshoot past target.
            motor.stop();

            setState(MachineState::PASSED);
        }
        else
        {
            // Smoothly glide toward the profile's target speed. This
            // never jumps, no matter how the profile value changes
            // tick to tick -- that's what removes the glitching.
            float targetSpeed = computeTargetSpeed(progressPct);

            motor.rampToSpeed(targetSpeed);
        }

        lastRunningForce = force;
    }

    //------------------------------------------------
    // PASSED / FAILED: any key sends motor home
    //------------------------------------------------

    else if (currentState == MachineState::PASSED || currentState == MachineState::FAILED)
    {
        if (anyKeyPressed())
        {
            motor.returnHome();

            setState(MachineState::RETURNING);
        }
    }

    //------------------------------------------------
    // RETURNING: wait for the motor to reach home, then back to READY
    //------------------------------------------------

    else if (currentState == MachineState::RETURNING)
    {
        if (motor.isAtTarget())
        {
            setState(MachineState::READY);
        }
    }

    //------------------------------------------------
    // CALIBRATION: place weight -> wait for stable reading -> save ->
    // remove weight -> home. No timeout -- it just waits as long as it
    // takes for the reading to settle.
    //------------------------------------------------

    else if (currentState == MachineState::CALIBRATION)
    {
        if (calStep == CalStep::WAIT_WEIGHT)
        {
            if (anyKeyPressed())
            {
                calStep = CalStep::CHECKING_STABILITY;

                display.showCalibrationScreen("Waiting for stable reading...");
            }
        }
        else if (calStep == CalStep::CHECKING_STABILITY)
        {
            // Stable once the last 5 readings agree within 0.01kg
            if (loadCell.isReadingStable(0.01f, 5))
            {
                loadCell.calibrateWithKnownWeight(CALIBRATION_WEIGHT_KG);

                calStep = CalStep::WAIT_REMOVE;

                display.showCalibrationScreen("Saved " + String(CALIBRATION_WEIGHT_KG, 3) +
                                              " kg - Remove weight, press any key");
            }
        }
        else // WAIT_REMOVE
        {
            if (anyKeyPressed())
            {
                loadCell.tare();

                calStep = CalStep::WAIT_WEIGHT;

                setState(MachineState::READY);
            }
        }
    }

    display.update();

    motor.update();
}

//====================================================
// Set Machine State
//====================================================

void MachineController::setState(MachineState state)
{
    currentState = state;

    switch (currentState)
    {
    case MachineState::READY:
        display.showHomeScreen();
        display.setTargetForce(targetForce);
        buzzer.readyTone();
        break;

    case MachineState::RUNNING:
        display.showRunningScreen();
        buzzer.startTone();
        break;

    case MachineState::PASSED:
        display.showPassedScreen();
        buzzer.targetReachedTone();
        break;

    case MachineState::FAILED:
        display.showFailedScreen();
        buzzer.errorTone();
        break;

    case MachineState::RETURNING:
        display.showReturningScreen();
        break;

    case MachineState::CALIBRATION:
        display.showCalibrationScreen("Place " + String(CALIBRATION_WEIGHT_KG, 3) + " kg");
        break;

    case MachineState::ERROR:
        display.showErrorScreen("Machine Error");
        buzzer.errorTone();
        break;

    default:
        break;
    }
}

//====================================================
// Get Current State
//====================================================

MachineState MachineController::getState() const
{
    return currentState;
}

//====================================================
// Get Target Force
//====================================================

float MachineController::getTargetForce() const
{
    return targetForce;
}