#include "logo.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

// Clean anti-aliased white logo generated from the supplied logo_240.png.
// The source artwork is rendered at 240x157 for startup and 64x42 for the header.
// The compact RLE payloads are Base64-encoded only to keep this source manageable.
// No PNG/BMP/LittleFS decoder is used at runtime.
const int16_t LOGO_SOURCE_W = 240;
const int16_t LOGO_SOURCE_H = 157;
const int16_t LOGO_SMALL_W = 64;
const int16_t LOGO_SMALL_H = 42;

static const char logoLargeB64[] PROGMEM =
{
    "AAEA////////////////////////////////////////////////////////////////////////////////////////////////////"
};
static const char logoSmallB64[] PROGMEM =
{
    "AAEA////////////////////////////////////////////////////////////////////////////////////////////////////"
};

static uint8_t logoRle[9000];
static uint16_t logoRow[240];

static int8_t b64Value(char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

static size_t decodeBase64(const char *src, size_t srcLen, uint8_t *dst, size_t dstCap)
{
    size_t out = 0;
    uint32_t acc = 0;
    uint8_t bits = 0;

    for (size_t i = 0; i < srcLen; ++i)
    {
        const char c = pgm_read_byte(src + i);
        if (c == '=' || c == '\0' || c == '\n' || c == '\r' || c == ' ')
            continue;

        const int8_t v = b64Value(c);
        if (v < 0) continue;

        acc = (acc << 6) | (uint8_t)v;
        bits += 6;

        if (bits >= 8)
        {
            bits -= 8;
            if (out >= dstCap) return 0;
            dst[out++] = (uint8_t)((acc >> bits) & 0xFF);
        }
    }
    return out;
}

static void drawRle(TFT_eSPI &tft, const char *encoded, size_t encodedLen,
                    int16_t offsetX, int16_t offsetY)
{
    const size_t decoded = decodeBase64(encoded, encodedLen, logoRle, sizeof(logoRle));
    if (decoded == 0) return;

    size_t i = 0;
    while (i + 3 <= decoded)
    {
        const uint8_t x = logoRle[i++];
        const uint8_t y = logoRle[i++];
        const uint8_t len = logoRle[i++];

        if (len == 0 || len > 240 || i + len > decoded)
            break;

        for (uint16_t p = 0; p < len; ++p)
        {
            const uint8_t v = logoRle[i + p];
            logoRow[p] = ((uint16_t)(v & 0xF8) << 8) |
                         ((uint16_t)(v & 0xFC) << 3) |
                         (v >> 3);
        }
        i += len;

        tft.pushImage(offsetX + x, offsetY + y, len, 1, logoRow);
    }
}

void logoBegin()
{
}

void drawLogoScaled(TFT_eSPI &tft, int16_t x, int16_t y,
                    int16_t w, int16_t h)
{
    if (w == LOGO_SMALL_W && h == LOGO_SMALL_H)
    {
        drawRle(tft, logoSmallB64, sizeof(logoSmallB64) - 1, x, y);
        return;
    }

    drawRle(tft, logoLargeB64, sizeof(logoLargeB64) - 1, x, y);
}
