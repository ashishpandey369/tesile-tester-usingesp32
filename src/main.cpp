#include <Arduino.h>

#include "config.h"
#include "pins.h"
#include "display.h"
#include "motor.h"
#include "buzzer.h"
#include "safety.h"
#include "ui.h"
#include "machine.h"

void setup()
{
    Serial.begin(SERIAL_BAUDRATE);

    Serial.println();
    Serial.println("====================================");
    Serial.println(" GoldTester v" PROJECT_VERSION " Starting...");
    Serial.println(" Motion-Controlled Tensile / Push Tester");
    Serial.println("====================================");

    display.begin();
    buzzer.begin();
    motor.begin();
    safety.begin();
    ui.begin();
    machine.begin();

    buzzer.bootTone();
    buzzer.readyTone();

    Serial.println("Initialization Complete");
    Serial.println("System Ready");
}

void loop()
{
    ui.update();

    // Safety layer remains available for emergency-stop calls.
    safety.update();

    // Machine logic handles the toggle switch, mode selection,
    // manual jog, automatic motion and time-based virtual force.
    machine.update();

    // Update the BTS7960 motor controller and timed manual moves.
    motor.update();

    display.update();

    static unsigned long lastPrint = 0;

    if (millis() - lastPrint >= 500)
    {
        lastPrint = millis();

        Serial.print("Mode: ");
        Serial.print(machine.getMode() == MachineMode::TENSILE ? "TENSILE" : "PUSH");

        Serial.print(" | State: ");
        if (machine.getState() == MachineState::READY)
            Serial.print("READY");
        else if (machine.getState() == MachineState::RUNNING)
            Serial.print("RUNNING");
        else
            Serial.print("STOP");

        Serial.print(" | Motor: ");
        if (!motor.isRunning())
        {
            Serial.println("STOP");
        }
        else
        {
            Serial.print("RUNNING (");
            Serial.print(motor.getDirection() > 0 ? "FORWARD" : "REVERSE");
            Serial.println(")");
        }
    }
}