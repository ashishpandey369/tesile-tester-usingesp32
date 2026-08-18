#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "pins.h"
#include "config.h"

class DisplayManager
{
public:
    void begin();
    void update();

    // Screens
    void showBootScreen();
    void showHomeScreen();
    void showRunningScreen();
    void showPassedScreen();
    void showFailedScreen();
    void showReturningScreen();
    void showCalibrationScreen(const String &msg = "");
    void showErrorScreen(const String &msg);

    // Update Values
    void setCurrentForce(float value);
    void setTargetForce(float value);
    void setMotorStatus(const String &status);
    void setMachineStatus(const String &status);
    void setProgress(uint8_t percent);

    void clear();

private:
    TFT_eSPI tft;

    //-----------------------------
    // Current Values
    //-----------------------------

    float currentForce = 0.0f;
    float targetForce = DEFAULT_TARGET_FORCE;

    String motorStatus = "STOP";
    String machineStatus = "READY";

    uint8_t progress = 0;

    //-----------------------------
    // Previous Values
    //-----------------------------

    float lastForce = -9999;
    float lastTarget = -9999;

    String lastMotor = "";
    String lastStatus = "";

    uint8_t lastProgress = 255;

    bool layoutDrawn = false;

    unsigned long lastRefresh = 0;

    //-----------------------------
    // Palette (computed once in begin())
    //-----------------------------

    uint16_t colorPanel;   // card / panel outline
    uint16_t colorBtnBlue; // UP / DOWN button fill

    //-----------------------------

    void drawLayout();

    void drawForce(); // now draws the big CURRENT FORCE card
    void drawTarget();
    void drawMotor();
    void drawStatus(); // now draws the mini MACHINE STATUS card
    void drawProgress();

    // Live force readout shown on the CALIBRATION card (so you can
    // actually see the reading move while you place the weight)
    void drawCalibrationForce();

    // Faux-bold helpers (default TFT_eSPI fonts have no bold variant,
    // so we double-stamp the glyph 1px to the right)
    void drawBoldString(const String &text, int32_t x, int32_t y, uint8_t font);
    void drawBoldCentreString(const String &text, int32_t x, int32_t y, uint8_t font);
};

extern DisplayManager display;

#endif