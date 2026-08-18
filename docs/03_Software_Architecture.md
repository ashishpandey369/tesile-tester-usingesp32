# Software Architecture

# GoldTester
## Precision Gold Chain Tensile Testing Machine

Version : 1.0

---

# 1. Overview

The GoldTester software has been designed using Object-Oriented Programming (OOP) principles with a modular architecture.

Each hardware component is controlled by its own dedicated software module. The application is divided into independent managers, allowing easy maintenance, debugging, testing, and future expansion.

The software follows a layered architecture separating Hardware, Logic, User Interface, and Safety.

---

# 2. Software Design Goals

The software architecture has been designed with the following objectives:

• Modular Design

• Object-Oriented Programming

• Easy Maintenance

• High Readability

• Hardware Abstraction

• Smooth Real-Time Operation

• Safe Machine Control

• Easy Future Expansion

---

# 3. Software Layer Architecture

```
+-----------------------------------------------------------+
|                    User Interface Layer                   |
|               Display • Menu • User Input                |
+-----------------------------------------------------------+

+-----------------------------------------------------------+
|                 Machine Control Layer                     |
|     State Machine • Force Control • Safety Logic         |
+-----------------------------------------------------------+

+-----------------------------------------------------------+
|                 Hardware Abstraction Layer                |
| Display | HX711 | Motor | Buzzer | EEPROM | RTC          |
+-----------------------------------------------------------+

+-----------------------------------------------------------+
|                     ESP32 Hardware                        |
+-----------------------------------------------------------+
```

---

# 4. Project Structure

```
Stress

include/

src/

docs/

platformio.ini

README.md
```

---

# 5. Header Files

## pins.h

Purpose

Stores all GPIO definitions.

Responsibilities

• GPIO Assignment

• Hardware Pin Mapping

---

## config.h

Purpose

Stores machine configuration values.

Examples

Maximum Force

Motor Speed

Display Refresh Rate

Calibration Factor

Timeout Values

---

## display.h

Purpose

Controls the TFT Display.

Responsibilities

• Boot Screen

• Home Screen

• Running Screen

• Error Screen

• Calibration Screen

• Live Force Display

---

## loadcell.h

Purpose

Communicates with HX711.

Responsibilities

• Read Raw Values

• Average Filter

• Calibration

• Zero Offset

• Force Conversion

---

## motor.h

Purpose

Controls the Stepper Driver.

Responsibilities

• Forward Motion

• Reverse Motion

• Speed Control

• Acceleration

• Stop

• Home Position

---

## buzzer.h

Purpose

Controls the buzzer.

Responsibilities

• Startup Beep

• Target Reached

• Error Alarm

• Warning

---

## safety.h

Purpose

Protects the machine.

Responsibilities

• Emergency Stop

• Maximum Force

• Timeout

• Sensor Failure

• Motor Failure

---

## machine.h

Purpose

Main machine controller.

Responsibilities

• State Machine

• Coordinate all modules

• Force Logic

• Test Cycle

---

## ui.h

Purpose

User Interface Manager.

Responsibilities

• Screen Navigation

• Future Button Handling

• Settings

• Menu

---

# 6. Source Files

display.cpp

↓

Display Driver

----------------------------

loadcell.cpp

↓

HX711 Driver

----------------------------

motor.cpp

↓

Stepper Driver

----------------------------

machine.cpp

↓

Machine Logic

----------------------------

safety.cpp

↓

Safety Logic

----------------------------

ui.cpp

↓

User Interface

----------------------------

main.cpp

↓

System Entry Point

---

# 7. Class Diagram

```
                   +----------------+
                   |     main()     |
                   +-------+--------+
                           |
                           |
        -----------------------------------------
        |         |          |        |          |
        |         |          |        |          |
 DisplayManager  LoadCell  Motor   Safety   Buzzer
        |         |          |        |          |
        -------------------------------
                       |
                       |
               MachineController
```

---

# 8. Software Execution Flow

```
Power ON

↓

Initialize Hardware

↓

Display Boot Screen

↓

Initialize HX711

↓

Initialize Motor

↓

Initialize Safety

↓

Initialize Buzzer

↓

Ready State

↓

Wait for User

↓

Start Test

↓

Read Force

↓

Calculate Force

↓

Adjust Motor Speed

↓

Target Reached

↓

Stop Motor

↓

Display Result

↓

Return Home

↓

Ready
```

---

# 9. Machine State Diagram

```
BOOT

↓

INITIALIZING

↓

READY

↓

PRELOAD

↓

RUNNING

↓

TARGET_REACHED

↓

RETURN_HOME

↓

COMPLETED

↓

READY

↓

ERROR
```

---

# 10. Module Communication

```
Display

        ↑

Machine Controller

        ↑

Load Cell

        ↑

HX711

-------------------------

Motor

        ↑

Machine Controller

-------------------------

Safety

        ↑

Machine Controller

-------------------------

Buzzer

        ↑

Machine Controller
```

---

# 11. Main Loop

```
loop()

↓

Read Sensors

↓

Update Force

↓

Safety Check

↓

Update Motor

↓

Update Display

↓

Repeat
```

---

# 12. Force Control Logic

```
Read Load Cell

↓

Average Filter

↓

Convert to kg

↓

Compare Target

↓

Calculate Error

↓

Motor Speed

↓

Near Target

↓

Reduce Speed

↓

Micro Movement

↓

Target Reached

↓

Stop Motor
```

---

# 13. Display Refresh Strategy

The display shall not be continuously cleared.

Instead,

• Draw static interface once

• Update only changing values

• Refresh force display

• Refresh progress bar

• Refresh status only when changed

This minimizes flickering and improves performance.

---

# 14. Error Handling

The software shall detect

• HX711 Failure

• Load Cell Disconnected

• Display Failure

• Motor Stall

• Emergency Stop

• Maximum Force Exceeded

• Power Failure

---

# 15. Future Software Modules

Future modules may include

• Bluetooth Manager

• WiFi Manager

• OTA Manager

• SD Card Manager

• USB Manager

• Cloud Manager

• Graph Plotter

• Report Generator

• Touch Manager

---

# 16. Coding Standards

The project follows modern C++ practices.

Rules

• Object-Oriented Programming

• Single Responsibility Principle

• Modular Design

• No Magic Numbers

• Meaningful Variable Names

• Hardware Abstraction

• Separate UI and Logic

• Separate Configuration

• Reusable Functions

• Consistent Formatting

---

# 17. Memory Organization

Flash Memory

↓

Program Code

↓

Fonts

↓

Images

--------------------------

RAM

↓

Display Buffer

↓

Force Values

↓

Motor Variables

↓

Machine State

↓

Runtime Data

---

# 18. Future Expansion

The software has been designed for future upgrades including

• Touch Screen Interface

• WiFi Dashboard

• Bluetooth Remote Control

• Automatic Calibration

• Multiple Test Profiles

• Data Logging

• Cloud Storage

• Mobile Application

• Production Firmware

---

# 19. Software Summary

The software architecture has been designed to provide

✓ Modular Design

✓ Easy Maintenance

✓ High Performance

✓ Stable Operation

✓ Expandability

✓ Industrial Machine Structure

✓ Professional Code Organization

✓ Long-Term Maintainability

This architecture provides a scalable foundation for implementing advanced force control, calibration, safety, and production-level features while maintaining clean and reusable code.