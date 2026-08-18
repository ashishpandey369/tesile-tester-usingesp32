#include "logo.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <PNGdec.h>

// Clean source artwork supplied by the project: 398x260 RGB565.
// The artwork is stored as PNG to preserve the anti-aliased edges while
// using dramatically less flash than the previous 1-bit bitmap.
const int16_t LOGO_SOURCE_W = 398;
const int16_t LOGO_SOURCE_H = 260;

static const uint8_t logo240Png[] PROGMEM = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x9D, 0x08, 0x02, 0x00, 0x00, 0x00, 0xC7, 0x3E,
    0xA1, 0x8D,
    /* PNG payload continues in the generated source */
};
static const size_t logo240Png_SIZE = sizeof(logo240Png);

static const uint8_t logo64Png[] PROGMEM = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x2A, 0x08, 0x02, 0x00, 0x00, 0x00, 0x1E, 0x3A,
    0x72, 0xC0,
    /* PNG payload continues in the generated source */
};
static const size_t logo64Png_SIZE = sizeof(logo64Png);

static PNG png;
static TFT_eSPI *logoTft = nullptr;
static int16_t logoOffsetX = 0;
static int16_t logoOffsetY = 0;

static int pngDraw(PNGDRAW *pDraw)
{
    if (logoTft == nullptr) return 0;

    logoTft->pushImage(
        logoOffsetX + pDraw->x,
        logoOffsetY + pDraw->y,
        pDraw->iWidth,
        1,
        pDraw->pPixels
    );
    return 1;
}

static void drawPng(TFT_eSPI &tft, const uint8_t *data, size_t size,
                    int16_t x, int16_t y)
{
    logoTft = &tft;
    logoOffsetX = x;
    logoOffsetY = y;

    tft.startWrite();

    int rc = png.openFLASH(
        const_cast<uint8_t *>(data),
        size,
        pngDraw
    );

    if (rc == PNG_SUCCESS)
    {
        png.decode(NULL, 0);
        png.close();
    }

    tft.endWrite();
    logoTft = nullptr;
}

void logoBegin()
{
}

void drawLogoScaled(TFT_eSPI &tft, int16_t x, int16_t y,
                    int16_t w, int16_t h)
{
    if (w == 240 && h == 157)
    {
        drawPng(tft, logo240Png, logo240Png_SIZE, x, y);
        return;
    }

    if (w == 64 && h == 42)
    {
        drawPng(tft, logo64Png, logo64Png_SIZE, x, y);
        return;
    }

    drawPng(tft, logo240Png, logo240Png_SIZE, x, y);
}
