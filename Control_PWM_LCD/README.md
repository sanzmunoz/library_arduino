# Control PWM LCD

DC motor PWM control in 4 stages (25%, 50%, 75%, 100%) with LCD interface and pushbuttons.

## Description

A system that allows controlling the speed of a DC motor gradually through 4 power stages. Each stage is selected via pushbuttons and displayed on a 16×2 LCD.

## Features

- **4 PWM Stages:** 25%, 50%, 75%, 100% duty cycle
- **LCD I2C Display:** Shows current stage and duty cycle percentage
- **Gradual Acceleration:** Motor ramps smoothly to target speed
- **Start/Stop Button:** System control
- **Status LEDs:** One indicator per stage

## Hardware

| Component | Arduino Mega Pin |
|-----------|-----------------|
| Motor PWM | 9 |
| Motor IN1 | 24 |
| Motor IN2 | 25 |
| Stage 1 Button | 50 |
| Stage 2 Button | 51 |
| Stage 3 Button | 52 |
| Stage 4 Button | 53 |
| Stage 1 LED | 40 |
| Stage 2 LED | 41 |
| Stage 3 LED | 42 |
| Stage 4 LED | 43 |
| Start Button | 46 |
| Stop Button | 47 |
| LCD SDA | 20 |
| LCD SCL | 21 |

## Operation

1. **Initial:** System waits for START button press
2. **Active:** System awaits stage selection via pushbuttons
3. **Acceleration:** Motor ramps smoothly to target PWM (20 ms per step)
4. **Stop:** STOP button immediately halts operation

## Required Libraries

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
```

## Technical Notes

- Debounce: 30 ms on pushbuttons
- PWM: 8-bit (0-255)
- Frequency: 490 Hz (Arduino Mega pin 9)
- LCD Address: 0x27 (change to 0x3F if not detected)

---
**Board:** Arduino Mega 2560  
**File:** `Control_PWM_LCD.ino`
