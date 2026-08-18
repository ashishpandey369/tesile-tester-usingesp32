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
    Serial.println(" GoldTester v2.0.0 Starting...");
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
    // manual jog, automatic motion and virtual current-force value.
    machine.update();

    // Step pulses are generated after the machine decides the motion.
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

        Serial.print(" | Position: ");
        Serial.print(motor.getCurrentPosition());

        Serial.print(" | Motor: ");
        Serial.println(motor.isRunning() ? "RUNNING" : "STOP");
    }
}