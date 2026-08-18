#include "display.h"

DisplayManager display;

#define BORDER_COLOR TFT_CYAN
#define CONTENT_L 20
#define CONTENT_R 460
#define CONTENT_W (CONTENT_R - CONTENT_L)

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
    tft.drawRect(6, 6, 468, 308, BORDER_COLOR);

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("GOLD TESTER", 240, 22, 4);

    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawCentreString("ESP32 TENSILE TESTER", 240, 44, 1);

    tft.drawFastHLine(CONTENT_L, 57, CONTENT_W, TFT_DARKGREY);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("CURRENT FORCE", CONTENT_L, 68, 2);
    tft.drawString("MODE", 270, 68, 2);

    tft.drawFastHLine(CONTENT_L, 150, CONTENT_W, TFT_DARKGREY);

    tft.drawString("MOTOR", CONTENT_L, 162, 2);
    tft.drawString("MACHINE STATUS", 270, 162, 2);

    tft.drawFastHLine(CONTENT_L, 229, CONTENT_W, TFT_DARKGREY);

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawCentreString("UP / DOWN  =  MANUAL MOVEMENT", 240, 244, 1);
    tft.drawCentreString("SWITCH ON  =  START TEST", 240, 262, 1);
    tft.drawCentreString("RESET / MODE  =  RESET / SELECT MODE", 240, 280, 1);
}

void DisplayManager::drawForce()
{
    if (currentForce == lastForce)
        return;

    lastForce = currentForce;

    // Dedicated force panel. Use a fixed, known-safe font size and
    // center the complete value and unit as one visual block.
    const int16_t panelX = 20;
    const int16_t panelY = 84;
    const int16_t panelW = 240;
    const int16_t panelH = 62;

    tft.fillRect(panelX, panelY, panelW, panelH, TFT_BLACK);

    String forceText = String(currentForce, 3) + " kg";

    // Font 4 is deliberately used here so the entire "00.000 kg"
    // string always fits inside the 240 px force panel.
    const int textSize = 4;
    int16_t textWidth = tft.textWidth(forceText, textSize);

    int16_t x = panelX + (panelW - textWidth) / 2;
    if (x < panelX + 2)
        x = panelX + 2;

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString(String(currentForce, 3), x, 115, textSize);

    // Draw kg immediately after the measured numeric value.
    int16_t numberWidth = tft.textWidth(String(currentForce, 3), textSize);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(" kg", x + numberWidth, 115, textSize);
}

void DisplayManager::drawMode()
{
    if (mode == lastMode)
        return;

    lastMode = mode;

    tft.fillRect(270, 88, 185, 54, TFT_BLACK);

    tft.setTextDatum(ML_DATUM);

    if (mode == "TENSILE")
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
    else
        tft.setTextColor(TFT_ORANGE, TFT_BLACK);

    tft.drawString(mode, 270, 114, 4);
}

void DisplayManager::drawMotor()
{
    if (motorStatus == lastMotor)
        return;

    lastMotor = motorStatus;

    tft.fillRect(CONTENT_L, 181, 230, 38, TFT_BLACK);

    uint16_t color = TFT_YELLOW;

    if (motorStatus == "UP")
        color = TFT_GREEN;
    else if (motorStatus == "DOWN")
        color = TFT_RED;

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(color, TFT_BLACK);
    tft.drawString(motorStatus, CONTENT_L, 200, 4);
}

void DisplayManager::drawStatus()
{
    if (machineStatus == lastStatus)
        return;

    lastStatus = machineStatus;

    tft.fillRect(270, 181, 185, 38, TFT_BLACK);

    uint16_t color = TFT_WHITE;

    if (machineStatus == "READY")
        color = TFT_GREEN;
    else if (machineStatus == "RUNNING")
        color = TFT_CYAN;
    else if (machineStatus == "STOP")
        color = TFT_RED;

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(color, TFT_BLACK);
    tft.drawString(machineStatus, 270, 200, 4);
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
