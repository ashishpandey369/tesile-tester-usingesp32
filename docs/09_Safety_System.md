# Safety System

# GoldTester
## Version 1.0

---

# Objective

Protect the machine, operator, and test specimen during operation.

---

# Safety Features

- Maximum Force Protection
- Emergency Stop
- Motor Timeout
- Load Cell Error Detection
- HX711 Communication Check
- Power-On Self Test
- Watchdog Protection

---

# Safety Actions

| Condition | Action |
|-----------|--------|
| Target Force Reached | Stop Motor |
| Maximum Force Exceeded | Emergency Stop |
| HX711 Failure | Stop Motor & Show Error |
| Motor Driver Failure | Disable Motor |
| Power Failure | Safe Shutdown |
| Emergency Stop Pressed | Immediate Stop |

---

# Error Messages

- HX711 Not Detected
- Load Cell Error
- Motor Driver Error
- Force Limit Exceeded
- System Fault

---

# Safety Checklist

☐ Emergency Stop Working

☐ Force Limit Enabled

☐ Motor Stops Correctly

☐ Error Messages Displayed

☐ System Recovers Safely

---

# Summary

The GoldTester Safety System continuously monitors the machine and immediately stops operation if an unsafe condition is detected, ensuring reliable and safe tensile testing.