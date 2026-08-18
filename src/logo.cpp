#include "logo.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

const int16_t LOGO_SOURCE_W = 398;
const int16_t LOGO_SOURCE_H = 260;

namespace {
struct LogoRun {
    uint16_t count;
    uint16_t color;
};

/*
 * Exact RGB565 pixel data from the user's 398x260 converted logo.
 * Stored as compact run-length encoded 16-bit pixels so the logo stays
 * in flash and does not require LittleFS/PNG/BMP decoding.
 */
static const LogoRun logoRuns[] PROGMEM = {
    {1,0x0000}, {117,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000},
    {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000},
    {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000},
    {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}, {1,0x0000}
};

static const size_t LOGO_RUN_COUNT = sizeof(logoRuns) / sizeof(logoRuns[0]);

static void readRun(size_t index, uint16_t &count, uint16_t &color) {
    count = pgm_read_word(&logoRuns[index].count);
    color = pgm_read_word(&logoRuns[index].color);
}

static bool loadSourceRow(int16_t sourceY, uint16_t *row) {
    if (sourceY < 0 || sourceY >= LOGO_SOURCE_H) return false;

    const uint32_t targetStart = (uint32_t)sourceY * LOGO_SOURCE_W;
    const uint32_t targetEnd = targetStart + LOGO_SOURCE_W;
    uint32_t pixelPos = 0;

    for (size_t i = 0; i < LOGO_RUN_COUNT && pixelPos < targetEnd; ++i) {
        uint16_t count, color;
        readRun(i, count, color);

        const uint32_t runStart = pixelPos;
        const uint32_t runEnd = pixelPos + count;

        if (runEnd > targetStart && runStart < targetEnd) {
            uint32_t from = (targetStart > runStart) ? targetStart : runStart;
            uint32_t to = (targetEnd < runEnd) ? targetEnd : runEnd;

            for (uint32_t p = from; p < to; ++p) {
                row[p - targetStart] = color;
            }
        }

        pixelPos = runEnd;
    }

    return pixelPos >= targetEnd;
}
}

void logoBegin() {
    // Logo is compiled into firmware; no filesystem initialization is needed.
}

void drawLogoScaled(TFT_eSPI &tft, int16_t x, int16_t y,
                    int16_t w, int16_t h) {
    if (w <= 0 || h <= 0 || w > 320 || h > 320) return;

    static uint16_t sourceRow[LOGO_SOURCE_W];
    static uint16_t outputRow[320];

    int16_t cachedSourceY = -1;

    for (int16_t outY = 0; outY < h; ++outY) {
        const int16_t sourceY = (int32_t)outY * LOGO_SOURCE_H / h;

        if (sourceY != cachedSourceY) {
            if (!loadSourceRow(sourceY, sourceRow)) return;
            cachedSourceY = sourceY;
        }

        for (int16_t outX = 0; outX < w; ++outX) {
            const int16_t sourceX = (int32_t)outX * LOGO_SOURCE_W / w;
            outputRow[outX] = sourceRow[sourceX];
        }

        tft.pushImage(x, y + outY, w, 1, outputRow);
    }
}
