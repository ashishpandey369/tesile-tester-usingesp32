#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

//====================================================
// Project Information
//====================================================

#define PROJECT_NAME "GoldTester"
#define PROJECT_VERSION "2.2.0"

//====================================================
// Display Configuration
//====================================================

constexpr uint16_t SCREEN_WIDTH = 480;
constexpr uint16_t SCREEN_HEIGHT = 320;
constexpr uint16_t DISPLAY_REFRESH_MS = 50;
constexpr uint8_t DISPLAY_ROTATION = 3;

//====================================================
// Virtual Test Value
//====================================================
// No physical load cell is used in this version.
// Current force is a software test-point value that increases
// only while the toggle switch is ON and the motor is running.

constexpr float INITIAL_CURRENT_FORCE = 0.000f;
constexpr float FORCE_PER_MOTOR_STEP_KG = 0.001f;
constexpr float MAX_VIRTUAL_FORCE_KG = 99.999f;

//====================================================
// Motor Configuration
//====================================================

constexpr float MOTOR_MAX_SPEED = 2000.0f;
constexpr float MOTOR_NORMAL_SPEED = 900.0f;
constexpr float MOTOR_RETURN_SPEED = 800.0f;
constexpr float MOTOR_ACCELERATION = 1000.0f;

// Manual movement
constexpr long MANUAL_STEP_STEPS = 100;
constexpr float MANUAL_HOLD_SPEED = 900.0f;

//====================================================
// Mode / Button Configuration
//====================================================

constexpr uint16_t BUTTON_LONG_PRESS_MS = 2000;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 30;

// Hold either UP or DOWN while turning the toggle ON to change mode.

//====================================================
// Machine Timing
//====================================================

constexpr uint16_t BOOT_SCREEN_TIME = 1500;

//====================================================
// Safety
//====================================================

constexpr uint32_t MACHINE_TIMEOUT = 300000UL;

//====================================================
// Buzzer
//====================================================

constexpr uint16_t BUZZER_DEFAULT_FREQ = 2500;
constexpr uint16_t BUZZER_DEFAULT_TIME = 100;

//====================================================
// Debug
//====================================================

constexpr bool SERIAL_DEBUG = true;
constexpr uint32_t SERIAL_BAUDRATE = 115200;

#endif