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
uint16_t sourceLine[400];
uint16_t scaledLine[240];

void *pngOpen(const char *filename, int32_t *size)
{
    File *file = new File(LittleFS.open(filename, "r"));
    if (file == nullptr || !(*file))
    {
        delete file;
        return nullptr;
    }

    *size = static_cast<int32_t>(file->size());
    Serial.printf("Logo PNG opened: %ld bytes\n", static_cast<long>(*size));
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

    const int16_t sourceWidth = static_cast<int16_t>(draw->iWidth);
    if (sourceWidth <= 0 || sourceWidth > 400)
        return 0;

    // Decode the complete source row first. The old implementation only
    // allocated 240 pixels even though the source image is 398 pixels wide,
    // which corrupted memory and caused PNG decoding to fail.
    png.getLineAsRGB565(draw, sourceLine, PNG_RGB565_BIG_ENDIAN, 0x0000);

    const int16_t outY = static_cast<int16_t>(
        (static_cast<int32_t>(draw->y) * targetH) / png.getHeight());

    // Several source rows can map to one destination row when downscaling.
    // Only draw the destination row once.
    static int16_t lastOutY = -1;
    if (outY == lastOutY)
        return 1;
    lastOutY = outY;

    for (int16_t x = 0; x < targetW; ++x)
    {
        const int16_t sourceX = static_cast<int16_t>(
            (static_cast<int32_t>(x) * sourceWidth) / targetW);
        scaledLine[x] = sourceLine[sourceX];
    }

    logoTft->pushImage(targetX, targetY + outY, targetW, 1, scaledLine);
    return 1;
}

bool drawLogoFile(TFT_eSPI &tft, int16_t x, int16_t y, int16_t w, int16_t h)
{
    if (!LittleFS.exists("/logo.png"))
    {
        Serial.println("Logo file missing: /logo.png");
        return false;
    }

    File checkFile = LittleFS.open("/logo.png", "r");
    const size_t fileSize = checkFile ? checkFile.size() : 0;
    checkFile.close();

    if (fileSize < 32)
    {
        Serial.printf("Logo file invalid: %u bytes\n", static_cast<unsigned>(fileSize));
        return false;
    }

    logoTft = &tft;
    targetX = x;
    targetY = y;
    targetW = min<int16_t>(w, 240);
    targetH = h;

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
        Serial.printf("Logo PNG decoded header: %dx%d\n", png.getWidth(), png.getHeight());
        const int decodeRc = png.decode(nullptr, 0);
        success = (decodeRc == PNG_SUCCESS);
        Serial.printf("Logo PNG decode rc=%d\n", decodeRc);
        png.close();
    }
    else
    {
        Serial.printf("Logo PNG open failed: rc=%d, file=%u bytes\n",
                      rc, static_cast<unsigned>(fileSize));
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

    File file = LittleFS.open("/logo.png", "r");
    if (!file)
    {
        Serial.println("Logo file missing: /logo.png");
        return;
    }

    Serial.printf("Logo filesystem ready, logo.png=%u bytes\n",
                  static_cast<unsigned>(file.size()));
    file.close();
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
