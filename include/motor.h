#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <AccelStepper.h>

#include "pins.h"
#include "config.h"

class MotorController
{
public:
    void begin();
    void update();

    void enable();
    void disable();

    // Continuous test motion. direction: +1 forward, -1 backward.
    void runContinuous(int direction, float speed);

    // One manual positioning step. direction: +1 or -1.
    void manualStep(int direction);

    // Continuous manual movement while a button is held.
    void manualHold(int direction);

    void stop();

    bool isRunning() const;
    long getCurrentPosition() const;

private:
    AccelStepper stepper = AccelStepper(
        AccelStepper::DRIVER,
        STEP_PIN,
        DIR_PIN);

    bool running = false;
    bool speedMode = false;
};

extern MotorController motor;

#endif