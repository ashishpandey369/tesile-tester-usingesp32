#ifndef MACHINE_H
#define MACHINE_H

#include <Arduino.h>

#include "display.h"
#include "loadcell.h"
#include "motor.h"
#include "buzzer.h"
#include "config.h"

enum class MachineState
{
    BOOT,
    READY,
    RUNNING,
    PASSED,
    FAILED,
    RETURNING,
    CALIBRATION,
    ERROR
};

class MachineController
{
public:
    //------------------------------------------------
    // Initialization
    //------------------------------------------------

    void begin();

    //------------------------------------------------
    // Main Update
    //------------------------------------------------

    void update();

    //------------------------------------------------
    // State Control
    //------------------------------------------------

    void setState(MachineState state);

    MachineState getState() const;

    float getTargetForce() const;

private:
    MachineState currentState = MachineState::BOOT;

    float targetForce = DEFAULT_TARGET_FORCE;

    // Force reading from the previous RUNNING tick -- used to detect a
    // sudden drop (sample break) for TEST FAILED.
    float lastRunningForce = 0.0f;

    //------------------------------------------------
    // Calibration sub-flow
    //------------------------------------------------

    enum class CalStep
    {
        WAIT_WEIGHT,        // "Place 0.510kg" -> waiting for any key to confirm
        CHECKING_STABILITY, // waiting for the reading to settle before saving
        WAIT_REMOVE         // "Saved" -> waiting for weight removal + any key
    };

    CalStep calStep = CalStep::WAIT_WEIGHT;

    // UP or DOWN edge press, used as the generic "any key" confirm for
    // calibration steps and for leaving PASSED/FAILED to go home. The
    // START switch is deliberately excluded here since it's level-
    // detected (true continuously while ON) and would fire repeatedly.
    bool anyKeyPressed();
};

extern MachineController machine;

#endif