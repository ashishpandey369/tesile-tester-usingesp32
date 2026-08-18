#include <math.h>

#include "loadcell.h"

LoadCellManager loadCell;

//====================================================
// Initialization
//====================================================

void LoadCellManager::begin()
{
    scale.begin(HX711_DT_PIN, HX711_SCK_PIN);

    if (scale.is_ready())
    {
        ready = true;

        scale.set_scale(LOADCELL_CALIBRATION_FACTOR);

        scale.tare();
    }
    else
    {
        ready = false;
    }
}

//====================================================
// Update Reading
//====================================================
//
// Every tick we take ONE HX711 reading (fast, non-blocking-ish) and
// push it into a small ring buffer. The reported force is the AVERAGE
// of the last LOADCELL_FILTER_SAMPLES readings, which smooths out the
// raw jitter (0.025 / 0.035 / 0.045 wandering) you were seeing.
//
// Anything that averages out to within ZERO_DEADBAND_KG of zero is
// snapped to exactly 0.000 so an empty platform always reads a clean
// zero instead of drifting.
//====================================================

void LoadCellManager::update()
{
    if (!ready)
        return;

    if (scale.is_ready())
    {
        rawValue = scale.read();

        float reading = scale.get_units(1);

        filterBuffer[filterIndex] = reading;

        filterIndex++;

        if (filterIndex >= LOADCELL_FILTER_SAMPLES)
        {
            filterIndex = 0;
            filterFilled = true;
        }

        uint8_t count = filterFilled ? LOADCELL_FILTER_SAMPLES : filterIndex;

        if (count == 0)
            count = 1;

        float sum = 0.0f;

        for (uint8_t i = 0; i < count; i++)
        {
            sum += filterBuffer[i];
        }

        float average = sum / count;

        if (fabsf(average) < ZERO_DEADBAND_KG)
        {
            average = 0.0f;
        }

        force = average;
    }
}

//====================================================
// Tare
//====================================================

void LoadCellManager::tare()
{
    if (!ready)
        return;

    scale.tare();

    // Clear the filter so old (pre-tare) samples don't linger and
    // pull the average away from the fresh zero.
    for (uint8_t i = 0; i < LOADCELL_FILTER_SAMPLES; i++)
    {
        filterBuffer[i] = 0.0f;
    }

    filterIndex = 0;
    filterFilled = false;
    force = 0.0f;
}

//====================================================
// Calibration
//====================================================

void LoadCellManager::setCalibrationFactor(float factor)
{
    scale.set_scale(factor);
}

void LoadCellManager::calibrateWithKnownWeight(float knownWeightKg)
{
    if (!ready || knownWeightKg <= 0.0f)
        return;

    // Average several offset-adjusted raw readings under load for a
    // stable calibration point.
    long value = scale.get_value(10);

    float newScale = (float)value / knownWeightKg;

    scale.set_scale(newScale);

    // Reset the display filter so the newly-scaled reading takes
    // effect immediately instead of blending with pre-calibration values.
    for (uint8_t i = 0; i < LOADCELL_FILTER_SAMPLES; i++)
    {
        filterBuffer[i] = knownWeightKg;
    }

    filterIndex = 0;
    filterFilled = true;
    force = knownWeightKg;
}

//====================================================
// Stability Check
//====================================================

bool LoadCellManager::isReadingStable(float toleranceKg, uint8_t numChecks) const
{
    if (numChecks > LOADCELL_FILTER_SAMPLES || numChecks < 2)
        return false;

    // Find min and max of the last numChecks readings
    float minVal = filterBuffer[0];
    float maxVal = filterBuffer[0];

    uint8_t count = filterFilled ? LOADCELL_FILTER_SAMPLES : filterIndex;

    // Use the last numChecks readings (or fewer if not enough samples yet)
    uint8_t startIdx = (count >= numChecks) ? (count - numChecks) : 0;

    for (uint8_t i = startIdx; i < count; i++)
    {
        float val = filterBuffer[i];
        if (val < minVal)
            minVal = val;
        if (val > maxVal)
            maxVal = val;
    }

    // Stable if the spread is within tolerance
    return (maxVal - minVal) <= toleranceKg;
}

//====================================================
// Get Force
//====================================================

float LoadCellManager::getForce() const
{
    return force;
}

//====================================================
// Get Raw ADC
//====================================================

long LoadCellManager::getRaw() const
{
    return rawValue;
}

//====================================================
// Ready Status
//====================================================

bool LoadCellManager::isReady() const
{
    return ready;
}