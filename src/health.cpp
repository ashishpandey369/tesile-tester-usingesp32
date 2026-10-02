#include "health.h"
#include "pins.h"

HealthMonitor health;

namespace { constexpr uint32_t HEALTH_REPORT_INTERVAL_MS = 1000; }

void HealthMonitor::begin() {
    lastReport = 0;
    Serial.println();
    Serial.println("========== ESP32 HEALTH MONITOR ==========");
    Serial.println("Read-only GPIO diagnostic enabled.");
    Serial.println("WARNING: This monitor does NOT drive any GPIO.");
    Serial.println("==========================================");
}

void HealthMonitor::printPin(const char *name, uint8_t pin, const char *role) {
    const int level = digitalRead(pin);
    Serial.print(name);
    Serial.print(" GPIO");
    Serial.print(pin);
    Serial.print(" = ");
    Serial.print(level ? "HIGH" : "LOW");
    Serial.print(" | ");
    Serial.println(role);
}

void HealthMonitor::update() {
    const uint32_t now = millis();
    if (now - lastReport < HEALTH_REPORT_INTERVAL_MS) return;
    lastReport = now;

    Serial.println();
    Serial.println("[HEALTH] -------- GPIO STATUS --------");

    printPin("UP", BUTTON_UP_PIN, "INPUT_PULLUP / button");
    printPin("DOWN", BUTTON_DOWN_PIN, "INPUT_PULLUP / button");
    printPin("RESET", RESET_MODE_BUTTON_PIN, "INPUT_PULLUP / mode-reset");
    printPin("START", START_SWITCH_PIN, "INPUT_PULLUP / master switch");

    printPin("RPWM", BTS7960_RPWM_PIN, "BTS7960 motor PWM");
    printPin("LPWM", BTS7960_LPWM_PIN, "BTS7960 motor PWM");
    printPin("R_EN", BTS7960_R_EN_PIN, "BTS7960 right enable");
    printPin("L_EN", BTS7960_L_EN_PIN, "BTS7960 left enable");

    printPin("TFT_SCLK", DISPLAY_SCLK_PIN, "ILI9488 SPI clock");
    printPin("TFT_MOSI", DISPLAY_MOSI_PIN, "ILI9488 SPI MOSI");
    printPin("TFT_MISO", DISPLAY_MISO_PIN, "ILI9488 SPI MISO");
    printPin("TFT_CS", DISPLAY_CS_PIN, "ILI9488 chip select");
    printPin("TFT_DC", DISPLAY_DC_PIN, "ILI9488 data/command");
    printPin("TFT_RST", DISPLAY_RST_PIN, "ILI9488 reset");

    Serial.println("[HEALTH] ---------------------------------");
}
