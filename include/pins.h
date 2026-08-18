#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

//====================================================
// HX711 Load Cell
//====================================================

constexpr uint8_t HX711_DT_PIN = 4;
constexpr uint8_t HX711_SCK_PIN = 5;

//====================================================
// Buzzer
//====================================================

constexpr uint8_t BUZZER_PIN = 14;

//====================================================
// User Inputs
//====================================================

// Push Button (+100g)
constexpr uint8_t BUTTON_UP_PIN = 21; // 34;

// Push Button (-100g)
constexpr uint8_t BUTTON_DOWN_PIN = 22; // 35;

// Start / Stop Toggle Switch
constexpr uint8_t START_SWITCH_PIN = 13;

//====================================================
// Stepper Driver (HW-134A)
//====================================================

constexpr uint8_t STEP_PIN = 25;
constexpr uint8_t DIR_PIN = 17;
constexpr uint8_t ENABLE_PIN = 16;

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

constexpr bool MOTOR_ENABLE = LOW;
constexpr bool MOTOR_DISABLE = HIGH;

constexpr bool SWITCH_ON = LOW;
constexpr bool SWITCH_OFF = HIGH;

constexpr bool BUTTON_PRESSED = LOW;
constexpr bool BUTTON_RELEASED = HIGH;

#endif