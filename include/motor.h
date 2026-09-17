#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
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
    void runContinuous(int direction, float speedPercent);

    // Short manual movement. With a DC motor this is a timed pulse,
    // because there is no position feedback.
    void manualStep(int direction);

    // Continuous manual movement while a button is held.
    void manualHold(int direction);

    void stop();

    bool isRunning() const;
    int getDirection() const;

private:
    bool running = false;
    int direction = 0; // -1 = UP, +1 = DOWN, 0 = STOP
    uint32_t manualStopAt = 0;
    bool timedManualMove = false;

    void setPwm(int duty);
};

extern MotorController motor;

#endif
