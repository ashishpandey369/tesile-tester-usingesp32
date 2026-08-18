#include "display.h"

DisplayManager display;

// Border size in pixels (2mm ≈ 8 pixels at typical TFT DPI)
#define BORDER_SIZE 8
#define BORDER_COLOR TFT_CYAN

//====================================================
// Layout constants
//====================================================
// Outer border: drawRect(6,6,468,308) -> spans x:6..474, y:6..314
// CONTENT_L / CONTENT_R keep the SAME margin (19px) from the border
// on both the left and right side, so nothing is closer to one edge
// of the border than the other.
#define CONTENT_L 25
#define CONTENT_R 455
#define CONTENT_W (CONTENT_R - CONTENT_L) // 430

#define COL_GAP 30
#define COL_W ((CONTENT_W - COL_GAP) / 2)     // 200
#define COL_L_X CONTENT_L                     // 25
#define COL_R_X (CONTENT_L + COL_W + COL_GAP) // 255

// Vertical rhythm: fixed gap between a heading and the value below it
#define HEAD_Y1 50
#define VALUE_GAP 26
#define VALUE_Y1 (HEAD_Y1 + VALUE_GAP) // 76

#define HEAD_Y2 140
#define VALUE_Y2 (HEAD_Y2 + VALUE_GAP) // 166

#define PROGRESS_LABEL_Y 210
#define PROGRESS_BAR_Y 235
#define PROGRESS_BAR_H 24

#define FOOTER_LINE_Y 268
#define FOOTER_TEXT_Y 280

//====================================================
// Initialization
//====================================================

void DisplayManager::begin()
{
    tft.init();

    tft.setRotation(1);

    tft.fillScreen(TFT_BLACK);

    tft.setTextWrap(false);

    showBootScreen();
}

//====================================================

void DisplayManager::update()
{
    if (millis() - lastRefresh < DISPLAY_REFRESH_MS)
        return;

    lastRefresh = millis();

    // The grid widgets (force/target/motor/status/progress) only exist
    // on layout-based screens (drawLayout() sets layoutDrawn = true).
    // CALIBRATION and ERROR screens draw their own simple layout and
    // must NOT have this periodic redraw stamping stray text over them.
    if (!layoutDrawn)
        return;

    drawForce();
    drawTarget();
    drawMotor();
    drawStatus();
    drawProgress();
}

//====================================================

void DisplayManager::clear()
{
    tft.fillScreen(TFT_BLACK);

    // IMPORTANT: showBootScreen() sets MC_DATUM for its centered text and
    // never restores it. Every plain drawString() call after that (headings,
    // force/target values, motor/status) would then treat x,y as the CENTER
    // of the text instead of the top-left corner, pushing text outside the
    // border and out of alignment. Reset here so every screen starts clean.
    tft.setTextDatum(TL_DATUM);

    // Draw the border on EVERY screen so it never disappears
    tft.drawRect(6, 6, 468, 308, BORDER_COLOR);

    layoutDrawn = false;

    lastForce = -9999;
    lastTarget = -9999;

    lastMotor = "";
    lastStatus = "";

    lastProgress = 255;
}

//====================================================
// Boot Screen
//====================================================

void DisplayManager::showBootScreen()
{
    clear();

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);

    tft.drawCentreString("GOLD TESTER", 240, 100, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawCentreString("Precision Tensile Testing Machine", 240, 150, 2);

    tft.drawCentreString("Initializing...", 240, 200, 2);

    delay(1200);

    showHomeScreen();
}

//====================================================

void DisplayManager::showHomeScreen()
{
    clear();

    machineStatus = "READY";
    motorStatus = "STOP";
    progress = 0;

    drawLayout();

    drawForce();
    drawTarget();
    drawMotor();
    drawStatus();
    drawProgress();
}

//====================================================

void DisplayManager::showRunningScreen()
{
    machineStatus = "RUNNING";
    motorStatus = "FORWARD";

    drawStatus();
    drawMotor();
}

//====================================================

void DisplayManager::showPassedScreen()
{
    machineStatus = "PASSED";
    motorStatus = "STOP";
    progress = 100;

    drawStatus();
    drawMotor();
    drawProgress();

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawCentreString("TEST PASSED", 240, 185, 2);
}

//====================================================

void DisplayManager::showFailedScreen()
{
    machineStatus = "FAILED";
    motorStatus = "STOP";

    drawStatus();
    drawMotor();
    drawProgress();

    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawCentreString("TEST FAILED", 240, 185, 2);
}

//====================================================

void DisplayManager::showReturningScreen()
{
    machineStatus = "HOMING";
    motorStatus = "REVERSE";

    // Clear the "PASSED"/"FAILED" banner line before it goes stale
    tft.fillRect(0, 176, 480, 20, TFT_BLACK);

    drawStatus();
    drawMotor();
}

//====================================================

void DisplayManager::showCalibrationScreen(const String &msg)
{
    // Only wipe the screen and redraw the static heading the FIRST time
    // we enter calibration. Subsequent calls (e.g. moving from "Place
    // weight" -> "Saved") just update the message line, so the screen
    // doesn't flash/reset mid-flow.
    if (machineStatus != "CALIBRATION")
    {
        clear();

        machineStatus = "CALIBRATION";

        tft.setTextColor(TFT_YELLOW, TFT_BLACK);

        tft.drawCentreString("CALIBRATION", 240, 45, 4);

        tft.drawFastHLine(20, 75, 440, TFT_DARKGREY);
    }

    // Clear just the message area and draw the new message - keep inside border
    tft.fillRect(20, 110, 440, 130, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawCentreString(msg, 240, 155, 2);
}

//====================================================

void DisplayManager::showErrorScreen(const String &msg)
{
    clear();

    tft.setTextColor(TFT_RED, TFT_BLACK);

    tft.drawCentreString("SYSTEM ERROR", 240, 38, 4);

    tft.drawFastHLine(CONTENT_L, 60, CONTENT_W, TFT_RED);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawCentreString(msg, 240, 140, 2);

    tft.drawCentreString("Restart Machine", 240, 240, 2);
}

//====================================================
// Static Layout
//====================================================

void DisplayManager::drawLayout()
{
    if (layoutDrawn)
        return;

    layoutDrawn = true;

    tft.fillScreen(TFT_BLACK);

    //---------------- OUTER BORDER ----------------
    tft.drawRect(6, 6, 468, 308, BORDER_COLOR);

    //---------------- HEADER ----------------
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString("GOLD TESTER", 240, 14, 4);

    tft.drawFastHLine(CONTENT_L, 38, CONTENT_W, TFT_DARKGREY);

    //---------------- LEFT COLUMN ----------------
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawString("Applied Force", COL_L_X, HEAD_Y1, 2);

    tft.drawString("Target Force", COL_L_X, HEAD_Y2, 2);

    //---------------- RIGHT COLUMN ----------------
    tft.drawString("Actuator", COL_R_X, HEAD_Y1, 2);

    tft.drawString("Machine", COL_R_X, HEAD_Y2, 2);

    //---------------- Progress ----------------
    tft.drawString("Progress", COL_L_X, PROGRESS_LABEL_Y, 2);

    tft.drawRect(CONTENT_L, PROGRESS_BAR_Y, CONTENT_W, PROGRESS_BAR_H, TFT_WHITE);

    //---------------- FOOTER ----------------
    tft.drawFastHLine(CONTENT_L, FOOTER_LINE_Y, CONTENT_W, TFT_DARKGREY);

    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);

    tft.drawCentreString("Push START Switch to Begin", 240, FOOTER_TEXT_Y, 2);
}

//====================================================
// Remaining Functions
//====================================================

void DisplayManager::drawForce()
{
    if (currentForce == lastForce)
        return;

    lastForce = currentForce;

    tft.fillRect(COL_L_X, VALUE_Y1, COL_W, 35, TFT_BLACK);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);

    tft.drawString(String(currentForce, 3) + " kg", COL_L_X, VALUE_Y1, 4);
}
void DisplayManager::drawTarget()
{
    if (targetForce == lastTarget)
        return;

    lastTarget = targetForce;

    tft.fillRect(COL_L_X, VALUE_Y2, COL_W, 35, TFT_BLACK);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);

    tft.drawString(String(targetForce, 3) + " kg", COL_L_X, VALUE_Y2, 4);
}

void DisplayManager::drawMotor()
{
    if (motorStatus == lastMotor)
        return;

    lastMotor = motorStatus;

    tft.fillRect(COL_R_X, VALUE_Y1, COL_W, 35, TFT_BLACK);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);

    tft.drawString(motorStatus, COL_R_X, VALUE_Y1, 2);
}
void DisplayManager::drawStatus()
{
    if (machineStatus == lastStatus)
        return;

    lastStatus = machineStatus;

    tft.fillRect(COL_R_X, VALUE_Y2, COL_W, 35, TFT_BLACK);

    uint16_t color = TFT_WHITE;

    if (machineStatus == "READY")
        color = TFT_GREEN;

    else if (machineStatus == "RUNNING")
        color = TFT_CYAN;

    else if (machineStatus == "PASSED")
        color = TFT_GREEN;

    else if (machineStatus == "FAILED")
        color = TFT_RED;

    else if (machineStatus == "HOMING")
        color = TFT_YELLOW;

    else if (machineStatus == "ERROR")
        color = TFT_RED;

    tft.setTextColor(color, TFT_BLACK);

    tft.drawString(machineStatus, COL_R_X, VALUE_Y2, 2);
}

void DisplayManager::drawProgress()
{
    if (progress == lastProgress)
        return;

    lastProgress = progress;

    tft.fillRect(CONTENT_L, PROGRESS_BAR_Y + 3, CONTENT_W, PROGRESS_BAR_H - 6, TFT_BLACK);

    uint16_t width = map(progress, 0, 100, 0, CONTENT_W - 6);

    tft.fillRect(CONTENT_L + 3, PROGRESS_BAR_Y + 3, width, PROGRESS_BAR_H - 6, TFT_GREEN);

    tft.fillRect(CONTENT_R - 60, PROGRESS_LABEL_Y - 2, 60, 18, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawRightString(String(progress) + " %", CONTENT_R, PROGRESS_LABEL_Y, 2);
}

void DisplayManager::setCurrentForce(float value)
{
    currentForce = value;
}

void DisplayManager::setTargetForce(float value)
{
    targetForce = value;
}

void DisplayManager::setMotorStatus(const String &status)
{
    motorStatus = status;
}

void DisplayManager::setMachineStatus(const String &status)
{
    machineStatus = status;
}

void DisplayManager::setProgress(uint8_t percent)
{
    if (percent > 100)
        percent = 100;

    progress = percent;
}