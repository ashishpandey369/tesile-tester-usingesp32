# ESP32 Tensile Tester

**Project for Design Roots Company**

An ESP32-based tensile/push testing machine using a motor-driven mechanism, ILI9488 display, manual jog buttons and a master ON/OFF toggle switch.

## Current Version

**v2.0.0 — Motion-Controlled Tester**

The physical load cell and HX711 have been removed from the active machine architecture.

## Controls

- **UP button:** one manual step; hold for continuous UP movement.
- **DOWN button:** one manual step; hold for continuous DOWN movement.
- **UP + DOWN long press:** change between `TENSILE` and `PUSH` mode.
- **START/STOP toggle:** master machine control. ON starts automatic motion; OFF stops the motor immediately.

## Modes

### TENSILE
Automatic toggle-controlled motion runs the motor forward for the pull/tensile test.

### PUSH
Automatic toggle-controlled motion runs the motor backward for the push test.

## Current Force Display

`Current Force` is now a **software test-point value**, not a physical load measurement. It starts at `00.000 kg`, does not change during manual positioning, and increases from motor movement only while the master toggle is ON.

When the toggle is turned OFF, the last value remains visible. The next UP/DOWN interaction resets the value to `00.000 kg` for the next test cycle.

## Display Status

- `READY` — initial powered state.
- `RUNNING` — toggle is ON and automatic motion is active.
- `STOP` — toggle is OFF after operation.

## Hardware

- ESP32 DevKit V1
- ILI9488 480×320 SPI TFT
- HW-134A stepper driver
- NEMA17 / compatible stepper actuator
- Buzzer
- UP button
- DOWN button
- START/STOP toggle switch

## Pin Summary

| Function | GPIO |
|---|---:|
| UP | 21 |
| DOWN | 22 |
| START/STOP | 13 |
| Buzzer | 14 |
| STEP | 25 |
| DIR | 17 |
| ENABLE | 16 |
| TFT SCLK | 18 |
| TFT MOSI | 23 |
| TFT MISO | 19 |
| TFT CS | 27 |
| TFT DC | 32 |
| TFT RST | 33 |

## Safety

The START/STOP toggle is treated as the master stop control. Turning it OFF disables motor operation immediately in software. A physical emergency-stop circuit should still be used for a production machine.

## Build

PlatformIO / Arduino framework:

```bash
pio run
pio run -t upload
```
