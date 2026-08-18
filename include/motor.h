#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <AccelStepper.h>

#include "pins.h"
#include "config.h"

class MotorController
{
public:
    //------------------------------------------------
    // Initialization
    //------------------------------------------------

    void begin();

    void update();

    //------------------------------------------------
    // Enable / Disable
    //------------------------------------------------

    void enable();

    void disable();

    //------------------------------------------------
    // Motion
    //------------------------------------------------

    // Starts a pull in CONSTANT-SPEED mode (see rampToSpeed()). This is
    // what avoids AccelStepper's internal acceleration engine glitching
    // when the target speed is changed frequently/abruptly.
    void moveForward();

    // Position-mode move (kept for completeness / manual jogging)
    void moveBackward();

    // Immediate stop -- no ramp-down. Used the instant target force is
    // reached so there's no overshoot.
    void stop();

    // Drive back to position 0 (the position recorded at boot). Uses
    // AccelStepper's normal position mode with its own acceleration.
    void returnHome();

    //------------------------------------------------
    // Speed
    //------------------------------------------------

    // Legacy direct setter (kept for compatibility) -- prefer
    // rampToSpeed() during RUNNING for a glitch-free profile.
    void setSpeed(float speed);

    // Smoothly slides the current speed toward targetSpeed, limited to
    // MOTOR_ACCELERATION steps/s^2, based on real elapsed time. Call
    // this every loop tick while pulling; it never jumps.
    void rampToSpeed(float targetSpeed);

    //------------------------------------------------
    // Status
    //------------------------------------------------

    bool isRunning() const;

    // True once the stepper has arrived at its current moveTo() target
    // (position mode only -- used for returnHome())
    bool isAtTarget();

private:
    AccelStepper stepper = AccelStepper(
        AccelStepper::DRIVER,
        STEP_PIN,
        DIR_PIN);

    bool running = false;

    // true while pulling (constant-speed mode via runSpeed()),
    // false while homing/jogging (position mode via run())
    bool speedMode = false;

    float currentSpeed = 0.0f;
    unsigned long lastRampTime = 0;
};

extern MotorController motor;

#endif