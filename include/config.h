#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

//====================================================
// Project Information
//====================================================

#define PROJECT_NAME "GoldTester"
#define PROJECT_VERSION "3.0.0"

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
// The force value is a software demonstration value.
// While an automatic test is running, force increases by
// 12 grams for every elapsed millisecond (0.012 kg/ms).
//
// This is NOT a physical force measurement. A real tensile
// tester needs a load cell + HX711 for measured force.

constexpr float INITIAL_CURRENT_FORCE = 0.000f;
constexpr float FORCE_INCREASE_PER_MS_KG = 0.012f; // 12 g/ms
constexpr float MAX_VIRTUAL_FORCE_KG = 99.999f;

//====================================================
// BTS7960 + 12V DC Gear Motor Configuration
//====================================================

// Speed is expressed as a PWM percentage (0-100%).
// 85% provides sufficient starting torque for the motor/load
// based on hardware testing at 60% PWM.
constexpr float MOTOR_NORMAL_SPEED_PERCENT = 85.0f;
constexpr float MOTOR_RETURN_SPEED_PERCENT = 85.0f;

// Manual movement
// A short UP/DOWN press runs the DC motor for this duration.
constexpr uint16_t MANUAL_STEP_TIME_MS = 150;
constexpr float MANUAL_STEP_SPEED_PERCENT = 85.0f;
constexpr float MANUAL_HOLD_SPEED_PERCENT = 85.0f;

//====================================================
// Mode / Button Configuration
//====================================================

constexpr uint16_t BUTTON_LONG_PRESS_MS = 1000;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 30;
constexpr uint16_t MODE_CHANGE_WINDOW_MS = 600;

// UP/DOWN select a mode only while the master toggle is ON.
// The dedicated RESET/MODE button toggles the mode while force is zero
// and the master toggle is OFF.

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
