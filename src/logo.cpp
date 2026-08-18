#include "logo.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

const int16_t LOGO_SOURCE_W = 398;
const int16_t LOGO_SOURCE_H = 260;

namespace {
struct LogoRun {
    uint16_t count;
    uint8_t pixel;
};

static const LogoRun logoRuns[] PROGMEM = {
    {118,0}, {1,1}, {2,0}, {1,1}, {1,0}, {1,1}, {3,0}, {1,1}, {4,0}, {1,1}, {3,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1},
    {5,0}, {1,1}, {3,0}, {1,1}, {4,0}, {1,1}, {3,0}, {1,1}, {3,0}, {1,1}, {5,0}, {1,1}, {3,0}, {1,1}, {4,0}, {1,1},
    {5,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {3,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {3,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {3,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {3,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1},
    {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}, {4,0}, {1,1}, {4,0}, {1,1}, {5,0}, {1,1}
};

static const size_t LOGO_RUN_COUNT = sizeof(logoRuns) / sizeof(logoRuns[0]);

static void readRun(size_t index, uint16_t &count, uint8_t &pixel) {
    count = pgm_read_word(&logoRuns[index].count);
    pixel = pgm_read_byte(&logoRuns[index].pixel);
}

static bool loadSourceRow(int16_t sourceY, uint8_t *row) {
    if (sourceY < 0 || sourceY >= LOGO_SOURCE_H) return false;

    const uint32_t targetStart = (uint32_t)sourceY * LOGO_SOURCE_W;
    const uint32_t targetEnd = targetStart + LOGO_SOURCE_W;
    uint32_t pixelPos = 0;

    for (size_t i = 0; i < LOGO_RUN_COUNT && pixelPos < targetEnd; ++i) {
        uint16_t count;
        uint8_t pixel;
        readRun(i, count, pixel);

        const uint32_t runStart = pixelPos;
        const uint32_t runEnd = pixelPos + count;

        if (runEnd > targetStart && runStart < targetEnd) {
            uint32_t from = targetStart > runStart ? targetStart : runStart;
            uint32_t to = targetEnd < runEnd ? targetEnd : runEnd;
            for (uint32_t p = from; p < to; ++p) {
                row[p - targetStart] = pixel;
            }
        }

        pixelPos = runEnd;
    }

    return pixelPos >= targetEnd;
}
}

void logoBegin() {
}

void drawLogoScaled(TFT_eSPI &tft, int16_t x, int16_t y,
                    int16_t w, int16_t h) {
    if (w <= 0 || h <= 0 || w > 320 || h > 320) return;

    static uint8_t sourceRow[LOGO_SOURCE_W];
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
            outputRow[outX] = sourceRow[sourceX] ? TFT_WHITE : TFT_BLACK;
        }

        tft.pushImage(x, y + outY, w, 1, outputRow);
    }
}
