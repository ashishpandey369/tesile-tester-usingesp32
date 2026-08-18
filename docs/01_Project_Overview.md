# ESP32 Tensile Tester

**Project for Design Roots Company**

Version: 2.0.0
Platform: ESP32 DevKit V1 / PlatformIO / C++

## Project Objective

Build a motor-controlled machine that can perform two motion-based test modes:

- **TENSILE / PULL:** automatic forward movement.
- **PUSH:** automatic backward movement.

The machine also provides manual motor positioning through UP and DOWN buttons and a 480×320 ILI9488 display.

## Major Architecture Change — v2.0.0

The physical load cell and HX711 have been removed completely from the active machine.

The displayed `Current Force` value is now a software test-point value derived from motor steps during automatic toggle-controlled operation. It is not a physical force measurement.

## Operating Workflow

Power ON

↓

Hardware Initialization

↓

Display Boot Screen

↓

READY / Current Force = 00.000

↓

Manual positioning with UP / DOWN

↓

Toggle ON

↓

RUNNING

↓

Automatic motion according to selected mode

↓

Current Force software value increases

↓

Toggle OFF

↓

STOP / Motor disabled

↓

Next UP or DOWN interaction resets Current Force to 00.000

↓

Manual positioning again

## Controls

### UP

- Short press: one fixed motor step upward.
- Long press: continuous upward movement until release.
- Manual movement does not change Current Force.

### DOWN

- Short press: one fixed motor step downward.
- Long press: continuous downward movement until release.
- Manual movement does not change Current Force.

### Mode Change

Hold **UP + DOWN together** for the configured long-press duration while the master toggle is OFF.

The mode alternates:

`TENSILE ↔ PUSH`

### START/STOP Toggle

The toggle is the master machine control.

- ON → automatic motor motion starts according to the selected mode.
- OFF → motor stops immediately and the machine shows `STOP`.

## Machine Status

- `READY` — first powered state.
- `RUNNING` — automatic toggle-controlled motion active.
- `STOP` — toggle OFF after operation.

## Hardware

| Component | Function |
|---|---|
| ESP32 DevKit V1 | Main controller |
| ILI9488 480×320 TFT | User interface |
| HW-134A | Stepper driver |
| NEMA17 / compatible actuator | Mechanical movement |
| Buzzer | Machine feedback |
| UP button | Manual UP |
| DOWN button | Manual DOWN |
| Toggle switch | Master ON/OFF |

## Pin Mapping

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

## Display

The display shows:

- Current Force software value
- Current mode
- Motor direction/state
- Machine status
- Manual-control guidance

Display rotation is configured through `DISPLAY_ROTATION` in `config.h`.

## Safety

The master toggle is treated as the software emergency-stop input: OFF stops and disables the motor. A physical emergency-stop circuit is still recommended for the final machine.

## Folder Structure

```text
include/
src/
docs/
lib/
test/
platformio.ini
README.md
```

## Status

**v2.0.0 — Motion-Controlled Tester**

Ready for ESP32 hardware testing of manual jog, mode selection, automatic motion and toggle-stop behavior.