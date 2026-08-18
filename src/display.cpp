#include "display.h"
#include "config.h"
#include <LittleFS.h>

DisplayManager display;

#define SCREEN_W 480
#define SCREEN_H 320
#define OUTER_X 6
#define OUTER_Y 6
#define OUTER_W 468
#define OUTER_H 308
#define HEADER_LINE_Y 50
#define LOGO_X 14
#define LOGO_Y 14
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
#define LOGO_BMP_PATH "/logo.bmp"

namespace {
File logoFile;
uint32_t bmpPixelOffset = 0;
int32_t bmpWidth = 0;
int32_t bmpHeight = 0;
uint32_t bmpRowBytes = 0;
bool bmpReady = false;

static uint32_t le32(const uint8_t *p) {
    return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}

static uint16_t alphaTo565(uint8_t alpha) {
    const uint16_t r = (uint16_t(alpha) * 31 + 127) / 255;
    const uint16_t g = (uint16_t(alpha) * 63 + 127) / 255;
    const uint16_t b = (uint16_t(alpha) * 31 + 127) / 255;
    return (r << 11) | (g << 5) | b;
}

static bool openLogoBmp() {
    bmpReady = false;
    if (!LittleFS.exists(LOGO_BMP_PATH)) {
        Serial.println("Logo BMP not found: /logo.bmp");
        return false;
    }
    logoFile = LittleFS.open(LOGO_BMP_PATH, "r");
    if (!logoFile || logoFile.size() < 138) {
        Serial.println("Logo BMP open failed");
        return false;
    }

    uint8_t header[138];
    if (logoFile.read(header, sizeof(header)) != sizeof(header) || header[0] != 'B' || header[1] != 'M') {
        Serial.println("Logo BMP header invalid");
        logoFile.close();
        return false;
    }

    bmpPixelOffset = le32(header + 10);
    bmpWidth = (int32_t)le32(header + 18);
    bmpHeight = (int32_t)le32(header + 22);
    const uint16_t bpp = uint16_t(header[28]) | (uint16_t(header[29]) << 8);
    const uint32_t compression = le32(header + 30);

    if (bmpWidth <= 0 || bmpHeight == 0 || bpp != 32 || compression != 3) {
        Serial.printf("Unsupported logo BMP: %ldx%ld %u-bit comp=%lu\n", (long)bmpWidth, (long)bmpHeight, bpp, (unsigned long)compression);
        logoFile.close();
        return false;
    }

    if (bmpHeight < 0) bmpHeight = -bmpHeight;
    bmpRowBytes = uint32_t(bmpWidth) * 4U;
    bmpReady = true;
    Serial.printf("Logo BMP ready: %ldx%ld, 32-bit BGRA\n", (long)bmpWidth, (long)bmpHeight);
    return true;
}

static bool readBmpRow(int32_t sourceY, uint8_t *row) {
    if (!bmpReady || sourceY < 0 || sourceY >= bmpHeight) return false;
    const uint32_t fileY = uint32_t(bmpHeight - 1 - sourceY);
    if (!logoFile.seek(bmpPixelOffset + fileY * bmpRowBytes)) return false;
    return logoFile.read(row, bmpRowBytes) == bmpRowBytes;
}

static void drawStartupLogo(TFT_eSPI &tft) {
    static uint8_t row[1600];
    static uint16_t pixels[400];
    const int16_t x0 = (SCREEN_W - bmpWidth) / 2;
    const int16_t y0 = (SCREEN_H - bmpHeight) / 2;
    for (int32_t y = 0; y < bmpHeight; ++y) {
        if (!readBmpRow(y, row)) return;
        for (int32_t x = 0; x < bmpWidth; ++x) pixels[x] = alphaTo565(row[x * 4 + 3]);
        tft.pushImage(x0, y0 + y, bmpWidth, 1, pixels);
    }
}

static void drawHeaderLogo(TFT_eSPI &tft) {
    static uint8_t row[1600];
    static uint16_t pixels[LOGO_W];
    for (int16_t oy = 0; oy < LOGO_H; ++oy) {
        const int32_t sourceY = (int32_t(oy) * bmpHeight) / LOGO_H;
        if (!readBmpRow(sourceY, row)) return;
        for (int16_t ox = 0; ox < LOGO_W; ++ox) {
            const int32_t sourceX = (int32_t(ox) * bmpWidth) / LOGO_W;
            pixels[ox] = alphaTo565(row[sourceX * 4 + 3]);
        }
        tft.pushImage(LOGO_X, LOGO_Y + oy, LOGO_W, 1, pixels);
    }
}
}

void DisplayManager::begin() {
    tft.init();
    tft.setRotation(DISPLAY_ROTATION);
    tft.fillScreen(TFT_BLACK);
    tft.setTextWrap(false);
    if (!LittleFS.begin(true)) Serial.println("LittleFS initialization failed");
    openLogoBmp();
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
    tft.fillScreen(TFT_BLACK);
    if (bmpReady) drawStartupLogo(tft);
    else {
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawCentreString("LOGO ERROR", 240, 130, 4);
    }
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Initializing...", 240, 286, 4);
    delay(3000);
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
    tft.setTextColor(TFT_RED, TFT_BLACK); tft.drawCentreString("SYSTEM STOP", 240, 72, 4);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.drawCentreString(msg, 240, 135, 2);
    tft.drawCentreString("TURN SWITCH OFF", 240, 200, 2); tft.drawCentreString("AND RESTART", 240, 228, 2);
}

void DisplayManager::drawLogo() {
    tft.fillRect(LOGO_X, LOGO_Y, LOGO_W, LOGO_H, TFT_BLACK);
    if (bmpReady) drawHeaderLogo(tft);
}

void DisplayManager::drawLayout() {
    layoutDrawn = true;
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawRoundRect(OUTER_X, OUTER_Y, OUTER_W, OUTER_H, 5, TFT_WHITE);
    drawLogo();
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("PUSH/PULL TESTER", 300, 25, 4);
    tft.drawCentreString("PUSH/PULL TESTER", 301, 25, 4);
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
    const int16_t x=LEFT_X+4,y=TOP_Y+34,w=CELL_W-8,h=CELL_H-39;
    tft.fillRect(x,y,w,h,TFT_BLACK);
    String value=String(currentForce,3), unit="kg";
    const uint8_t valueFont=6,unitFont=4; const int16_t gap=5;
    const int16_t valueW=tft.textWidth(value,valueFont),unitW=tft.textWidth(unit,unitFont);
    const int16_t totalW=valueW+gap+unitW,startX=LEFT_X+(CELL_W-totalW)/2,centerY=TOP_Y+CELL_H/2+4;
    tft.setTextDatum(ML_DATUM); tft.setTextColor(TFT_CYAN,TFT_BLACK); tft.drawString(value,startX,centerY,valueFont);
    tft.setTextColor(TFT_WHITE,TFT_BLACK); tft.drawString(unit,startX+valueW+gap,centerY,unitFont);
}

void DisplayManager::drawMode() {
    if(mode==lastMode)return; lastMode=mode;
    tft.fillRect(RIGHT_X+4,TOP_Y+34,CELL_W-8,CELL_H-39,TFT_BLACK);
    tft.setTextDatum(MC_DATUM); tft.setTextColor(mode=="TENSILE"?TFT_GREEN:TFT_ORANGE,TFT_BLACK);
    tft.drawCentreString(mode,RIGHT_X+CELL_W/2,TOP_Y+CELL_H/2+4,4);
}

void DisplayManager::drawMotor() {
    if(motorStatus==lastMotor)return; lastMotor=motorStatus;
    tft.fillRect(LEFT_X+4,BOTTOM_Y+34,CELL_W-8,CELL_H-39,TFT_BLACK);
    String displayMotor=motorStatus; uint16_t color=TFT_YELLOW;
    if(motorStatus=="UP"){displayMotor="DOWN";color=TFT_RED;} else if(motorStatus=="DOWN"){displayMotor="UP";color=TFT_GREEN;}
    tft.setTextDatum(MC_DATUM); tft.setTextColor(color,TFT_BLACK);
    tft.drawCentreString(displayMotor,LEFT_X+CELL_W/2,BOTTOM_Y+CELL_H/2+4,4);
}

void DisplayManager::drawStatus() {
    if(machineStatus==lastStatus)return; lastStatus=machineStatus;
    tft.fillRect(RIGHT_X+4,BOTTOM_Y+34,CELL_W-8,CELL_H-39,TFT_BLACK);
    uint16_t color=TFT_WHITE;
    if(machineStatus=="READY")color=TFT_GREEN; else if(machineStatus=="RUNNING")color=TFT_CYAN; else if(machineStatus=="STOP")color=TFT_RED; else if(machineStatus=="TURN OFF")color=TFT_ORANGE;
    tft.setTextDatum(MC_DATUM); tft.setTextColor(color,TFT_BLACK);
    tft.drawCentreString(machineStatus,RIGHT_X+CELL_W/2,BOTTOM_Y+CELL_H/2+4,4);
}

void DisplayManager::setCurrentForce(float value){if(value<0.0f)value=0.0f;if(value>MAX_VIRTUAL_FORCE_KG)value=MAX_VIRTUAL_FORCE_KG;currentForce=value;}
void DisplayManager::setMode(const String &newMode){mode=newMode;}
void DisplayManager::setMotorStatus(const String &status){motorStatus=status;}
void DisplayManager::setMachineStatus(const String &status){machineStatus=status;}
