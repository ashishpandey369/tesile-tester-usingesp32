#include "safety.h"

SafetyManager safety;

//====================================================
// Initialization
//====================================================

void SafetyManager::begin()
{
    safe = true;
}

//====================================================
// Update
//====================================================

void SafetyManager::update()
{
    //------------------------------------------------
    // HX711 Check
    //------------------------------------------------

    if (!loadCell.isReady())
    {
        safe = false;

        emergencyStop();

        return;
    }

    //------------------------------------------------
    // Maximum Force Check
    //------------------------------------------------

    if (loadCell.getForce() >= MAX_FORCE_KG)
    {
        safe = false;

        emergencyStop();

        return;
    }

    safe = true;
}

//====================================================
// Safety Status
//====================================================

bool SafetyManager::isSafe() const
{
    return safe;
}

//====================================================
// Emergency Stop
//====================================================

void SafetyManager::emergencyStop()
{
    motor.stop();

    motor.disable();

    display.showErrorScreen("EMERGENCY STOP");

    buzzer.emergencyTone();
}