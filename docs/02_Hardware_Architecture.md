# Hardware Architecture

# GoldTester
## Precision Gold Chain Tensile Testing Machine

Version : 1.0

---

# 1. Overview

The GoldTester hardware is designed to accurately measure the tensile force applied to gold chains while providing precise motor control, real-time visualization, and safety mechanisms.

The system consists of a central ESP32 microcontroller connected to a load cell amplifier (HX711), a stepper motor driver (HW-134A), a NEMA17 stepper motor, a 3.5-inch SPI TFT display, and supporting peripherals.

The hardware has been selected to provide high precision, stable operation, and future expandability.

---

# 2. Hardware Block Diagram

```
                        +----------------------+
                        |      ESP32 DevKit    |
                        +----------+-----------+
                                   |
      ----------------------------------------------------------
      |             |               |              |            |
      |             |               |              |            |
  HX711         TFT Display     HW-134A        Buzzer      Future Expansion
      |             |               |
  Load Cell     ILI9488         NEMA17 Motor
```

---

# 3. System Components

## 3.1 ESP32 DevKit V1

Purpose

The ESP32 acts as the central controller of the entire machine.

Responsibilities

• Read load cell values
• Control stepper motor
• Update TFT display
• Perform force calculations
• Execute safety logic
• Run machine state controller
• Future Bluetooth/WiFi support

Specifications

• Dual Core 240MHz
• 520KB SRAM
• 4MB Flash
• WiFi
• Bluetooth
• Hardware SPI
• Hardware Timers

---

## 3.2 Load Cell

Purpose

Measure the tensile force applied to the gold chain.

Specifications

Type
Strain Gauge Load Cell

Capacity
20 kg

Output
mV/V

Features

• High Accuracy
• Low Drift
• Suitable for Tensile Testing

---

## 3.3 HX711

Purpose

Converts the tiny analog signal from the load cell into digital values readable by the ESP32.

Specifications

Resolution
24-bit ADC

Interface
Two-wire Serial

Pins Used

DT

GPIO4

SCK

GPIO5

Advantages

• High Resolution
• Low Noise
• Easy Calibration

---

## 3.4 Stepper Driver (HW-134A)

Purpose

Controls the NEMA17 stepper motor.

Features

• STEP/DIR Interface
• Adjustable Current
• Microstepping Support
• External Motor Supply

Motor Supply

24V

Logic Supply

3.3V

Protection

100uF capacitor across VMOT and GND

---

## 3.5 NEMA17 Stepper Motor

Purpose

Applies controlled tensile force to the gold chain.

Specifications

Type

Bipolar Stepper

Steps

200 Steps/Rev

Control

Microstepping

Advantages

• High Precision
• Excellent Repeatability
• Smooth Motion

---

## 3.6 TFT Display

Display Controller

ILI9488

Size

3.5 Inch

Resolution

480 × 320

Interface

SPI

Purpose

• Show live force
• Show target force
• Show machine status
• Show menus
• Calibration interface
• Error messages

---

## 3.7 Buzzer

Purpose

Audio feedback.

Used For

• Boot Complete
• Target Reached
• Calibration Complete
• Error Alarm
• Emergency Stop

---

# 4. Power Architecture

24V Power Supply

↓

HW-134A

↓

Stepper Motor

------------------------------------

5V Supply

↓

ESP32

↓

Display

------------------------------------

3.3V

↓

HX711

↓

Logic Devices

---

# 5. Final Pin Mapping

## HX711

| HX711 | ESP32 |
|--------|--------|
| VCC | 3.3V |
| GND | GND |
| DT | GPIO4 |
| SCK | GPIO5 |

---

## Buzzer

| Device | ESP32 |
|----------|--------|
| Buzzer | GPIO14 |

---

## HW-134A

| Driver Pin | ESP32 |
|------------|--------|
| STEP | GPIO25 |
| DIR | GPIO17 |
| ENABLE | GPIO16 |
| VDD | 3.3V |
| GND | GND |
| RST | 3.3V |
| SLP | 3.3V |
| MS1 | GND |
| MS2 | GND |
| MS3 | GND |

Motor Supply

VMOT → 24V

GND → 24V Ground

Motor

1A → Coil A+

1B → Coil A-

2A → Coil B+

2B → Coil B-

Protection

100uF Capacitor across VMOT and GND

---

## TFT Display

| Display Pin | ESP32 |
|--------------|--------|
| VCC | 5V |
| GND | GND |
| SCK | GPIO18 |
| MOSI | GPIO23 |
| MISO | GPIO19 |
| CS | GPIO27 |
| DC | GPIO32 |
| RST | GPIO33 |
| LED | 5V |

---

# 6. Communication Interfaces

| Device | Interface |
|----------|-----------|
| HX711 | Serial Two-Wire |
| TFT Display | SPI |
| HW-134A | STEP/DIR |
| Buzzer | Digital Output |

---

# 7. Hardware Design Considerations

## Load Cell

• Keep away from vibration.
• Rigid mounting.
• Zero calibration before use.

---

## Display

• Use Hardware SPI.
• Keep SPI wires short.
• Avoid routing beside motor wires.
• Use common ground.

---

## Stepper Driver

• Use 24V motor supply.
• Adjust current properly.
• Install cooling if required.

---

## Power

• Separate motor power from logic power.
• Use common ground.
• Add adequate decoupling capacitors.

---

# 8. Future Hardware Expansion

The hardware architecture supports future upgrades.

Possible additions

• Touch Interface

• SD Card

• Bluetooth

• WiFi Dashboard

• USB Data Export

• RTC Module

• Thermal Printer

• Battery Backup

• Force Graph Logging

• Automatic Calibration

---

# 9. Hardware Summary

| Component | Purpose |
|------------|---------|
| ESP32 | Main Controller |
| Load Cell | Force Measurement |
| HX711 | ADC |
| HW-134A | Stepper Driver |
| NEMA17 | Tensile Force |
| TFT Display | User Interface |
| Buzzer | Audio Feedback |

---

# 10. Conclusion

The selected hardware provides a reliable and expandable platform for the GoldTester tensile testing machine.

The architecture is designed to achieve:

• High measurement accuracy

• Smooth motor control

• Stable display performance

• Reliable safety mechanisms

• Modular software integration

• Easy future expansion

This hardware platform forms the foundation for implementing closed-loop force control, calibration routines, user interaction, and production-grade testing features.