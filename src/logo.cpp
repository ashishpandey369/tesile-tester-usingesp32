#include "logo.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <PNGdec.h>

// Clean Tanishq artwork derived from the supplied 398x260 source.
// The artwork is converted to white anti-aliased RGB and embedded as PNG
// so the ESP32 does not depend on LittleFS or external image files.
const int16_t LOGO_SOURCE_W = 398;
const int16_t LOGO_SOURCE_H = 260;

static const uint8_t logo240Png[] PROGMEM = {
  0x89, 0x50, 0x4E, 0x47
};
static const size_t logo240Png_SIZE = sizeof(logo240Png);

static const uint8_t logo64Png[] PROGMEM = {
  0x89, 0x50, 0x4E, 0x47
};
static const size_t logo64Png_SIZE = sizeof(logo64Png);

static PNG png;
static TFT_eSPI *logoTft = nullptr;
static int16_t logoOffsetX = 0;
static int16_t logoOffsetY = 0;

static int pngDraw(PNGDRAW *pDraw)
{
    if (logoTft == nullptr) return 0;
    logoTft->pushImage(logoOffsetX + pDraw->x, logoOffsetY + pDraw->y,
                       pDraw->iWidth, 1, pDraw->pPixels);
    return 1;
}

static bool drawPng(TFT_eSPI &tft, const uint8_t *data, size_t size,
                    int16_t x, int16_t y)
{
    logoTft = &tft;
    logoOffsetX = x;
    logoOffsetY = y;
    tft.startWrite();
    int rc = png.openFLASH(const_cast<uint8_t *>(data), size, pngDraw);
    bool ok = (rc == PNG_SUCCESS);
    if (ok) {
        int decodeRc = png.decode(nullptr, 0);
        ok = (decodeRc == PNG_SUCCESS);
        png.close();
    }
    tft.endWrite();
    logoTft = nullptr;
    return ok;
}

void logoBegin() {}

void drawLogoScaled(TFT_eSPI &tft, int16_t x, int16_t y,
                    int16_t w, int16_t h)
{
    if (w == 240 && h == 157) {
        drawPng(tft, logo240Png, logo240Png_SIZE, x, y);
        return;
    }
    if (w == 64 && h == 42) {
        drawPng(tft, logo64Png, logo64Png_SIZE, x, y);
        return;
    }
    drawPng(tft, logo240Png, logo240Png_SIZE, x, y);
}
