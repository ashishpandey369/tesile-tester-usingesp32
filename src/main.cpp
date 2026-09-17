#include <Arduino.h>

#include "config.h"
#include "pins.h"
#include "display.h"
#include "motor.h"
#include "buzzer.h"
#include "safety.h"
#include "ui.h"
#include "machine.h"

namespace
{
    void handleMotorDiagnostic()
    {
        if (!Serial.available())
            return;

        char command = static_cast<char>(Serial.read());

        // Diagnostic controls are intentionally available only with the
        // START switch OFF so they cannot override an automatic test.
        if (ui.startOn())
        {
            if (command == 's' || command == 'S')
                motor.stop();
            return;
        }

        switch (command)
        {
        case 'f':
        case 'F':
            motor.runContinuous(+1, 30.0f);
            Serial.println("[MOTOR DIAG] FORWARD 30%");
            break;

        case 'r':
        case 'R':
            motor.runContinuous(-1, 30.0f);
            Serial.println("[MOTOR DIAG] REVERSE 30%");
            break;

        case '1':
            motor.runContinuous(+1, 20.0f);
            Serial.println("[MOTOR DIAG] FORWARD 20%");
            break;

        case '2':
            motor.runContinuous(+1, 40.0f);
            Serial.println("[MOTOR DIAG] FORWARD 40%");
            break;

        case '3':
            motor.runContinuous(+1, 60.0f);
            Serial.println("[MOTOR DIAG] FORWARD 60%");
            break;

        case '4':
            motor.runContinuous(-1, 20.0f);
            Serial.println("[MOTOR DIAG] REVERSE 20%");
            break;

        case '5':
            motor.runContinuous(-1, 40.0f);
            Serial.println("[MOTOR DIAG] REVERSE 40%");
            break;

        case '6':
            motor.runContinuous(-1, 60.0f);
            Serial.println("[MOTOR DIAG] REVERSE 60%");
            break;

        case 's':
        case 'S':
            motor.stop();
            Serial.println("[MOTOR DIAG] STOP");
            break;

        case 'h':
        case 'H':
            Serial.println("[MOTOR DIAG] f=+30%, r=-30%, 1/2/3=+20/40/60%, 4/5/6=-20/40/60%, s=stop");
            break;

        default:
            break;
        }
    }
}

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
    Serial.println("BTS7960 diagnostic: H = help, F/R = +/-30%, 1-3 = +20/40/60%, 4-6 = -20/40/60%, S = stop");
}

void loop()
{
    ui.update();

    // Safety layer remains available for emergency-stop calls.
    safety.update();

    // Machine logic handles the toggle switch, mode selection,
    // manual jog, automatic motion and time-based virtual force.
    machine.update();

    // Optional serial diagnostic control for the BTS7960/DC motor.
    // START must be OFF before diagnostic motion is accepted.
    handleMotorDiagnostic();

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