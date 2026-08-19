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

    void showBootScreen();
    void showHomeScreen();
    void showErrorScreen(const String &msg);

    // Keep these names aligned with the current display.cpp implementation.
    void updateForce(float value);
    void updateMode(const String &mode);
    void updateMotor(const String &status);
    void updateStatus(const String &status);

    void clear();

private:
    TFT_eSPI tft;

    float currentForce = INITIAL_CURRENT_FORCE;
    String mode = "TENSILE";
    String motorStatus = "STOP";
    String machineStatus = "READY";

    float lastForce = -9999.0f;
    String lastMode = "";
    String lastMotor = "";
    String lastStatus = "";

    bool layoutDrawn = false;
    unsigned long lastRefresh = 0;

    void drawLayout();
    void drawLogo();
    void drawForce();
    void drawMode();
    void drawMotor();
    void drawStatus();
};

extern DisplayManager display;

#endif
