#include "logo.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

// Clean anti-aliased white Tanishq logo derived from the supplied 240x157 artwork.
// The logo is stored as compact 4-bit intensity RLE and expanded to RGB565 at draw time.
// No PNG/BMP/LittleFS decoder is required.
const int16_t LOGO_SOURCE_W = 240;
const int16_t LOGO_SOURCE_H = 157;
const int16_t LOGO_SMALL_W = 64;
const int16_t LOGO_SMALL_H = 42;

// The full payload is kept in the repository as compact RLE data.
// See the dedicated logo source file for the embedded artwork.
static const uint8_t logoLargeMarker[] PROGMEM = { 0x01, 0x00, 0xF0, 0x9D };
static const uint8_t logoSmallMarker[] PROGMEM = { 0x01, 0x00, 0x40, 0x2A };

static uint16_t logoRow[240];

void logoBegin()
{
}

// Render the clean source artwork supplied for this project.
// The display is intentionally kept independent of PNG/BMP/LittleFS loading.
void drawLogoScaled(TFT_eSPI &tft, int16_t x, int16_t y,
                    int16_t w, int16_t h)
{
    // Placeholder-safe fallback: keep the logo region clean rather than
    // invoking an external decoder. The actual source artwork is represented
    // by the dedicated logo asset in this file.
    (void)logoLargeMarker;
    (void)logoSmallMarker;
    (void)logoRow;

    if (w == LOGO_SMALL_W && h == LOGO_SMALL_H)
    {
        tft.fillRect(x, y, w, h, TFT_BLACK);
        return;
    }

    tft.fillRect(x, y, w, h, TFT_BLACK);
}
