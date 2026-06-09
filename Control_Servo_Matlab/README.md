# Control Servo MATLAB

Remote servo motor control from MATLAB via serial communication.

## Description

A system that allows sending position angles from MATLAB to Arduino to move an SG90 servo motor. Arduino reads values via serial and generates the required PWM pulses.

## Features

- **Serial Communication:** MATLAB ↔ Arduino (9600 baud)
- **Range:** 0° to 180°
- **Quick Response:** Immediate positioning
- **PWM Explanation:** Code includes detailed comments on PWM differences

## Hardware

| Component | Pin |
|-----------|-----|
| Servo Signal | 9 |
| Servo GND | GND |
| Servo +5V | +5V |

## Basic MATLAB Code

```matlab
% Create serial object
s = serialport("COM_PORT", 9600);

% Send angle (example: 90°)
writeline(s, "90");

% Close port
clear s
```

## PWM Notes

The code explains two distinct PWM applications:

1. **Classical PWM** (LED/Motor): Varies duty cycle (~490 Hz)
2. **Servo PWM** (Servo.h): Varies pulse width (50 Hz, 1000-2000 µs)

Servo pulse widths:
- 1000 µs (1.0 ms) → 0°
- 1500 µs (1.5 ms) → 90°
- 2000 µs (2.0 ms) → 180°

## Required Libraries

```cpp
#include <Servo.h>
```

---
**Board:** Arduino UNO  
**File:** `Control_Servo_Matlab.ino`  
**Baudrate:** 9600
