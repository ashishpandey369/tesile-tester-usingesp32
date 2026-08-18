#include <Arduino.h>
#include "pins.h"
#include "buzzer.h"

BuzzerManager buzzer;

void BuzzerManager::begin()
{
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
}

void BuzzerManager::toneDelay(uint16_t freq, uint16_t duration, uint16_t pause)
{
    tone(BUZZER_PIN, freq, duration);
    delay(duration + pause);
    noTone(BUZZER_PIN);
}

void BuzzerManager::beep(uint16_t freq, uint16_t duration)
{
    toneDelay(freq, duration);
}

//
// POWER ON
// ♪ _ __ ___
//

void BuzzerManager::bootTone()
{
    toneDelay(1200, 120);
    toneDelay(1800, 120);
    toneDelay(2600, 180);
}

//
// READY
//

void BuzzerManager::readyTone()
{
    toneDelay(2200, 100);
}

//
// START TEST
//

void BuzzerManager::startTone()
{
    toneDelay(1800, 80);
    toneDelay(2200, 80);
}

//
// TARGET REACHED
//

void BuzzerManager::targetReachedTone()
{
    toneDelay(2800, 500);
}

//
// COMPLETE
//

void BuzzerManager::completedTone()
{
    toneDelay(1800, 100);
    toneDelay(2200, 100);
    toneDelay(2800, 200);
}

//
// WARNING
//

void BuzzerManager::warningTone()
{
    toneDelay(1200, 200, 250);
    toneDelay(1200, 200);
}

//
// ERROR
//

void BuzzerManager::errorTone()
{
    for (int i = 0; i < 4; i++)
    {
        toneDelay(900, 120, 60);
    }
}

//
// EMERGENCY
//

void BuzzerManager::emergencyTone()
{
    for (int i = 0; i < 10; i++)
    {
        toneDelay(1000, 80, 20);
    }
}