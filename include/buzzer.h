#pragma once

class BuzzerManager
{
public:
    void begin();

    // Basic
    void beep(uint16_t freq = 2500, uint16_t duration = 100);

    // Machine Events
    void bootTone();
    void readyTone();
    void startTone();
    void targetReachedTone();
    void completedTone();
    void warningTone();
    void errorTone();
    void emergencyTone();

private:
    void toneDelay(uint16_t freq, uint16_t duration, uint16_t pause = 40);
};

extern BuzzerManager buzzer;