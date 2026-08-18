#include "safety.h"

SafetyManager safety;

void SafetyManager::begin()
{
    safe = true;
}

void SafetyManager::update()
{
    // The START/STOP toggle is handled as the master machine control
    // by MachineController. This module remains available for a direct
    // emergency stop call and does not depend on a load cell.
    safe = true;
}

bool SafetyManager::isSafe() const
{
    return safe;
}

void SafetyManager::emergencyStop()
{
    motor.stop();
    motor.disable();
    safe = false;
}
