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

    // Continuous test motion. direction: +1 = DOWN, -1 = UP.
    void runContinuous(int direction, float speed);

    // One manual positioning step. direction: +1 = DOWN, -1 = UP.
    void manualStep(int direction);

    // Continuous manual movement while a button is held.
    void manualHold(int direction);

    void stop();

    bool isRunning() const;
    long getCurrentPosition() const;

    // Exact commanded motor direction/state for the display.
    int getDirection() const;

private:
    AccelStepper stepper = AccelStepper(
        AccelStepper::DRIVER,
        STEP_PIN,
        DIR_PIN);

    bool running = false;
    bool speedMode = false;
    int direction = 0; // -1 = UP, +1 = DOWN, 0 = STOP
};

extern MotorController motor;

#endif