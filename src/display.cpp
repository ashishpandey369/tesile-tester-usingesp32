#include "display.h"
#include "config.h"
#include <LittleFS.h>
#define PNG_MAX_BUFFERED_PIXELS 600
#include <PNGdec.h>

DisplayManager display;

#define SCREEN_W 480
#define SCREEN_H 320
#define OUTER_X 6
#define OUTER_Y 6
#define OUTER_W 468
#define OUTER_H 308
#define HEADER_LINE_Y 43
#define LOGO_PATH "/logo.png"
#define LOGO_X 16
#define LOGO_Y 10
#define LOGO_BOX_W 92
#define LOGO_BOX_H 24
#define LOGO_SAMPLE 4
#define LOGO_SOURCE_MAX 600
#define GRID_X 14
#define GRID_Y 51
#define GRID_W 452
#define GRID_H 246
#define CELL_GAP 8
#define CELL_W ((GRID_W - CELL_GAP) / 2)
#define CELL_H ((GRID_H - CELL_GAP) / 2)
#define LEFT_X GRID_X
#define RIGHT_X (GRID_X + CELL_W + CELL_GAP)
#define TOP_Y GRID_Y
#define BOTTOM_Y (GRID_Y + CELL_H + CELL_GAP)

namespace {
PNG logoPng;
File logoFile;
TFT_eSPI *logoTft = nullptr;

void *logoOpen(const char *filename, int32_t *size) {
    logoFile = LittleFS.open(filename, "r");
    if (!logoFile) { *size = 0; return nullptr; }
    *size = static_cast<int32_t>(logoFile.size());
    return &logoFile;
}

void logoClose(void *handle) { (void)handle; if (logoFile) logoFile.close(); }

int32_t logoRead(PNGFILE *file, uint8_t *buffer, int32_t length) {
    (void)file;
    return logoFile ? static_cast<int32_t>(logoFile.read(buffer, length)) : 0;
}

int32_t logoSeek(PNGFILE *file, int32_t position) {
    (void)file;
    if (!logoFile) return -1;
    return logoFile.seek(position) ? position : -1;
}

int logoDraw(PNGDRAW *pDraw) {
    if (!logoTft || pDraw->iWidth <= 0) return 0;
    if ((pDraw->y % LOGO_SAMPLE) != 0) return 1;

    static uint16_t sourceLine[LOGO_SOURCE_MAX];
    static uint16_t outputLine[LOGO_SOURCE_MAX / LOGO_SAMPLE + 2];
    int16_t sourceWidth = pDraw->iWidth;
    if (sourceWidth > LOGO_SOURCE_MAX) sourceWidth = LOGO_SOURCE_MAX;

    logoPng.getLineAsRGB565(pDraw, sourceLine, PNG_RGB565_BIG_ENDIAN, TFT_BLACK);

    int16_t outputWidth = 0;
    for (int16_t sx = 0; sx < sourceWidth && outputWidth < LOGO_BOX_W; sx += LOGO_SAMPLE)
        outputLine[outputWidth++] = sourceLine[sx];

    int16_t outputY = LOGO_Y + (pDraw->y / LOGO_SAMPLE);
    if (outputY < LOGO_Y || outputY >= LOGO_Y + LOGO_BOX_H || outputWidth <= 0) return 1;

    logoTft->pushImage(LOGO_X, outputY, outputWidth, 1, outputLine);
    return 1;
}
}

void DisplayManager::begin() {
    tft.init();
    tft.setRotation(DISPLAY_ROTATION);
    tft.fillScreen(TFT_BLACK);
    tft.setTextWrap(false);
    if (!LittleFS.begin(true)) Serial.println("LittleFS initialization failed; logo disabled");
    showBootScreen();
}

void DisplayManager::update() {
    if (millis() - lastRefresh < DISPLAY_REFRESH_MS) return;
    lastRefresh = millis();
    if (!layoutDrawn) return;
    drawForce(); drawMode(); drawMotor(); drawStatus();
}

void DisplayManager::clear() {
    tft.fillScreen(TFT_BLACK);
    layoutDrawn = false;
    lastForce = -9999.0f;
    lastMode = ""; lastMotor = ""; lastStatus = "";
}

void DisplayManager::showBootScreen() {
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

void DisplayManager::showHomeScreen() {
    clear();
    currentForce = INITIAL_CURRENT_FORCE;
    mode = "TENSILE"; motorStatus = "STOP"; machineStatus = "READY";
    drawLayout(); drawForce(); drawMode(); drawMotor(); drawStatus();
}

void DisplayManager::showErrorScreen(const String &msg) {
    clear();
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawCentreString("SYSTEM STOP", 240, 72, 4);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString(msg, 240, 135, 2);
    tft.drawCentreString("TURN SWITCH OFF", 240, 200, 2);
    tft.drawCentreString("AND RESTART", 240, 228, 2);
}

void DisplayManager::drawLogo() {
    tft.fillRect(LOGO_X, LOGO_Y, LOGO_BOX_W, LOGO_BOX_H, TFT_BLACK);
    if (!LittleFS.exists(LOGO_PATH)) {
        Serial.println("Logo not found: /logo.png");
        return;
    }

    logoTft = &tft;
    int rc = logoPng.open(LOGO_PATH, logoOpen, logoClose, logoRead, logoSeek, logoDraw);
    if (rc != PNG_SUCCESS) {
        File f = LittleFS.open(LOGO_PATH, "r");
        unsigned long bytes = f ? f.size() : 0;
        if (f) f.close();
        Serial.printf("Logo PNG open failed (PNGdec rc=%d, file=%lu bytes)\n", rc, bytes);
        logoTft = nullptr;
        return;
    }

    Serial.printf("Logo PNG: %d x %d, %d bpp\n", logoPng.getWidth(), logoPng.getHeight(), logoPng.getBpp());
    tft.startWrite();
    int decodeRc = logoPng.decode(nullptr, 0);
    tft.endWrite();
    logoPng.close();
    logoTft = nullptr;
    if (decodeRc != PNG_SUCCESS) Serial.printf("Logo PNG decode failed (PNGdec rc=%d)\n", decodeRc);
}

void DisplayManager::drawLayout() {
    layoutDrawn = true;
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawRoundRect(OUTER_X, OUTER_Y, OUTER_W, OUTER_H, 5, TFT_WHITE);
    drawLogo();
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("PUSH/PULL TESTER", 315, 24, 2);
    tft.drawFastHLine(OUTER_X + 8, HEADER_LINE_Y, OUTER_W - 16, TFT_DARKGREY);

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

void DisplayManager::drawForce() {
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

void DisplayManager::drawMode() {
    if (mode == lastMode) return;
    lastMode = mode;
    tft.fillRect(RIGHT_X + 4, TOP_Y + 34, CELL_W - 8, CELL_H - 39, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(mode == "TENSILE" ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
    tft.drawCentreString(mode, RIGHT_X + CELL_W / 2, TOP_Y + CELL_H / 2 + 4, 4);
}

void DisplayManager::drawMotor() {
    if (motorStatus == lastMotor) return;
    lastMotor = motorStatus;
    tft.fillRect(LEFT_X + 4, BOTTOM_Y + 34, CELL_W - 8, CELL_H - 39, TFT_BLACK);
    String displayMotor = motorStatus; uint16_t color = TFT_YELLOW;
    if (motorStatus == "UP") { displayMotor = "DOWN"; color = TFT_RED; }
    else if (motorStatus == "DOWN") { displayMotor = "UP"; color = TFT_GREEN; }
    tft.setTextDatum(MC_DATUM); tft.setTextColor(color, TFT_BLACK);
    tft.drawCentreString(displayMotor, LEFT_X + CELL_W / 2, BOTTOM_Y + CELL_H / 2 + 4, 4);
}

void DisplayManager::drawStatus() {
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

void DisplayManager::setCurrentForce(float value) {
    if (value < 0.0f) value = 0.0f;
    if (value > MAX_VIRTUAL_FORCE_KG) value = MAX_VIRTUAL_FORCE_KG;
    currentForce = value;
}
void DisplayManager::setMode(const String &newMode) { mode = newMode; }
void DisplayManager::setMotorStatus(const String &status) { motorStatus = status; }
void DisplayManager::setMachineStatus(const String &status) { machineStatus = status; }
