# Changelog

## v2.0.0 — 2026-08-18 14:15 IST

### Major architecture change

- Removed TAL220 / HX711 load-cell functionality from the active machine.
- Removed force-based calibration and force-based pass/fail logic.
- Added `TENSILE` and `PUSH` operating modes.
- Added UP/DOWN manual single-step movement.
- Added UP/DOWN long-hold continuous movement.
- Added UP + DOWN long press for mode switching.
- Converted START/STOP toggle into master automatic-motion control.
- Toggle OFF immediately stops and disables the motor.
- Added software Current Force value based on motor steps during automatic operation.
- Manual positioning does not change Current Force.
- Next manual button interaction after STOP resets Current Force to `00.000`.
- Updated display for mode, current software force, motor status and machine status.
- Removed HX711 dependency from PlatformIO.
- Updated project overview and README.

### Hardware test target

1. Power ON → `READY`, `00.000`.
2. Use UP/DOWN for manual positioning.
3. Hold UP + DOWN to change `TENSILE` / `PUSH` mode.
4. Turn toggle ON → `RUNNING`, automatic motion starts.
5. Turn toggle OFF → immediate `STOP`.
6. Press UP or DOWN → Current Force resets to `00.000` and manual positioning resumes.
