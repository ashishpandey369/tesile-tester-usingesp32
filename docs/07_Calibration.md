# Calibration

# GoldTester
## Precision Gold Chain Tensile Testing Machine

Version : 1.0

---

# 1. Overview

Calibration is the process of converting the raw digital values from the HX711 into accurate force measurements.

Without proper calibration, the displayed force values will be incorrect regardless of the quality of the load cell.

GoldTester uses a two-step calibration process:

• Zero Calibration

• Span Calibration

---

# 2. Objectives

The calibration system shall

• Remove sensor offset

• Calculate calibration factor

• Improve measurement accuracy

• Store calibration values

• Restore calibration after reboot

• Verify calibration quality

---

# 3. Calibration Workflow

```
Power ON

↓

Initialize HX711

↓

Read Zero Load

↓

Store Offset

↓

Place Known Weight

↓

Read Sensor

↓

Calculate Factor

↓

Save Calibration

↓

Verification

↓

Calibration Complete
```

---

# 4. Calibration Parameters

| Parameter | Description |
|------------|-------------|
| Offset | Zero-load sensor value |
| Calibration Factor | Converts raw value to kilograms |
| Known Weight | Standard reference weight |
| Measured Weight | Calculated value |
| Error | Difference between actual and measured |

---

# 5. Zero Calibration

Purpose

Removes the sensor offset when no force is applied.

Procedure

1. Remove all load from the load cell.
2. Wait for the reading to stabilize.
3. Read 100 samples.
4. Calculate the average.
5. Store this value as the zero offset.

Formula

```
Offset = Average(Raw Samples)
```

Example

```
Raw Samples

8456208

8456210

8456209

8456211

↓

Average

8456209
```

Offset

8456209

---

# 6. Span Calibration

Purpose

Determines the relationship between raw sensor values and actual force.

Procedure

1. Complete Zero Calibration.
2. Apply a known weight.
3. Read 100 samples.
4. Calculate average.
5. Compute calibration factor.

Formula

```
Calibration Factor

=

(Raw Reading - Offset)

/

Known Weight
```

Example

Known Weight

5.000 kg

Offset

8456209

Loaded Reading

8421109

Difference

35100

Calibration Factor

7020

---

# 7. Force Calculation

Once calibration is complete,

Force is calculated using

```
Force

=

(Raw Reading - Offset)

/

Calibration Factor
```

Example

Raw Reading

8438659

Offset

8456209

Calibration Factor

7020

↓

Force

2.500 kg

---

# 8. Sampling Strategy

To reduce electrical noise,

multiple samples shall be averaged.

Recommended

100 samples

↓

Average

↓

Calibration

Advantages

• Better Stability

• Lower Noise

• Higher Accuracy

---

# 9. Filtering

The calibration routine shall use a Moving Average Filter.

```
Sample1

Sample2

Sample3

...

Sample100

↓

Average
```

This removes random fluctuations.

---

# 10. Calibration Storage

Calibration values shall be permanently stored.

Stored Parameters

• Offset

• Calibration Factor

Future Storage

ESP32 Preferences

or

EEPROM

---

# 11. Automatic Restore

After every reboot,

the machine shall

Load Offset

↓

Load Calibration Factor

↓

Verify Values

↓

Start Normally

No recalibration required.

---

# 12. Calibration Verification

Procedure

Apply

1kg

↓

Measure

↓

Apply

2kg

↓

Measure

↓

Apply

5kg

↓

Measure

↓

Compare Results

---

# 13. Accuracy Check

Measurement Error

```
Error

=

Measured Weight

-

Actual Weight
```

Example

Actual

5.000 kg

Measured

5.004 kg

Error

0.004 kg

---

# 14. Acceptance Criteria

| Parameter | Requirement |
|------------|-------------|
| Zero Drift | Less than ±0.005 kg |
| Repeatability | Better than ±0.01 kg |
| Accuracy | Better than ±0.02 kg |
| Stability | Continuous |

---

# 15. Calibration Frequency

Recommended

Initial Installation

↓

Monthly

↓

After Load Cell Replacement

↓

After Mechanical Changes

↓

Whenever Accuracy is Suspected

---

# 16. Common Problems

Problem

Zero Drift

Cause

Mechanical Stress

Solution

Re-zero

----------------------------

Problem

Large Error

Cause

Incorrect Calibration Factor

Solution

Repeat Span Calibration

----------------------------

Problem

Unstable Reading

Cause

Electrical Noise

Solution

Improve Grounding

----------------------------

Problem

Negative Reading

Cause

Incorrect Offset

Solution

Zero Calibration

---

# 17. Future Automatic Calibration

Future versions may include

Automatic Zero

Automatic Drift Compensation

Temperature Compensation

Multiple Calibration Profiles

Factory Reset

Cloud Backup

---

# 18. Calibration Safety

Never calibrate while

Motor Running

Chain Installed

Force Applied

Machine Vibrating

Power Unstable

---

# 19. Calibration Summary

The GoldTester calibration process ensures accurate conversion of raw HX711 readings into real-world force measurements.

The process includes

✓ Zero Calibration

✓ Span Calibration

✓ Noise Filtering

✓ Accuracy Verification

✓ Permanent Storage

✓ Automatic Restore

Proper calibration is essential for obtaining reliable and repeatable tensile testing results and forms the basis of all force measurements performed by the machine.