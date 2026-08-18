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

    void setCurrentForce(float value);
    void setMode(const String &mode);
    void setMotorStatus(const String &status);
    void setMachineStatus(const String &status);

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
    void drawForce();
    void drawMode();
    void drawMotor();
    void drawStatus();
};

extern DisplayManager display;

#endif