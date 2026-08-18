#ifndef LOADCELL_H
#define LOADCELL_H

#include <Arduino.h>
#include <HX711.h>

#include "pins.h"
#include "config.h"

class LoadCellManager
{
public:
    //------------------------------------------------
    // Initialization
    //------------------------------------------------

    void begin();

    void update();

    //------------------------------------------------
    // Calibration
    //------------------------------------------------

    void tare();

    void setCalibrationFactor(float factor);

    // Reads the current load (assumed to be exactly knownWeightKg,
    // e.g. CALIBRATION_WEIGHT_KG) and derives + applies the scale
    // factor from it. Call tare() BEFORE placing the weight so the
    // zero offset is already correct.
    void calibrateWithKnownWeight(float knownWeightKg);

    // Check if current readings have stabilized (last N values within tolerance)
    bool isReadingStable(float toleranceKg, uint8_t numChecks) const;

    //------------------------------------------------
    // Read Values
    //------------------------------------------------

    float getForce() const;

    long getRaw() const;

    bool isReady() const;

private:
    HX711 scale;

    float force = 0.0f;

    long rawValue = 0;

    bool ready = false;

    //------------------------------------------------
    // Moving average filter (kills HX711 jitter)
    //------------------------------------------------

    float filterBuffer[LOADCELL_FILTER_SAMPLES] = {0};

    uint8_t filterIndex = 0;

    bool filterFilled = false;
};

extern LoadCellManager loadCell;

#endif