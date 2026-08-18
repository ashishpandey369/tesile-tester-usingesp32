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

    // Draw the other UI first. Current force is deliberately drawn LAST
    // so nothing else can overwrite its display area.
    drawMode();
    drawMotor();
    drawStatus();
    drawForce();
}

void DisplayManager::clear()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawRect(6, 6, 468, 308, BORDER_COLOR);

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
    tft.drawCentreString("GOLD TESTER", 240, 90, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("ESP32 TENSILE TESTER", 240, 135, 2);
    tft.drawCentreString("Initializing...", 240, 180, 2);

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
    drawMode();
    drawMotor();
    drawStatus();
    drawForce();
}

void DisplayManager::showErrorScreen(const String &msg)
{
    clear();

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawCentreString("SYSTEM STOP", 240, 70, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString(msg, 240, 135, 2);
    tft.drawCentreString("Turn switch OFF", 240, 200, 2);
    tft.drawCentreString("and restart", 240, 230, 2);
}

void DisplayManager::drawLayout()
{
    layoutDrawn = true;

    tft.setTextDatum(TL_DATUM);
    tft.fillScreen(TFT_BLACK);
    tft.drawRect(6, 6, 468, 308, BORDER_COLOR);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("GOLD TESTER", 240, 18, 4);

    tft.drawFastHLine(CONTENT_L, 48, CONTENT_W, TFT_DARKGREY);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("CURRENT FORCE", CONTENT_L, 65, 2);
    tft.drawString("MODE", 250, 65, 2);

    tft.drawFastHLine(CONTENT_L, 155, CONTENT_W, TFT_DARKGREY);

    tft.drawString("MOTOR", CONTENT_L, 175, 2);
    tft.drawString("MACHINE STATUS", 250, 175, 2);

    tft.drawFastHLine(CONTENT_L, 245, CONTENT_W, TFT_DARKGREY);

    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawCentreString("PRESS UP/DOWN: MANUAL STEP", 240, 260, 2);
    tft.drawCentreString("HOLD: CONTINUOUS | SWITCH ON: TEST", 240, 285, 2);
}

void DisplayManager::drawForce()
{
    if (currentForce == lastForce)
        return;

    lastForce = currentForce;

    // Dedicated force region. Keep the proven position and use a fixed
    // font size so the value and kg remain visible and predictable.
    tft.fillRect(CONTENT_L, 90, 220, 55, TFT_BLACK);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString(String(currentForce, 3) + " kg", CONTENT_L, 96, 5);
}

void DisplayManager::drawMode()
{
    if (mode == lastMode)
        return;

    lastMode = mode;

    tft.fillRect(250, 90, 210, 55, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(mode == "TENSILE" ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
    tft.drawString(mode, 250, 105, 4);
}

void DisplayManager::drawMotor()
{
    if (motorStatus == lastMotor)
        return;

    lastMotor = motorStatus;

    tft.fillRect(CONTENT_L, 200, 210, 35, TFT_BLACK);

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

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(color, TFT_BLACK);
    tft.drawString(displayMotor, CONTENT_L, 205, 3);
}

void DisplayManager::drawStatus()
{
    if (machineStatus == lastStatus)
        return;

    lastStatus = machineStatus;

    tft.fillRect(250, 200, 210, 35, TFT_BLACK);

    uint16_t color = TFT_WHITE;
    if (machineStatus == "READY")
        color = TFT_GREEN;
    else if (machineStatus == "RUNNING")
        color = TFT_CYAN;
    else if (machineStatus == "STOP")
        color = TFT_RED;
    else if (machineStatus == "TURN OFF")
        color = TFT_ORANGE;

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(color, TFT_BLACK);
    tft.drawString(machineStatus, 250, 205, 3);

    if (machineStatus == "TURN OFF")
    {
        tft.setTextColor(TFT_ORANGE, TFT_BLACK);
        tft.drawCentreString("TURN SWITCH OFF", 240, 230, 2);
    }
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
