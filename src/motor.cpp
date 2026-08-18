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
        stepper.runSpeed();
        return;
    }

    stepper.run();

    if (running && stepper.distanceToGo() == 0)
    {
        digitalWrite(ENABLE_PIN, MOTOR_DISABLE);
        running = false;
        direction = 0;
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
    direction = 0;
}

void MotorController::runContinuous(int requestedDirection, float speed)
{
    if (requestedDirection == 0)
        return;

    enable();
    speedMode = true;
    running = true;

    // Machine direction convention:
    // TENSILE uses requestedDirection -1 and is displayed as UP.
    // PUSH uses requestedDirection +1 and is displayed as DOWN.
    direction = requestedDirection < 0 ? +1 : -1;

    float signedSpeed = fabsf(speed) * (requestedDirection > 0 ? 1.0f : -1.0f);
    stepper.setSpeed(signedSpeed);
}

void MotorController::manualStep(int requestedDirection)
{
    if (requestedDirection == 0)
        return;

    enable();
    speedMode = false;
    running = true;

    // Manual UI convention:
    // +1 = UP, -1 = DOWN.
    direction = requestedDirection > 0 ? +1 : -1;

    stepper.setMaxSpeed(MOTOR_NORMAL_SPEED);
    stepper.setAcceleration(MOTOR_ACCELERATION);
    stepper.move(requestedDirection > 0 ? MANUAL_STEP_STEPS : -MANUAL_STEP_STEPS);
}

void MotorController::manualHold(int requestedDirection)
{
    if (requestedDirection == 0)
        return;

    enable();
    speedMode = true;
    running = true;

    // Manual UI convention: +1 = UP, -1 = DOWN.
    direction = requestedDirection > 0 ? +1 : -1;

    float signedSpeed = MANUAL_HOLD_SPEED * (requestedDirection > 0 ? 1.0f : -1.0f);
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
    return const_cast<AccelStepper &>(stepper).currentPosition();
}

int MotorController::getDirection() const
{
    if (!running)
        return 0;

    return direction;
}
