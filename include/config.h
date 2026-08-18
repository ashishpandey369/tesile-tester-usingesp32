#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

//====================================================
// Project Information
//====================================================

#define PROJECT_NAME "GoldTester"
#define PROJECT_VERSION "1.0.0"

//====================================================
// Display Configuration
//====================================================

constexpr uint16_t SCREEN_WIDTH = 480;
constexpr uint16_t SCREEN_HEIGHT = 320;

constexpr uint16_t DISPLAY_REFRESH_MS = 100;

//====================================================
// Load Cell Configuration
//====================================================

// Initial calibration factor (Update after calibration)
constexpr float LOADCELL_CALIBRATION_FACTOR = -7050.0f;

// Initial offset
constexpr long LOADCELL_OFFSET = 0;

// Moving average filter samples
constexpr uint8_t LOADCELL_FILTER_SAMPLES = 10;

// Read interval
constexpr uint16_t LOADCELL_READ_INTERVAL = 10; // ms

// Known reference weight used for the on-machine calibration routine
constexpr float CALIBRATION_WEIGHT_KG = 0.510f;

// Any filtered reading whose magnitude is below this is snapped to
// exactly 0.000 kg. Kills HX711 noise/jitter around zero.
constexpr float ZERO_DEADBAND_KG = 0.010f;

//====================================================
// Force Limits
//====================================================

constexpr float MIN_FORCE_KG = 0.100f;
constexpr float MAX_FORCE_KG = 20.000f;

constexpr float DEFAULT_TARGET_FORCE = 1.000f;
constexpr float TARGET_STEP = 0.100f;

// Acceptable tolerance
constexpr float TARGET_TOLERANCE = 0.010f;

// A test is called PASSED once the reading is within this much of target
constexpr float PASS_TOLERANCE_KG = 0.005f; // 5 grams

// A single-tick drop bigger than this while RUNNING (before target is
// reached) is treated as the sample breaking -> TEST FAILED
constexpr float FAIL_DROP_KG = 0.100f;

//====================================================
// Motor Configuration
//====================================================

// Maximum Speed
constexpr float MOTOR_MAX_SPEED = 2000.0f;

// Normal Running Speed
constexpr float MOTOR_NORMAL_SPEED = 1200.0f;

// Precision Speed
constexpr float MOTOR_PRECISION_SPEED = 250.0f;

// Return Speed
constexpr float MOTOR_RETURN_SPEED = 800.0f;

// Motor Acceleration
constexpr float MOTOR_ACCELERATION = 1000.0f;

//====================================================
// Display Refresh
//====================================================

constexpr uint16_t FORCE_REFRESH_TIME = 100;
constexpr uint16_t STATUS_REFRESH_TIME = 250;

//====================================================
// Button Configuration
//====================================================

// Long press detection
constexpr uint16_t BUTTON_LONG_PRESS_MS = 4000;

// Auto repeat
constexpr uint16_t BUTTON_REPEAT_MS = 150;

//====================================================
// Machine Timing
//====================================================

// Boot Screen
constexpr uint16_t BOOT_SCREEN_TIME = 1500;

// Result Screen
constexpr uint16_t RESULT_SCREEN_TIME = 3000;

//====================================================
// Safety
//====================================================

// Emergency timeout
constexpr uint32_t MACHINE_TIMEOUT = 300000UL; // 5 minutes

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