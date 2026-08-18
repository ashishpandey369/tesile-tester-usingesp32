#include "logo.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

const int16_t LOGO_SOURCE_W = 398;
const int16_t LOGO_SOURCE_H = 260;

void logoBegin() {
    // Logo is rendered directly by TFT_eSPI; no filesystem is required.
}

void drawLogoScaled(TFT_eSPI &tft, int16_t x, int16_t y,
                    int16_t w, int16_t h) {
    if (w <= 0 || h <= 0) return;

    // Compile-safe embedded logo mark. This avoids the broken generated
    // string payload that previously caused the missing-quote build error.
    tft.fillRect(x, y, w, h, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("TENSILE", x + w / 2, y + h / 2 - 7, 4);
    tft.drawCentreString("TESTER", x + w / 2, y + h / 2 + 13, 2);
}
