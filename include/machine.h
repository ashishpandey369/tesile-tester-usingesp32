#ifndef MACHINE_H
#define MACHINE_H

#include <Arduino.h>

#include "display.h"
#include "motor.h"
#include "buzzer.h"
#include "config.h"

enum class MachineMode
{
    TENSILE,
    PUSH
};

enum class MachineState
{
    READY,
    RUNNING,
    STOP
};

class MachineController
{
public:
    void begin();
    void update();

    MachineMode getMode() const;
    MachineState getState() const;

private:
    MachineMode mode = MachineMode::TENSILE;
    MachineState state = MachineState::READY;

    float currentForce = INITIAL_CURRENT_FORCE;
    long lastRunPosition = 0;

    bool modeChangeLock = false;
    bool manualContinuousActive = false;

    void updateManualControl();
    void startTestMotion();
    void stopTestMotion();
    void resetCurrentForce();
    void updateVirtualForce();
    void toggleMode();
    void refreshDisplay();
};

extern MachineController machine;

#endif