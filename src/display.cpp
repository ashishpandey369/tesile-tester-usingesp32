#include "display.h"

DisplayManager display;

#define BORDER_COLOR TFT_CYAN
#define CONTENT_L 20
#define CONTENT_R 460
#define CONTENT_W (CONTENT_R - CONTENT_L)

//====================================================
// Initialization
//====================================================

void DisplayManager::begin()
{
    tft.init();
    tft.setRotation(DISPLAY_ROTATION);
    tft.fillScreen(TFT_BLACK);
    tft.setTextWrap(false);

    showBootScreen();
}

//====================================================
// Display Update
//====================================================

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

//====================================================
// Clear Screen
//====================================================

void DisplayManager::clear()
{
    tft.fillScreen(TFT_BLACK);
    layoutDrawn = false;

    lastForce = -9999.0f;
    lastMode = "";
    lastMotor = "";
    lastStatus = "";
}

//====================================================
// Boot Screen
//====================================================

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

//====================================================
// Home Screen
//====================================================

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

//====================================================
// Error Screen
//====================================================

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

//====================================================
// Main Layout
//====================================================

void DisplayManager::drawLayout()
{
    layoutDrawn = true;

    tft.fillScreen(TFT_BLACK);

    // Outer frame
    tft.drawRect(6, 6, 468, 308, BORDER_COLOR);

    // Header
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("GOLD TESTER", 240, 22, 4);

    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawCentreString("ESP32 TENSILE TESTER", 240, 44, 1);

    tft.drawFastHLine(CONTENT_L, 57, CONTENT_W, TFT_DARKGREY);

    // Force / Mode labels
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("CURRENT FORCE", CONTENT_L, 68, 2);
    tft.drawString("MODE", 270, 68, 2);

    tft.drawFastHLine(CONTENT_L, 150, CONTENT_W, TFT_DARKGREY);

    // Motor / Machine labels
    tft.drawString("MOTOR", CONTENT_L, 162, 2);
    tft.drawString("MACHINE STATUS", 270, 162, 2);

    tft.drawFastHLine(CONTENT_L, 229, CONTENT_W, TFT_DARKGREY);

    // Footer instructions - intentionally kept short and clear.
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawCentreString("UP / DOWN  =  MANUAL MOVEMENT", 240, 244, 1);
    tft.drawCentreString("SWITCH ON  =  START TEST", 240, 262, 1);
    tft.drawCentreString("RESET / MODE  =  RESET / SELECT MODE", 240, 280, 1);
}

//====================================================
// Current Force
//====================================================

void DisplayManager::drawForce()
{
    if (currentForce == lastForce)
        return;

    lastForce = currentForce;

    // Dedicated value area.
    tft.fillRect(CONTENT_L, 88, 230, 52, TFT_BLACK);

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString(String(currentForce, 3) + " kg", CONTENT_L, 114, 4);
}

//====================================================
// Mode
//====================================================

void DisplayManager::drawMode()
{
    if (mode == lastMode)
        return;

    lastMode = mode;

    tft.fillRect(270, 88, 185, 52, TFT_BLACK);

    tft.setTextDatum(ML_DATUM);

    if (mode == "TENSILE")
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
    else
        tft.setTextColor(TFT_ORANGE, TFT_BLACK);

    tft.drawString(mode, 270, 114, 4);
}

//====================================================
// Motor Status
//====================================================

void DisplayManager::drawMotor()
{
    if (motorStatus == lastMotor)
        return;

    lastMotor = motorStatus;

    tft.fillRect(CONTENT_L, 181, 230, 38, TFT_BLACK);

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString(motorStatus, CONTENT_L, 200, 3);
}

//====================================================
// Machine Status
//====================================================

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
    else if (machineStatus == "TURN OFF")
        color = TFT_ORANGE;
    else if (machineStatus == "PRESS BUTTON")
        color = TFT_ORANGE;

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(color, TFT_BLACK);
    tft.drawString(machineStatus, 270, 200, 3);
}

//====================================================
// Setters
//====================================================

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
