#ifndef LOGO_H
#define LOGO_H

#include <Arduino.h>

// Source logo converted to RGB565 by the user's online converter.
// The pixel payload is kept isolated from display.cpp.
extern const uint16_t logoPixels[] PROGMEM;
extern const int16_t LOGO_SOURCE_W;
extern const int16_t LOGO_SOURCE_H;

#endif
