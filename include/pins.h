#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

//====================================================
// Legacy HX711 pins
//====================================================
// Kept documented for reference only. The current tester
// uses a software force value and does not use an HX711.
constexpr uint8_t HX711_DT_PIN = 4;
constexpr uint8_t HX711_SCK_PIN = 5;

//====================================================
// Buzzer
//====================================================

constexpr uint8_t BUZZER_PIN = 14;

//====================================================
// User Inputs
//====================================================

constexpr uint8_t BUTTON_UP_PIN = 22;
constexpr uint8_t BUTTON_DOWN_PIN = 21;
constexpr uint8_t RESET_MODE_BUTTON_PIN = 26;
constexpr uint8_t START_SWITCH_PIN = 13;

//====================================================
// BTS7960 43A H-Bridge + 12V DC Gear Motor
//====================================================
// RPWM/LPWM control motor direction and speed.
// R_EN/L_EN enable the two BTS7960 half-bridges.
//
// GPIO4 is no longer used by an HX711 in the active firmware,
// so it is repurposed for L_EN.
constexpr uint8_t BTS7960_RPWM_PIN = 25;
constexpr uint8_t BTS7960_LPWM_PIN = 17;
constexpr uint8_t BTS7960_R_EN_PIN = 16;
constexpr uint8_t BTS7960_L_EN_PIN = 4;

//====================================================
// ILI9488 Display (Hardware SPI)
//====================================================

constexpr uint8_t DISPLAY_SCLK_PIN = 18;
constexpr uint8_t DISPLAY_MOSI_PIN = 23;
constexpr uint8_t DISPLAY_MISO_PIN = 19;

constexpr uint8_t DISPLAY_CS_PIN = 27;
constexpr uint8_t DISPLAY_DC_PIN = 32;
constexpr uint8_t DISPLAY_RST_PIN = 33;

//====================================================
// Logic Levels
//====================================================

constexpr bool MOTOR_ENABLE = HIGH;
constexpr bool MOTOR_DISABLE = LOW;

constexpr bool SWITCH_ON = LOW;
constexpr bool SWITCH_OFF = HIGH;

constexpr bool BUTTON_PRESSED = LOW;
constexpr bool BUTTON_RELEASED = HIGH;

#endif
