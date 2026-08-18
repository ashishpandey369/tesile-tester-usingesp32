# Force Control Algorithm

# GoldTester
## Precision Gold Chain Tensile Testing Machine

Version : 1.0

---

# 1. Overview

The Force Control Algorithm is the heart of the GoldTester.

Its primary objective is to continuously monitor the applied tensile force using the Load Cell and accurately control the NEMA17 stepper motor so that the desired target force is reached smoothly without overshooting.

Unlike simple motor controllers, GoldTester uses a closed-loop control strategy where every motor movement depends on real-time feedback from the load cell.

---

# 2. Objectives

The algorithm shall:

• Read force continuously

• Remove sensor noise

• Calculate actual applied force

• Compare current force with target force

• Adjust motor speed automatically

• Reduce speed near target

• Prevent overshoot

• Stop accurately

• Protect expensive jewellery

---

# 3. Closed Loop Control

```
             User Target Force
                    │
                    ▼
          Compare Current Force
                    │
                    ▼
            Calculate Error
                    │
                    ▼
        Determine Motor Speed
                    │
                    ▼
             Move Stepper Motor
                    │
                    ▼
             Apply Tension
                    │
                    ▼
              Load Cell Reads
                    │
                    ▼
                 HX711 ADC
                    │
                    ▼
          Filter + Calibration
                    │
                    ▼
            Current Force
                    │
                    └──────────────┐
                                   │
                                   ▼
                              Repeat Loop
```

---

# 4. Machine Control Flow

```
Power ON

↓

Initialize Hardware

↓

Zero Load Cell

↓

Display READY

↓

User Sets Target Force

↓

Start Test

↓

Read Force

↓

Compare Target

↓

Move Motor

↓

Target Reached

↓

Stop Motor

↓

Buzzer

↓

Display Result

↓

Return Home
```

---

# 5. Force Calculation

Raw HX711 Reading

↓

Offset Removal

↓

Calibration Factor

↓

Actual Force (kg)

Formula

Force = (Raw - Offset) / CalibrationFactor

---

# 6. Signal Filtering

Load cells naturally produce small fluctuations.

To obtain stable readings the firmware uses a Moving Average Filter.

```
Sample1

Sample2

Sample3

...

Sample10

↓

Average

↓

Display Value
```

Advantages

• Stable Display

• Reduced Noise

• Better Accuracy

---

# 7. Force Zones

Instead of running the motor at one constant speed, the pulling force is divided into operating zones.

---

## Zone 1

Far From Target

Difference > 2kg

Motor Speed

100%

Purpose

Fast approach

---

## Zone 2

Approaching Target

Difference

0.5kg - 2kg

Motor Speed

60%

Purpose

Controlled Pulling

---

## Zone 3

Near Target

Difference

0.1kg - 0.5kg

Motor Speed

25%

Purpose

High Accuracy

---

## Zone 4

Precision Zone

Difference

Less than 0.1kg

Motor Speed

5%

Purpose

Micro Adjustment

---

## Zone 5

Target Reached

Difference

< ±0.01kg

Motor

STOP

---

# 8. Motor Behaviour

```
Target = 5.000kg

↓

0.0kg

Motor Full Speed

↓

3.0kg

Fast

↓

4.0kg

Medium

↓

4.7kg

Slow

↓

4.95kg

Very Slow

↓

4.99kg

Creep Mode

↓

5.00kg

STOP
```

---

# 9. Overshoot Protection

Without speed reduction the motor may continue moving due to inertia.

GoldTester prevents this by

• Reducing speed

• Micro stepping near target

• Continuous force monitoring

• Immediate motor stop

---

# 10. Acceleration Profile

Motor motion shall never start or stop instantly.

```
STOP

↓

20%

↓

40%

↓

60%

↓

80%

↓

100%

-------------------

100%

↓

80%

↓

60%

↓

40%

↓

20%

↓

STOP
```

This produces

• Smooth Motion

• Reduced Vibration

• Better Accuracy

---

# 11. Machine States

```
BOOT

↓

READY

↓

PRELOAD

↓

RUNNING

↓

PRECISION

↓

TARGET REACHED

↓

RETURN HOME

↓

COMPLETE
```

---

# 12. Control Loop Timing

The firmware executes a continuous loop.

Recommended update intervals

Load Cell

10 ms

Motor

1 ms

Display

100 ms

Safety

5 ms

Buzzer

Event Driven

---

# 13. Safety Monitoring

Every control cycle checks

Maximum Force

↓

Sensor Failure

↓

Motor Stall

↓

Emergency Stop

↓

Power Failure

↓

Timeout

If any fault occurs

↓

STOP MOTOR

↓

Display ERROR

↓

Sound Alarm

---

# 14. Target Detection

```
Current Force

↓

Target Comparison

↓

Difference

↓

Tolerance Check

↓

Within Limit?

↓

YES

↓

Motor Stop

↓

Success
```

Tolerance

±0.01 kg

---

# 15. Automatic Return

After a successful test

Motor Stops

↓

Display Result

↓

Buzzer

↓

Reverse Motor

↓

Home Position

↓

READY

---

# 16. Future PID Control

Current Version

Adaptive Speed Zones

Future Version

PID Force Controller

```
Error

↓

P

↓

I

↓

D

↓

Motor Speed
```

Advantages

• Faster Response

• Better Precision

• Industrial Grade Control

---

# 17. Algorithm Parameters

| Parameter | Default |
|------------|---------|
| Sampling Rate | 100 Hz |
| Display Refresh | 10 Hz |
| Target Tolerance | ±0.01 kg |
| Moving Average Samples | 10 |
| Maximum Force | 20 kg |
| Emergency Stop Delay | Immediate |

---

# 18. Error Handling

Possible Errors

HX711 Not Responding

↓

Motor Driver Fault

↓

Load Cell Overload

↓

Emergency Button Pressed

↓

Display Failure

↓

Power Supply Failure

↓

Watchdog Timeout

Each error immediately stops the machine.

---

# 19. Future Improvements

Adaptive Learning

Automatic Calibration

Material Profiles

Force Graph

Breaking Force Detection

Elasticity Measurement

PC Software Integration

Cloud Data Logging

Multiple Test Modes

---

# 20. Algorithm Summary

The GoldTester force control system uses a real-time closed-loop control strategy.

The algorithm continuously:

✓ Reads force

✓ Filters noise

✓ Calculates actual load

✓ Compares target

✓ Adjusts motor speed

✓ Prevents overshoot

✓ Stops precisely

✓ Protects the sample

This architecture provides accurate, repeatable, and safe tensile testing suitable for gold chains and other precision mechanical components.