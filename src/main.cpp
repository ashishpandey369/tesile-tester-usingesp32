#include <Arduino.h>

#include "config.h"
#include "pins.h"

#include "display.h"
#include "loadcell.h"
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
    Serial.println(" GoldTester Starting...");
    Serial.println("====================================");

    display.begin();

    buzzer.begin();
    buzzer.bootTone();

    loadCell.begin();

    motor.begin();

    safety.begin();

    ui.begin();

    machine.begin();

    Serial.println("Initialization Complete");
    Serial.println("System Ready");

    buzzer.readyTone();
}

void loop()
{
    //------------------------------------------------
    // Update Modules
    //------------------------------------------------

    ui.update();

    loadCell.update();

    motor.update();

    safety.update();

    machine.update();

    //------------------------------------------------
    // Update Display
    //------------------------------------------------

    display.setCurrentForce(loadCell.getForce());

    display.update();

    //------------------------------------------------
    // Debug
    //------------------------------------------------

    static unsigned long lastPrint = 0;

    if (millis() - lastPrint >= 500)
    {
        lastPrint = millis();

        Serial.print("Force : ");
        Serial.print(loadCell.getForce(), 3);
        Serial.print(" kg");

        Serial.print("   Raw : ");
        Serial.print(loadCell.getRaw());

        Serial.print("   Motor : ");
        Serial.print(motor.isRunning());

        Serial.print("   Safe : ");
        Serial.println(safety.isSafe());
    }
}