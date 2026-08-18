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

    // Proven force layout restored. Only reduce the size by about 10%.
    tft.fillRect(CONTENT_L, 90, 220, 55, TFT_BLACK);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString(String(currentForce, 3) + " kg", CONTENT_L, 95, 5);
}

void DisplayManager::drawMode()
{
    if (mode == lastMode)
        return;

    lastMode = mode;

    tft.fillRect(270, 88, 185, 54, TFT_BLACK);

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(mode == "TENSILE" ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
    tft.drawString(mode, 270, 114, 4);
}

void DisplayManager::drawMotor()
{
    if (motorStatus == lastMotor)
        return;

    lastMotor = motorStatus;

    tft.fillRect(CONTENT_L, 181, 230, 38, TFT_BLACK);

    // IMPORTANT: physical motor direction was restored in motor.cpp.
    // Only the displayed UP/DOWN label is inverted here as requested.
    String displayMotor = motorStatus;
    uint16_t color = TFT_YELLOW;

    if (motorStatus == "UP")
    {
        displayMotor = "DOWN";
        color = TFT_RED;
    }
    else if (motorStatus == "DOWN")
    {
        displayMotor = "UP";
        color = TFT_GREEN;
    }
    else
    {
        displayMotor = "STOP";
        color = TFT_YELLOW;
    }

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(color, TFT_BLACK);
    tft.drawString(displayMotor, CONTENT_L, 200, 4);
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
