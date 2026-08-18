# Wiring Guide

# GoldTester
## Precision Gold Chain Tensile Testing Machine

Version : 1.0

---

# 1. Overview

This document describes the complete electrical wiring of the GoldTester tensile testing machine.

The objective of this guide is to provide a single reference for connecting all hardware components to the ESP32.

The wiring has been optimized for:

• Stable SPI communication
• Reliable HX711 readings
• Smooth stepper motor operation
• Future PCB design
• Easy maintenance

---

# 2. System Wiring Overview

```
                     +-------------------------+
                     |       ESP32 DevKit      |
                     +-----------+-------------+
                                 |
        -------------------------------------------------------
        |                 |                 |                 |
        |                 |                 |                 |
     HX711           TFT Display        HW-134A          Buzzer
        |                 |                 |
    Load Cell         ILI9488          NEMA17 Motor
```

---

# 3. ESP32 Pin Assignment

| GPIO | Function |
|-------|----------|
| GPIO4 | HX711 DT |
| GPIO5 | HX711 SCK |
| GPIO14 | Buzzer |
| GPIO16 | Motor Enable |
| GPIO17 | Motor Direction |
| GPIO18 | TFT SPI Clock |
| GPIO19 | TFT SPI MISO |
| GPIO23 | TFT SPI MOSI |
| GPIO25 | Motor STEP |
| GPIO27 | TFT Chip Select |
| GPIO32 | TFT Data/Command |
| GPIO33 | TFT Reset |

---

# 4. HX711 Wiring

## Purpose

Reads the force applied to the load cell.

---

| HX711 Pin | ESP32 |
|------------|--------|
| VCC | 3.3V |
| GND | GND |
| DT | GPIO4 |
| SCK | GPIO5 |

---

Connection Diagram

```
ESP32                     HX711

3.3V -------------------- VCC

GND --------------------- GND

GPIO4 ------------------- DT

GPIO5 ------------------- SCK
```

---

# 5. Load Cell Wiring

The Load Cell connects directly to the HX711.

Typical Wire Colors

| Wire | HX711 |
|------|--------|
| Red | E+ |
| Black | E- |
| White | A- |
| Green | A+ |

> Verify the wire colors with the datasheet supplied with your load cell, as manufacturers may use different color conventions.

---

# 6. HW-134A Wiring

Purpose

Controls the NEMA17 Stepper Motor.

---

## Logic Connections

| HW-134A | ESP32 |
|----------|--------|
| STEP | GPIO25 |
| DIR | GPIO17 |
| ENABLE | GPIO16 |
| VDD | 3.3V |
| GND | GND |

---

## Sleep & Reset

These pins remain permanently HIGH.

| Driver Pin | Connection |
|-------------|------------|
| RESET | 3.3V |
| SLEEP | 3.3V |

---

## Microstepping

Initial Configuration

| Pin | Connection |
|------|------------|
| MS1 | GND |
| MS2 | GND |
| MS3 | GND |

Full Step Mode

---

## Motor Supply

| Driver Pin | Connection |
|-------------|------------|
| VMOT | +24V |
| GND | 24V Ground |

---

## Capacitor

A 100µF electrolytic capacitor **must** be connected across

VMOT

and

GND

as close to the driver as possible.

```
        +24V
          |
          |
       +-----+
       |100uF|
       +-----+
          |
         GND
```

---

## Stepper Motor

| HW-134A | Motor |
|----------|-------|
| 1A | Coil A+ |
| 1B | Coil A- |
| 2A | Coil B+ |
| 2B | Coil B- |

---

# 7. TFT Display Wiring

Display

3.5"

SPI

ILI9488

480 × 320

---

## Connections

| TFT Pin | ESP32 |
|----------|--------|
| VCC | 5V |
| GND | GND |
| SCK | GPIO18 |
| MOSI | GPIO23 |
| MISO | GPIO19 |
| CS | GPIO27 |
| DC | GPIO32 |
| RESET | GPIO33 |
| LED | 5V |

---

Connection Diagram

```
ESP32                     TFT

5V ---------------------- VCC

GND --------------------- GND

GPIO18 ------------------ SCK

GPIO23 ------------------ MOSI

GPIO19 ------------------ MISO

GPIO27 ------------------ CS

GPIO32 ------------------ DC

GPIO33 ------------------ RESET

5V ---------------------- LED
```

Touch Controller

Not Used

All touch pins remain unconnected.

---

# 8. Buzzer Wiring

| Device | ESP32 |
|----------|--------|
| Buzzer | GPIO14 |

Connection

```
GPIO14 -------- Positive

GND ----------- Negative
```

---

# 9. Power Distribution

```
24V Power Supply

↓

Stepper Driver VMOT

↓

Stepper Motor

--------------------------------

5V Regulator

↓

ESP32 VIN

↓

Display VCC

↓

Display LED

--------------------------------

ESP32 3.3V

↓

HX711

↓

Driver Logic

↓

ESP32 GPIO Logic
```

---

# 10. Grounding

All grounds **must** be connected together.

```
24V GND

↓

ESP32 GND

↓

HX711 GND

↓

Display GND

↓

Driver Logic GND
```

Common Ground is mandatory.

---

# 11. Recommended Wiring Practices

• Keep SPI wires as short as possible.

• Route motor wires separately from SPI wires.

• Twist motor coil wires together.

• Use a common ground.

• Secure all connectors mechanically.

• Avoid loose jumper wires in the final machine.

• Use ferrules or proper crimp connectors where possible.

---

# 12. Cable Management

Recommended

• Bundle display wires separately.

• Bundle motor wires separately.

• Keep load cell wires away from motor power wiring.

• Use cable ties.

• Label all connectors.

---

# 13. Hardware Verification Checklist

☐ ESP32 Powered

☐ HX711 Connected

☐ Load Cell Connected

☐ Display Connected

☐ Display Backlight ON

☐ Stepper Driver Connected

☐ Motor Connected

☐ 24V Supply Connected

☐ Common Ground Connected

☐ Capacitor Installed

☐ Buzzer Connected

---

# 14. Wiring Summary

| Device | Interface |
|----------|-----------|
| HX711 | Digital Serial |
| TFT | Hardware SPI |
| HW-134A | STEP/DIR |
| Buzzer | Digital GPIO |

---

# 15. Final Wiring Map

```
ESP32

GPIO4  ---------- HX711 DT

GPIO5  ---------- HX711 SCK

GPIO14 ---------- Buzzer

GPIO16 ---------- HW134A ENABLE

GPIO17 ---------- HW134A DIR

GPIO18 ---------- TFT SCK

GPIO19 ---------- TFT MISO

GPIO23 ---------- TFT MOSI

GPIO25 ---------- HW134A STEP

GPIO27 ---------- TFT CS

GPIO32 ---------- TFT DC

GPIO33 ---------- TFT RESET

3.3V ------------ HX711 VCC

5V -------------- TFT VCC

5V -------------- TFT LED

24V ------------- HW134A VMOT

GND ------------- Common Ground
```

---

# 16. Conclusion

The wiring architecture has been optimized for reliable communication, accurate force measurement, smooth motor control, and future migration to a custom PCB.

The pin assignment minimizes conflicts, uses the ESP32 hardware SPI interface for the display, and provides a clean separation between logic-level signals and high-power motor circuitry.

This wiring layout will remain fixed throughout the development of GoldTester Version 1.0.