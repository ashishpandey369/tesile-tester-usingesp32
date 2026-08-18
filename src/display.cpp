#include "display.h"

DisplayManager display;

#define BORDER_COLOR TFT_CYAN
#define SCREEN_W 480
#define SCREEN_H 320
#define OUTER_X 6
#define OUTER_Y 6
#define OUTER_W 468
#define OUTER_H 308
#define CONTENT_L 20
#define CONTENT_R 460
#define CONTENT_W (CONTENT_R - CONTENT_L)

// Physical reference for the 3.5-inch 480x320 panel.
// Approximate active area: 73.5 x 49.0 mm.
#define PX_PER_MM_X 6.53f
#define PX_PER_MM_Y 6.53f
#define MM_X(mm) ((int16_t)((mm) * PX_PER_MM_X + 0.5f))
#define MM_Y(mm) ((int16_t)((mm) * PX_PER_MM_Y + 0.5f))

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
    if (millis() - lastRefresh < DISPLAY_REFRESH_MS)
        return;

    lastRefresh = millis();

    if (!layoutDrawn)
        return;

    drawForce();
    drawMode();
    drawMotor();
    drawStatus();
}

void DisplayManager::clear()
{
    tft.fillScreen(TFT_BLACK);
    layoutDrawn = false;

    lastForce = -9999.0f;
    lastMode = "";
    lastMotor = "";
    lastStatus = "";
}

void DisplayManager::showBootScreen()
{
    clear();

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("GOLD TESTER", 240, 88, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("ESP32 TENSILE TESTER", 240, 132, 2);

    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawCentreString("Initializing...", 240, 176, 2);

    delay(BOOT_SCREEN_TIME);
    showHomeScreen();
}

void DisplayManager::showHomeScreen()
{
    clear();

    currentForce = INITIAL_CURRENT_FORCE;
    mode = "TENSILE";
    motorStatus = "STOP";
    machineStatus = "READY";

    drawLayout();
    drawForce();
    drawMode();
    drawMotor();
    drawStatus();
}

void DisplayManager::showErrorScreen(const String &msg)
{
    clear();

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawCentreString("SYSTEM STOP", 240, 72, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString(msg, 240, 132, 2);

    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.drawCentreString("TURN SWITCH OFF", 240, 195, 2);
    tft.drawCentreString("THEN RESTART", 240, 222, 2);
}

void DisplayManager::drawLayout()
{
    layoutDrawn = true;

    tft.fillScreen(TFT_BLACK);
    tft.drawRect(OUTER_X, OUTER_Y, OUTER_W, OUTER_H, BORDER_COLOR);

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("GOLD TESTER", 240, MM_Y(3.3f), 4);

    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawCentreString("ESP32 TENSILE TESTER", 240, MM_Y(6.8f), 1);

    tft.drawFastHLine(MM_X(3.0f), MM_Y(8.7f), MM_X(67.5f), TFT_DARKGREY);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("CURRENT FORCE", MM_X(3.0f), MM_Y(10.3f), 2);
    tft.drawString("MODE", MM_X(41.5f), MM_Y(10.3f), 2);

    tft.drawFastHLine(MM_X(3.0f), MM_Y(23.0f), MM_X(67.5f), TFT_DARKGREY);

    tft.drawString("MOTOR", MM_X(3.0f), MM_Y(24.8f), 2);
    tft.drawString("MACHINE STATUS", MM_X(41.5f), MM_Y(24.8f), 2);

    tft.drawFastHLine(MM_X(3.0f), MM_Y(35.1f), MM_X(67.5f), TFT_DARKGREY);

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawCentreString("UP / DOWN  =  MANUAL MOVEMENT", 240, MM_Y(37.4f), 1);
    tft.drawCentreString("SWITCH ON  =  START TEST", 240, MM_Y(40.2f), 1);
    tft.drawCentreString("RESET / MODE  =  RESET / SELECT MODE", 240, MM_Y(43.0f), 1);
}

void DisplayManager::drawForce()
{
    if (currentForce == lastForce)
        return;

    lastForce = currentForce;

    // Dedicated force area: approximately 34 mm x 10 mm.
    const int16_t panelX = MM_X(3.0f);
    const int16_t panelY = MM_Y(13.2f);
    const int16_t panelW = MM_X(34.0f);
    const int16_t panelH = MM_Y(8.5f);

    tft.fillRect(panelX, panelY, panelW, panelH, TFT_BLACK);

    // Always display 00.000 format, including both leading zeroes.
    char valueBuffer[20];
    snprintf(valueBuffer, sizeof(valueBuffer), "%06.3f", (double)currentForce);
    String value = String(valueBuffer);

    const int numberSize = 5;
    const int unitSize = 3;
    const int16_t gap = 5;

    int16_t numberWidth = tft.textWidth(value, numberSize);
    int16_t unitWidth = tft.textWidth("kg", unitSize);
    int16_t totalWidth = numberWidth + gap + unitWidth;

    // Center the complete value + unit block in the physical force area.
    int16_t startX = panelX + (panelW - totalWidth) / 2;
    if (startX < panelX + 2)
        startX = panelX + 2;

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString(value, startX, panelY + panelH / 2, numberSize);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("kg", startX + numberWidth + gap, panelY + panelH / 2, unitSize);
}

void DisplayManager::drawMode()
{
    if (mode == lastMode)
        return;

    lastMode = mode;

    const int16_t panelX = MM_X(41.5f);
    const int16_t panelY = MM_Y(13.2f);
    const int16_t panelW = MM_X(27.0f);
    const int16_t panelH = MM_Y(8.5f);

    tft.fillRect(panelX, panelY, panelW, panelH, TFT_BLACK);

    tft.setTextDatum(ML_DATUM);
    if (mode == "TENSILE")
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
    else
        tft.setTextColor(TFT_ORANGE, TFT_BLACK);

    tft.drawString(mode, panelX, panelY + panelH / 2, 4);
}

void DisplayManager::drawMotor()
{
    if (motorStatus == lastMotor)
        return;

    lastMotor = motorStatus;

    const int16_t panelX = MM_X(3.0f);
    const int16_t panelY = MM_Y(27.4f);
    const int16_t panelW = MM_X(34.0f);
    const int16_t panelH = MM_Y(6.0f);

    tft.fillRect(panelX, panelY, panelW, panelH, TFT_BLACK);

    uint16_t color = TFT_YELLOW;
    if (motorStatus == "UP")
        color = TFT_GREEN;
    else if (motorStatus == "DOWN")
        color = TFT_RED;

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(color, TFT_BLACK);
    tft.drawString(motorStatus, panelX, panelY + panelH / 2, 4);
}

void DisplayManager::drawStatus()
{
    if (machineStatus == lastStatus)
        return;

    lastStatus = machineStatus;

    const int16_t panelX = MM_X(41.5f);
    const int16_t panelY = MM_Y(27.4f);
    const int16_t panelW = MM_X(27.0f);
    const int16_t panelH = MM_Y(6.0f);

    tft.fillRect(panelX, panelY, panelW, panelH, TFT_BLACK);

    uint16_t color = TFT_WHITE;
    if (machineStatus == "READY")
        color = TFT_GREEN;
    else if (machineStatus == "RUNNING")
        color = TFT_CYAN;
    else if (machineStatus == "STOP")
        color = TFT_RED;

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(color, TFT_BLACK);
    tft.drawString(machineStatus, panelX, panelY + panelH / 2, 4);
}

void DisplayManager::setCurrentForce(float value)
{
    if (value < 0.0f)
        value = 0.0f;

    if (value > MAX_VIRTUAL_FORCE_KG)
        value = MAX_VIRTUAL_FORCE_KG;

    currentForce = value;
}

void DisplayManager::setMode(const String &newMode)
{
    mode = newMode;
}

void DisplayManager::setMotorStatus(const String &status)
{
    motorStatus = status;
}

void DisplayManager::setMachineStatus(const String &status)
{
    machineStatus = status;
}
