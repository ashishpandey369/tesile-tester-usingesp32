# GoldTester
## Precision Gold Chain Tensile Testing Machine

Version : 1.0

Author1 : Ashish Pandey
Author2: Vaishnavi NP 
Platform :
- ESP32 DevKit V1
- PlatformIO
- C++
- OOP

---

# Project Objective

Develop a professional precision tensile testing machine capable of measuring the tensile force applied to gold chains using a Load Cell and controlling the pulling force through a NEMA17 stepper motor.

The machine shall:

• Measure applied force continuously.
• Display live force on TFT.
• Allow target force selection.
• Automatically stop at target force.
• Prevent overshoot.
• Protect expensive jewellery.
• Store calibration.
• Provide smooth industrial operation.

---

# Machine Workflow

Power ON

↓

Hardware Initialization

↓

Display Boot Screen

↓

Initialize HX711

↓

Initialize Stepper Driver

↓

Initialize Safety System

↓

Ready Screen

↓

User Selects Target Force

↓

START

↓

Motor Pulls Chain

↓

Read Force

↓

Compare with Target

↓

Adjust Motor Speed

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

↓

Ready

---

# Hardware

Controller

ESP32 DevKit V1

Display

3.5 Inch SPI TFT
ILI9488
480x320

Load Cell

20kg

ADC

HX711

Motor

NEMA17

Driver

HW-134A

Buzzer

5V Active Buzzer

Power

24V Supply

---

# Final Pin Mapping

## HX711

DT
GPIO4

SCK
GPIO5

---

## Buzzer

GPIO14

---

## HW-134A

STEP
GPIO25

DIR
GPIO17

ENABLE
GPIO16

RST
3.3V

SLP
3.3V

MS1
GND

MS2
GND

MS3
GND

VMOT
24V

100uF Capacitor across VMOT

---

## TFT

SCK
GPIO18

MOSI
GPIO23

MISO
GPIO19

CS
GPIO27

DC
GPIO32

RST
GPIO33

LED
5V

---

# Software Architecture

main.cpp

↓

MachineController

↓

DisplayManager

↓

LoadCellManager

↓

MotorController

↓

SafetyManager

↓

BuzzerManager

---

# Machine States

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

ERROR

---

# Display Layout

Gold Tester

Current Force

0.000 kg

Target Force

5.000 kg

Motor

Forward

Status

Ready

Progress

██████████

---

# Force Control Logic

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

Motor Speed Adjustment

↓

Near Target

↓

Reduce Speed

↓

Micro Movement

↓

Target Reached

↓

Stop

---

# Motor Behaviour

Far From Target

Fast

Medium Distance

Medium Speed

Near Target

Slow

Very Near Target

Creep Mode

Target

Stop

---

# Load Cell

Sampling

80 SPS

Filtering

Moving Average

Calibration

Known Weight Method

Offset

Automatic Zero

Storage

EEPROM

---

# Safety Features

Maximum Force Protection

Emergency Stop

Motor Timeout

HX711 Failure Detection

Display Failure Detection

Power Loss Recovery

Watchdog

---

# User Interface

Boot Screen

Ready Screen

Running Screen

Completed Screen

Calibration Screen

Settings Screen

Error Screen

---

# Logging

Display

Current Force

Target

Motor Direction

Motor Speed

Machine State

Serial Monitor

Debug Information

Calibration Values

Errors

Warnings

---

# Calibration

Zero Calibration

Span Calibration

Known Weight Calibration

EEPROM Save

EEPROM Restore

Verification

---

# Testing Procedure

Display Test

HX711 Test

Motor Test

Calibration Test

Target Force Test

Emergency Stop Test

Long Duration Test

Repeatability Test

---

# Future Improvements

Touch UI

Bluetooth

WiFi

OTA

SD Card Logging

USB Export

Graph Plotting

CSV Export

Automatic Reports

Cloud Synchronization

Mobile Application

Battery Backup

Printer Support

QR Code Reports

---

# Development Roadmap

Phase 1

Hardware Testing

Phase 2

Display

Phase 3

Load Cell

Phase 4

Motor

Phase 5

Machine Controller

Phase 6

Force Control

Phase 7

Calibration

Phase 8

Safety

Phase 9

User Interface

Phase 10

PCB Design

Phase 11

Enclosure

Phase 12

Production Firmware

---

# Coding Rules

Object Oriented Programming

Single Responsibility Principle

No Global Variables

Reusable Modules

Readable Code

Meaningful Naming

Separate Hardware Layer

Separate Logic Layer

Separate UI Layer

---

# Folder Structure

Stress

include

src

docs

platformio.ini

README.md

---

# Documentation Files

README

Project Overview

Hardware Architecture

Software Architecture

Wiring Guide

Force Control Algorithm

Display UI

Calibration

Testing

Safety

Future Improvements

Roadmap

Engineering Logbook

TODO

Changelog

---

# TODO

Display

HX711

Calibration

Motor

Acceleration

Target Force

Display UI

EEPROM

Settings

Safety

PCB

Testing

Documentation

Production Firmware

---

# Version

v1.0

Status

Development Started
