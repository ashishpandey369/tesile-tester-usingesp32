#include "motor.h"

MotorController motor;

//====================================================
// Initialization
//====================================================

void MotorController::begin()
{
    pinMode(ENABLE_PIN, OUTPUT);

    disable();

    stepper.setMaxSpeed(MOTOR_MAX_SPEED);
    stepper.setAcceleration(MOTOR_ACCELERATION);

    // Position 0 here IS "home" for the whole session -- returnHome()
    // always drives back to this point.
    stepper.setCurrentPosition(0);
}

//====================================================
// Update
//====================================================
//
// speedMode == true  (pulling)  -> runSpeed(): steps at exactly
//                                  whatever rampToSpeed() last set,
//                                  no internal accel engine involved.
// speedMode == false (homing)   -> run(): normal AccelStepper position
//                                  mode with its own acceleration.
//====================================================

void MotorController::update()
{
    if (speedMode)
        stepper.runSpeed();
    else
        stepper.run();
}

//====================================================
// Enable Driver
//====================================================

void MotorController::enable()
{
    digitalWrite(ENABLE_PIN, MOTOR_ENABLE);
}

//====================================================
// Disable Driver
//====================================================

void MotorController::disable()
{
    digitalWrite(ENABLE_PIN, MOTOR_DISABLE);

    running = false;
}

//====================================================
// Move Forward (constant-speed mode)
//====================================================
//
// Starts from a dead stop at 0 speed. MachineController calls
// rampToSpeed() every tick during RUNNING, which glides the speed up
// to top speed and later back down -- never jumps. A short settle
// delay after enabling gives the driver a moment before the first step
// pulses, which avoids a jerky first step on some drivers.
//====================================================

void MotorController::moveForward()
{
    enable();

    delay(5); // let the driver's ENABLE line settle before stepping

    running = true;
    speedMode = true;

    currentSpeed = 0.0f;
    lastRampTime = millis();

    stepper.setSpeed(0.0f);
}

//====================================================
// Move Backward (position mode -- manual jog / legacy use)
//====================================================

void MotorController::moveBackward()
{
    enable();

    running = true;
    speedMode = false;

    stepper.setMaxSpeed(MOTOR_NORMAL_SPEED);
    stepper.setAcceleration(MOTOR_ACCELERATION);

    stepper.move(-1000000);
}

//====================================================
// Return Home (position mode, absolute move back to position 0)
//====================================================

void MotorController::returnHome()
{
    enable();

    running = true;
    speedMode = false;

    stepper.setMaxSpeed(MOTOR_RETURN_SPEED);
    stepper.setAcceleration(MOTOR_ACCELERATION);

    stepper.moveTo(0);
}

//====================================================
// Stop -- IMMEDIATE, no ramp-down (used the instant target is reached)
//====================================================

void MotorController::stop()
{
    currentSpeed = 0.0f;

    stepper.setSpeed(0.0f);
    stepper.stop(); // also clears any pending position-mode move

    running = false;
}

//====================================================
// Legacy direct speed setter (kept for compatibility)
//====================================================

void MotorController::setSpeed(float speed)
{
    stepper.setMaxSpeed(speed);
}

//====================================================
// Smooth Ramp -- the actual fix for the glitching
//====================================================
//
// Slides currentSpeed toward targetSpeed at a MAXIMUM rate of
// MOTOR_ACCELERATION steps/s^2, using real elapsed time (dt) rather
// than a fixed per-tick step. This means it behaves the same whether
// the loop is running fast or briefly slower, and it NEVER jumps --
// however big the gap between the old and new target speed is, it
// always gets there gradually.
//====================================================

void MotorController::rampToSpeed(float targetSpeed)
{
    unsigned long now = millis();

    float dt = (now - lastRampTime) / 1000.0f;

    lastRampTime = now;

    // Guard against an oversized dt on the very first call after
    // moveForward(), or if the loop briefly stalled for some reason.
    if (dt <= 0.0f || dt > 0.25f)
        dt = 0.01f;

    float maxDelta = MOTOR_ACCELERATION * dt;

    if (currentSpeed < targetSpeed)
    {
        currentSpeed += maxDelta;

        if (currentSpeed > targetSpeed)
            currentSpeed = targetSpeed;
    }
    else if (currentSpeed > targetSpeed)
    {
        currentSpeed -= maxDelta;

        if (currentSpeed < targetSpeed)
            currentSpeed = targetSpeed;
    }

    stepper.setSpeed(currentSpeed);
}

//====================================================
// Running Status
//====================================================

bool MotorController::isRunning() const
{
    return running;
}

//====================================================
// At Target
//====================================================

bool MotorController::isAtTarget()
{
    return stepper.distanceToGo() == 0;
}