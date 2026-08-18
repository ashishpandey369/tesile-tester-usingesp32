# Display User Interface (UI)

# GoldTester
## Precision Gold Chain Tensile Testing Machine

Version : 1.0

---

# 1. Overview

The 3.5-inch ILI9488 TFT display serves as the primary Human Machine Interface (HMI) of the GoldTester.

Its purpose is to provide a clean, easy-to-read interface showing machine status, applied force, target force, motor state, calibration information, and system messages.

The interface is designed for industrial use with minimal flickering and high readability.

---

# 2. Design Objectives

The UI shall provide

• Large force display

• Clear machine status

• Simple layout

• Smooth updates

• No unnecessary animations

• Minimal display flickering

• Easy readability from a distance

• Fast screen refresh

---

# 3. Display Specifications

Controller

ILI9488

Resolution

480 × 320 Pixels

Interface

SPI

Orientation

Landscape

Color Depth

16-bit RGB565

---

# 4. UI Design Principles

The display shall follow these rules

✓ Large Fonts

✓ High Contrast

✓ Industrial Appearance

✓ Static Background

✓ Update Only Dynamic Data

✓ No Screen Flashing

✓ Consistent Layout

---

# 5. Boot Screen

Displayed immediately after power-on.

```
################################################

               GOLD TESTER

      Precision Tensile Testing Machine

                Version 1.0

          Initializing Hardware...

################################################
```

Hardware Initialization Sequence

✓ Display

↓

✓ HX711

↓

✓ Stepper Driver

↓

✓ Safety System

↓

READY

---

# 6. Home Screen

Displayed after initialization.

```
+------------------------------------------------+

              GOLD TESTER

-----------------------------------------------

Current Force

              0.000 kg

-----------------------------------------------

Target Force

              5.000 kg

-----------------------------------------------

Machine

READY

-----------------------------------------------

Motor

STOPPED

+------------------------------------------------+
```

---

# 7. Running Screen

```
+------------------------------------------------+

              GOLD TESTER

-----------------------------------------------

Current Force

              2.345 kg

-----------------------------------------------

Target Force

              5.000 kg

-----------------------------------------------

Motor

FORWARD

-----------------------------------------------

Status

RUNNING

-----------------------------------------------

Progress

██████████░░░░░░░░░░░░

+------------------------------------------------+
```

---

# 8. Precision Mode Screen

Displayed when approaching target force.

```
+------------------------------------------------+

Current Force

4.985 kg

-----------------------------------------------

Target Force

5.000 kg

-----------------------------------------------

Motor

SLOW

-----------------------------------------------

Status

PRECISION MODE

-----------------------------------------------

Remaining

0.015 kg

+------------------------------------------------+
```

---

# 9. Target Reached Screen

```
+------------------------------------------------+

        TARGET REACHED

-----------------------------------------------

Applied Force

5.000 kg

-----------------------------------------------

Motor

STOPPED

-----------------------------------------------

Status

SUCCESS

-----------------------------------------------

Returning Home...

+------------------------------------------------+
```

---

# 10. Completed Screen

```
+------------------------------------------------+

      TEST COMPLETED

-----------------------------------------------

Maximum Force

5.003 kg

-----------------------------------------------

Duration

18 Seconds

-----------------------------------------------

Status

READY

+------------------------------------------------+
```

---

# 11. Calibration Screen

```
+------------------------------------------------+

      CALIBRATION MODE

-----------------------------------------------

Current Reading

0.002 kg

-----------------------------------------------

Offset

8456213

-----------------------------------------------

Calibration Factor

-7050.00

-----------------------------------------------

Status

WAITING

+------------------------------------------------+
```

---

# 12. Error Screen

```
################################################

            ERROR

HX711 NOT DETECTED

Please Check Wiring

################################################
```

Possible Errors

HX711 Failure

Stepper Driver Failure

Load Cell Failure

Display Error

Calibration Error

Emergency Stop

Maximum Force

Power Failure

---

# 13. Emergency Screen

```
################################################

      EMERGENCY STOP

Motor Disabled

Please Reset Machine

################################################
```

---

# 14. Future Settings Screen

```
+------------------------------------------------+

SETTINGS

Maximum Force

Target Force

Motor Speed

Brightness

Calibration

About

+------------------------------------------------+
```

---

# 15. Future Graph Screen

```
Force

20kg |

     |

15kg |

     |

10kg |

     |

5kg  |______________________

      Time
```

Future Version

Real-Time Force Graph

---

# 16. Screen Update Strategy

The display shall never be cleared continuously.

Static Elements

Draw Once

• Title

• Labels

• Boxes

• Lines

Dynamic Elements

Update Only

• Current Force

• Target Force

• Status

• Progress

• Motor Direction

This minimizes flickering and improves performance.

---

# 17. Refresh Rates

| Item | Refresh Rate |
|-------|--------------|
| Force Value | 100 ms |
| Progress Bar | 100 ms |
| Status | On Change |
| Target Force | On Change |
| Error Messages | Immediate |

---

# 18. Color Theme

| Item | Color |
|-------|-------|
| Background | Black |
| Titles | Gold |
| Labels | White |
| Force Value | Cyan |
| Target Value | Green |
| Running | Green |
| Precision | Yellow |
| Warning | Orange |
| Error | Red |
| Completed | Blue |

---

# 19. Font Strategy

Title

Large Bold

Current Force

Extra Large

Labels

Medium

Status

Medium Bold

Errors

Extra Large

---

# 20. Future UI Features

Touch Support

Dark Mode

Light Mode

Custom Themes

Animated Progress

Graphs

History Screen

USB Export

Bluetooth Dashboard

WiFi Dashboard

Cloud Synchronization

---

# 21. Display Performance Goals

Display Initialization

< 1 Second

Force Update

100 ms

Screen Flicker

None

User Readability

Excellent

CPU Usage

Low

---

# 22. UI Summary

The GoldTester user interface has been designed as a professional industrial Human Machine Interface (HMI).

The interface emphasizes clarity, responsiveness, and stability while minimizing display flickering.

The modular screen layout allows future expansion without major firmware changes and provides operators with all critical machine information in a simple, easy-to-read format.

The UI architecture supports future additions such as touch control, graphical force plots, settings menus, data export, and wireless connectivity while maintaining the same core design philosophy.