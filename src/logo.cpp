#include "logo.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <PNGdec.h>
#include <TFT_eSPI.h>

const int16_t LOGO_SOURCE_W = 398;
const int16_t LOGO_SOURCE_H = 260;

namespace
{
PNG png;
TFT_eSPI *logoTft = nullptr;
int16_t targetX = 0;
int16_t targetY = 0;
int16_t targetW = 0;
int16_t targetH = 0;
int16_t lastTargetRow = -1;
uint16_t lineBuffer[240];

void *pngOpen(const char *filename, int32_t *size)
{
    File *file = new File(LittleFS.open(filename, "r"));
    if (file == nullptr || !(*file))
    {
        delete file;
        return nullptr;
    }

    *size = static_cast<int32_t>(file->size());
    return file;
}

void pngClose(void *handle)
{
    File *file = static_cast<File *>(handle);
    if (file != nullptr)
    {
        file->close();
        delete file;
    }
}

int32_t pngRead(PNGFILE *file, uint8_t *buffer, int32_t length)
{
    if (file == nullptr || file->fHandle == nullptr)
        return 0;

    File *source = static_cast<File *>(file->fHandle);
    return static_cast<int32_t>(source->read(buffer, length));
}

int32_t pngSeek(PNGFILE *file, int32_t position)
{
    if (file == nullptr || file->fHandle == nullptr)
        return -1;

    File *source = static_cast<File *>(file->fHandle);
    return source->seek(position, SeekSet) ? position : -1;
}

int pngDraw(PNGDRAW *draw)
{
    if (logoTft == nullptr || draw == nullptr)
        return 0;

    const int16_t outY = static_cast<int16_t>(
        (static_cast<int32_t>(draw->y) * targetH) / png.getHeight());

    if (outY == lastTargetRow)
        return 1;

    lastTargetRow = outY;

    uint16_t sourceWidth = static_cast<uint16_t>(draw->iWidth);
    if (sourceWidth > 240)
        sourceWidth = 240;

    uint16_t decodedLine[240];
    png.getLineAsRGB565(draw, decodedLine, PNG_RGB565_BIG_ENDIAN, 0x0000);

    for (int16_t x = 0; x < targetW; ++x)
    {
        const int16_t sourceX = static_cast<int16_t>(
            (static_cast<int32_t>(x) * png.getWidth()) / targetW);

        lineBuffer[x] =
            (sourceX >= 0 && sourceX < sourceWidth)
                ? decodedLine[sourceX]
                : TFT_BLACK;
    }

    logoTft->pushImage(targetX, targetY + outY, targetW, 1, lineBuffer);
    return 1;
}

bool drawLogoFile(TFT_eSPI &tft, int16_t x, int16_t y, int16_t w, int16_t h)
{
    if (!LittleFS.exists("/logo.png"))
        return false;

    logoTft = &tft;
    targetX = x;
    targetY = y;
    targetW = min<int16_t>(w, 240);
    targetH = h;
    lastTargetRow = -1;

    tft.startWrite();

    const int rc = png.open(
        "/logo.png",
        pngOpen,
        pngClose,
        pngRead,
        pngSeek,
        pngDraw);

    bool success = false;

    if (rc == PNG_SUCCESS)
    {
        if (png.getWidth() <= 0 || png.getHeight() <= 0)
        {
            png.close();
        }
        else
        {
            const int decodeRc = png.decode(nullptr, 0);
            success = (decodeRc == PNG_SUCCESS);
            png.close();
        }
    }

    tft.endWrite();
    logoTft = nullptr;

    return success;
}
}

void logoBegin()
{
    if (!LittleFS.begin(false))
    {
        Serial.println("Logo filesystem mount failed");
        return;
    }

    if (!LittleFS.exists("/logo.png"))
        Serial.println("Logo file missing: /logo.png");
}

void drawLogoScaled(TFT_eSPI &tft, int16_t x, int16_t y,
                    int16_t w, int16_t h)
{
    if (!drawLogoFile(tft, x, y, w, h))
    {
        tft.fillRect(x, y, w, h, TFT_BLACK);
        Serial.println("Logo render failed");
    }
}
