# GoldTester

> **Precision Gold Chain Tensile Testing Machine**

GoldTester is an ESP32-based tensile testing machine designed to accurately measure the tensile force applied to gold chains and similar precision components. The system combines a high-resolution load cell, stepper motor control, and a graphical TFT display to provide reliable, repeatable, and safe force testing.

---

# Features

- High-precision force measurement using HX711
- Closed-loop force control
- NEMA17 stepper motor control
- HW-134A stepper driver support
- 3.5" ILI9488 TFT display
- Industrial-style user interface
- Automatic target force stopping
- Smooth motor acceleration and deceleration
- Calibration support
- Safety monitoring
- Modular Object-Oriented Architecture
- PlatformIO based development

---

# Hardware Used

| Component | Model |
|-----------|-------|
| Controller | ESP32 DevKit V1 |
| Display | 3.5" SPI TFT (ILI9488) |
| Load Cell | 20kg |
| ADC | HX711 |
| Stepper Driver | HW-134A |
| Motor | NEMA17 |
| Buzzer | 5V Active Buzzer |
| Power Supply | 24V DC |

---

# Final GPIO Mapping

## HX711

| Signal | GPIO |
|---------|------|
| DT | GPIO4 |
| SCK | GPIO5 |

---

## TFT Display

| Signal | GPIO |
|---------|------|
| SCK | GPIO18 |
| MOSI | GPIO23 |
| MISO | GPIO19 |
| CS | GPIO27 |
| DC | GPIO32 |
| RST | GPIO33 |

---

## HW-134A

| Signal | GPIO |
|---------|------|
| STEP | GPIO25 |
| DIR | GPIO17 |
| EN | GPIO16 |

---

## Buzzer

| Signal | GPIO |
|---------|------|
| BUZZER | GPIO14 |

---

# Software Architecture

```
main()

│

├── MachineController

├── DisplayManager

├── LoadCellManager

├── MotorController

├── SafetyManager

└── BuzzerManager
```

---

# Folder Structure

```
Stress/

├── include/
├── src/
├── docs/
├── platformio.ini
└── README.md
```

---

# Documentation

Detailed documentation is available in the `docs/` folder.

- Project Overview
- Hardware Architecture
- Software Architecture
- Wiring Guide
- Force Control Algorithm
- Display UI
- Calibration
- Testing Procedure
- Safety System
- Future Improvements
- Project Roadmap
- Engineering Logbook
- TODO
- Changelog

---

# Current Development Status

- ✅ Project Planning
- ✅ Hardware Selection
- ✅ Wiring Finalized
- 🔄 Display Integration
- ⏳ HX711 Integration
- ⏳ Motor Control
- ⏳ Force Control
- ⏳ Calibration
- ⏳ Final Testing

---

# Future Features

- Touch Screen Support
- Bluetooth Connectivity
- Wi-Fi Dashboard
- SD Card Data Logging
- USB Export
- OTA Firmware Updates
- Mobile Application
- Force Graph Plotting
- Automatic Report Generation

---

# Development Environment

- PlatformIO
- Visual Studio Code
- ESP32 Arduino Framework
- C++ (Object-Oriented Programming)

---

# License

This project is developed for educational, research, and engineering purposes.

---

# Author

**Ashish Pandey**

Department of Electronics and Communication Engineering (ECE)

ACS College of Engineering

Bengaluru, India

---

# Version

**GoldTester v1.0**

**Status:** 🚧 Under Development