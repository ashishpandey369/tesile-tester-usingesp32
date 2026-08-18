#include "display.h"
#include "config.h"
#include <pgmspace.h>

DisplayManager display;

#define SCREEN_W 480
#define SCREEN_H 320
#define OUTER_X 6
#define OUTER_Y 6
#define OUTER_W 468
#define OUTER_H 308
#define HEADER_LINE_Y 50

#define LOGO_X 14
#define LOGO_Y 6
#define LOGO_W 64
#define LOGO_H 40

#define GRID_X 14
#define GRID_Y 57
#define GRID_W 452
#define GRID_H 240
#define CELL_GAP 6
#define CELL_W ((GRID_W - CELL_GAP) / 2)
#define CELL_H ((GRID_H - CELL_GAP) / 2)
#define LEFT_X GRID_X
#define RIGHT_X (GRID_X + CELL_W + CELL_GAP)
#define TOP_Y GRID_Y
#define BOTTOM_Y (GRID_Y + CELL_H + CELL_GAP)

// Inspected source logo: 398x260 RGBA Tanishq mark.
// Converted to a compact 64x40 1-bit white-on-black bitmap so it is
// deterministic on the ESP32 TFT and does not depend on PNGdec/LittleFS.
static const uint8_t tanishqLogo[LOGO_W * LOGO_H / 8] PROGMEM = {
    0x00, 0x00, 0x03, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0F, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1F, 0x81, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1D, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0xC0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xE0, 0x70, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x7F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0xFF, 0x01, 0x90, 0x46, 0x1E, 0x08, 0x63, 0xC0,
    0x07, 0x01, 0x90, 0x46, 0x37, 0x1C, 0x63, 0x60, 0x07, 0x03, 0x18, 0x46, 0x73, 0x1C, 0x66, 0x30,
    0x06, 0x07, 0x9C, 0x46, 0x38, 0x1C, 0x66, 0x18, 0x06, 0x0D, 0x9E, 0x46, 0x1E, 0x1C, 0x6C, 0x18,
    0x06, 0x19, 0x97, 0x46, 0x0F, 0x1C, 0x6E, 0x18, 0x06, 0x31, 0x93, 0xC6, 0x03, 0x9F, 0xE6, 0x18,
    0x0E, 0x7F, 0x91, 0xC6, 0x41, 0xDF, 0xE6, 0x18, 0x0E, 0x7F, 0x90, 0xC6, 0x60, 0xDC, 0x67, 0x30,
    0x0E, 0xE1, 0x90, 0x46, 0x3B, 0x9C, 0x63, 0xF0, 0x0E, 0xE1, 0x10, 0x46, 0x1F, 0x08, 0x61, 0xE0,
    0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70,
    0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
};

void DisplayManager::begin()
{
    tft.init();
    tft.setRotation(DISPLAY_ROTATION);
    tft.fillScreen(TFT_BLACK);
    tft.setTextWrap(false);
    showBootScreen();
}

void DisplayManager::update()
{
    if (millis() - lastRefresh < DISPLAY_REFRESH_MS) return;
    lastRefresh = millis();
    if (!layoutDrawn) return;
    drawForce(); drawMode(); drawMotor(); drawStatus();
}

void DisplayManager::clear()
{
    tft.fillScreen(TFT_BLACK);
    layoutDrawn = false;
    lastForce = -9999.0f;
    lastMode = ""; lastMotor = ""; lastStatus = "";
}

void DisplayManager::showBootScreen()
{
    clear();
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("PUSH/PULL TESTER", 240, 88, 4);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("ESP32 TEST SYSTEM", 240, 132, 2);
    tft.drawCentreString("Initializing...", 240, 178, 2);
    delay(BOOT_SCREEN_TIME);
    showHomeScreen();
}

void DisplayManager::showHomeScreen()
{
    clear();
    currentForce = INITIAL_CURRENT_FORCE;
    mode = "TENSILE"; motorStatus = "STOP"; machineStatus = "READY";
    drawLayout(); drawForce(); drawMode(); drawMotor(); drawStatus();
}

void DisplayManager::showErrorScreen(const String &msg)
{
    clear();
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawCentreString("SYSTEM STOP", 240, 72, 4);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString(msg, 240, 135, 2);
    tft.drawCentreString("TURN SWITCH OFF", 240, 200, 2);
    tft.drawCentreString("AND RESTART", 240, 228, 2);
}

void DisplayManager::drawLogo()
{
    tft.fillRect(LOGO_X, LOGO_Y, LOGO_W, LOGO_H, TFT_BLACK);
    tft.drawBitmap(LOGO_X, LOGO_Y, tanishqLogo, LOGO_W, LOGO_H, TFT_WHITE, TFT_BLACK);
}

void DisplayManager::drawLayout()
{
    layoutDrawn = true;
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawRoundRect(OUTER_X, OUTER_Y, OUTER_W, OUTER_H, 5, TFT_WHITE);

    // Compact header: logo on the left, bold title beside it.
    drawLogo();
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("PUSH/PULL TESTER", 300, 25, 4);
    tft.drawCentreString("PUSH/PULL TESTER", 301, 25, 4);
    tft.drawFastHLine(OUTER_X + 8, HEADER_LINE_Y, OUTER_W - 16, TFT_DARKGREY);

    // Four separate fields — kept independent, with reduced height to give the header more room.
    tft.drawRoundRect(LEFT_X, TOP_Y, CELL_W, CELL_H, 5, TFT_WHITE);
    tft.drawRoundRect(RIGHT_X, TOP_Y, CELL_W, CELL_H, 5, TFT_WHITE);
    tft.drawRoundRect(LEFT_X, BOTTOM_Y, CELL_W, CELL_H, 5, TFT_WHITE);
    tft.drawRoundRect(RIGHT_X, BOTTOM_Y, CELL_W, CELL_H, 5, TFT_WHITE);

    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawCentreString("CURRENT FORCE", LEFT_X + CELL_W / 2, TOP_Y + 17, 2);
    tft.drawCentreString("MODE", RIGHT_X + CELL_W / 2, TOP_Y + 17, 2);
    tft.drawCentreString("MOTOR", LEFT_X + CELL_W / 2, BOTTOM_Y + 17, 2);
    tft.drawCentreString("MACHINE STATUS", RIGHT_X + CELL_W / 2, BOTTOM_Y + 17, 2);
}

void DisplayManager::drawForce()
{
    if (currentForce == lastForce) return;
    lastForce = currentForce;
    const int16_t x = LEFT_X + 4, y = TOP_Y + 34;
    const int16_t w = CELL_W - 8, h = CELL_H - 39;
    tft.fillRect(x, y, w, h, TFT_BLACK);
    String value = String(currentForce, 3), unit = "kg";
    const uint8_t valueFont = 6, unitFont = 4;
    const int16_t gap = 5;
    int16_t valueW = tft.textWidth(value, valueFont), unitW = tft.textWidth(unit, unitFont);
    int16_t totalW = valueW + gap + unitW;
    int16_t startX = LEFT_X + (CELL_W - totalW) / 2;
    int16_t centerY = TOP_Y + CELL_H / 2 + 4;
    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK); tft.drawString(value, startX, centerY, valueFont);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.drawString(unit, startX + valueW + gap, centerY, unitFont);
}

void DisplayManager::drawMode()
{
    if (mode == lastMode) return;
    lastMode = mode;
    tft.fillRect(RIGHT_X + 4, TOP_Y + 34, CELL_W - 8, CELL_H - 39, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(mode == "TENSILE" ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
    tft.drawCentreString(mode, RIGHT_X + CELL_W / 2, TOP_Y + CELL_H / 2 + 4, 4);
}

void DisplayManager::drawMotor()
{
    if (motorStatus == lastMotor) return;
    lastMotor = motorStatus;
    tft.fillRect(LEFT_X + 4, BOTTOM_Y + 34, CELL_W - 8, CELL_H - 39, TFT_BLACK);
    String displayMotor = motorStatus; uint16_t color = TFT_YELLOW;
    if (motorStatus == "UP") { displayMotor = "DOWN"; color = TFT_RED; }
    else if (motorStatus == "DOWN") { displayMotor = "UP"; color = TFT_GREEN; }
    tft.setTextDatum(MC_DATUM); tft.setTextColor(color, TFT_BLACK);
    tft.drawCentreString(displayMotor, LEFT_X + CELL_W / 2, BOTTOM_Y + CELL_H / 2 + 4, 4);
}

void DisplayManager::drawStatus()
{
    if (machineStatus == lastStatus) return;
    lastStatus = machineStatus;
    tft.fillRect(RIGHT_X + 4, BOTTOM_Y + 34, CELL_W - 8, CELL_H - 39, TFT_BLACK);
    uint16_t color = TFT_WHITE;
    if (machineStatus == "READY") color = TFT_GREEN;
    else if (machineStatus == "RUNNING") color = TFT_CYAN;
    else if (machineStatus == "STOP") color = TFT_RED;
    else if (machineStatus == "TURN OFF") color = TFT_ORANGE;
    tft.setTextDatum(MC_DATUM); tft.setTextColor(color, TFT_BLACK);
    tft.drawCentreString(machineStatus, RIGHT_X + CELL_W / 2, BOTTOM_Y + CELL_H / 2 + 4, 4);
}

void DisplayManager::setCurrentForce(float value)
{
    if (value < 0.0f) value = 0.0f;
    if (value > MAX_VIRTUAL_FORCE_KG) value = MAX_VIRTUAL_FORCE_KG;
    currentForce = value;
}
void DisplayManager::setMode(const String &newMode) { mode = newMode; }
void DisplayManager::setMotorStatus(const String &status) { motorStatus = status; }
void DisplayManager::setMachineStatus(const String &status) { machineStatus = status; }
