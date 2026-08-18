#ifndef TESILE_TESTER_LOGO_H
#define TESILE_TESTER_LOGO_H

#include <Arduino.h>
#include <TFT_eSPI.h>

extern const int16_t LOGO_SOURCE_W;
extern const int16_t LOGO_SOURCE_H;

void logoBegin();
void drawLogoScaled(TFT_eSPI &tft, int16_t x, int16_t y, int16_t w, int16_t h);

#endif
