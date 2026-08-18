#include <math.h>
#include "motor.h"

MotorController motor;

void MotorController::begin()
{
    pinMode(ENABLE_PIN, OUTPUT);
    disable();

    stepper.setMaxSpeed(MOTOR_MAX_SPEED);
    stepper.setAcceleration(MOTOR_ACCELERATION);
    stepper.setCurrentPosition(0);
}

void MotorController::update()
{
    if (speedMode)
    {
        // Continuous motion uses the same runSpeed path for manual hold
        // and automatic toggle-controlled movement.
        stepper.runSpeed();
        return;
    }

    stepper.run();

    // A manual fixed-step move must finish even after the button is released.
    if (running && stepper.distanceToGo() == 0)
    {
        digitalWrite(ENABLE_PIN, MOTOR_DISABLE);
        running = false;
    }
}

void MotorController::enable()
{
    digitalWrite(ENABLE_PIN, MOTOR_ENABLE);
}

void MotorController::disable()
{
    digitalWrite(ENABLE_PIN, MOTOR_DISABLE);
    running = false;
    speedMode = false;
}

void MotorController::runContinuous(int direction, float speed)
{
    if (direction == 0)
        return;

    enable();
    speedMode = true;
    running = true;

    float signedSpeed = fabsf(speed) * (direction > 0 ? 1.0f : -1.0f);
    stepper.setSpeed(signedSpeed);
}

void MotorController::manualStep(int direction)
{
    if (direction == 0)
        return;

    enable();
    speedMode = false;
    running = true;

    stepper.setMaxSpeed(MOTOR_NORMAL_SPEED);
    stepper.setAcceleration(MOTOR_ACCELERATION);
    stepper.move(direction > 0 ? MANUAL_STEP_STEPS : -MANUAL_STEP_STEPS);
}

void MotorController::manualHold(int direction)
{
    if (direction == 0)
        return;

    enable();
    speedMode = true;
    running = true;

    float signedSpeed = MANUAL_HOLD_SPEED * (direction > 0 ? 1.0f : -1.0f);
    stepper.setSpeed(signedSpeed);
}

void MotorController::stop()
{
    stepper.setSpeed(0.0f);
    stepper.stop();
    disable();
}

bool MotorController::isRunning() const
{
    return running;
}

long MotorController::getCurrentPosition() const
{
    // AccelStepper::currentPosition() is not const-qualified.
    return const_cast<AccelStepper &>(stepper).currentPosition();
}
