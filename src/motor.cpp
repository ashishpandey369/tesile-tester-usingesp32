#include "motor.h"

MotorController motor;

namespace
{
    constexpr uint32_t PWM_FREQUENCY_HZ = 20000;
    constexpr uint8_t PWM_RESOLUTION_BITS = 8;
    constexpr int PWM_MAX = 255;
}

void MotorController::begin()
{
    pinMode(BTS7960_R_EN_PIN, OUTPUT);
    pinMode(BTS7960_L_EN_PIN, OUTPUT);
    pinMode(BTS7960_RPWM_PIN, OUTPUT);
    pinMode(BTS7960_LPWM_PIN, OUTPUT);

    // ESP32 Arduino LEDC PWM. Both direction inputs use the same
    // PWM frequency/resolution.
    ledcAttach(BTS7960_RPWM_PIN, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
    ledcAttach(BTS7960_LPWM_PIN, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);

    disable();
}

void MotorController::update()
{
    if (timedManualMove && millis() >= manualStopAt)
    {
        timedManualMove = false;
        stop();
    }
}

void MotorController::enable()
{
    digitalWrite(BTS7960_R_EN_PIN, MOTOR_ENABLE);
    digitalWrite(BTS7960_L_EN_PIN, MOTOR_ENABLE);
}

void MotorController::disable()
{
    ledcWrite(BTS7960_RPWM_PIN, 0);
    ledcWrite(BTS7960_LPWM_PIN, 0);
    digitalWrite(BTS7960_R_EN_PIN, MOTOR_DISABLE);
    digitalWrite(BTS7960_L_EN_PIN, MOTOR_DISABLE);

    running = false;
    direction = 0;
    timedManualMove = false;
}

void MotorController::setPwm(int duty)
{
    duty = constrain(duty, 0, PWM_MAX);

    // Only one BTS7960 half-bridge is PWM driven at a time.
    if (direction > 0)
    {
        ledcWrite(BTS7960_RPWM_PIN, duty);
        ledcWrite(BTS7960_LPWM_PIN, 0);
    }
    else if (direction < 0)
    {
        ledcWrite(BTS7960_RPWM_PIN, 0);
        ledcWrite(BTS7960_LPWM_PIN, duty);
    }
    else
    {
        ledcWrite(BTS7960_RPWM_PIN, 0);
        ledcWrite(BTS7960_LPWM_PIN, 0);
    }
}

void MotorController::runContinuous(int requestedDirection, float speedPercent)
{
    if (requestedDirection == 0)
        return;

    enable();
    timedManualMove = false;
    running = true;
    direction = requestedDirection > 0 ? +1 : -1;

    int duty = static_cast<int>((constrain(speedPercent, 0.0f, 100.0f) / 100.0f) * PWM_MAX);
    setPwm(duty);
}

void MotorController::manualStep(int requestedDirection)
{
    if (requestedDirection == 0)
        return;

    enable();
    running = true;
    timedManualMove = true;
    direction = requestedDirection > 0 ? +1 : -1;

    int duty = static_cast<int>((constrain(MANUAL_STEP_SPEED_PERCENT, 0.0f, 100.0f) / 100.0f) * PWM_MAX);
    setPwm(duty);
    manualStopAt = millis() + MANUAL_STEP_TIME_MS;
}

void MotorController::manualHold(int requestedDirection)
{
    if (requestedDirection == 0)
        return;

    enable();
    timedManualMove = false;
    running = true;
    direction = requestedDirection > 0 ? +1 : -1;

    int duty = static_cast<int>((constrain(MANUAL_HOLD_SPEED_PERCENT, 0.0f, 100.0f) / 100.0f) * PWM_MAX);
    setPwm(duty);
}

void MotorController::stop()
{
    ledcWrite(BTS7960_RPWM_PIN, 0);
    ledcWrite(BTS7960_LPWM_PIN, 0);
    digitalWrite(BTS7960_R_EN_PIN, MOTOR_DISABLE);
    digitalWrite(BTS7960_L_EN_PIN, MOTOR_DISABLE);

    running = false;
    direction = 0;
    timedManualMove = false;
}

bool MotorController::isRunning() const
{
    return running;
}

int MotorController::getDirection() const
{
    if (!running)
        return 0;

    return direction;
}
