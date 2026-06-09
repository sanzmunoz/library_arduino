# Microcontrollers - ITM

Repository of practical microcontroller projects for the Microcontrollers course in the School of Engineering at ITM's Mechatronics program.

## Overview

This repository includes projects with **Arduino**, **Arduino Mega**, **ESP32**, and **MATLAB** for controlling:
- DC Motors (PWM, H-bridge, gradual acceleration)
- Servo Motors (angle control, feedback)
- Peripherals (LCD I2C, 4×4 keypad, potentiometers)
- Communication (Serial, Bluetooth, MATLAB)

## Directory Structure

| Folder | Description |
|--------|------------|
| [Control_PWM_LCD](./Control_PWM_LCD) | DC motor PWM control in 4 stages with LCD |
| [Control_Servo_Matlab](./Control_Servo_Matlab) | Servo control via MATLAB serial communication |
| [lab_2_1_servo_posiciones](./lab_2_1_servo_posiciones) | Servo with potentiometer, LCD, and 4-position recording |
| [labesp32](./labesp32) | DC motor Bluetooth control on ESP32 |
| [Laboratorio_1](./Laboratorio_1) | Binary to decimal converter with Arduino Mega |
| [LCD_Servo_T4x4](./LCD_Servo_T4x4) | Servo controlled by 4×4 keypad and LCD |
| [LCD_Servo_T4x4_v2](./LCD_Servo_T4x4_v2) | Enhanced version: servo with 4×4 keypad and LCD |
| [motor_con_puenteH](./motor_con_puenteH) | DC motor with gradual acceleration/braking and LEDs |
| [Servo_Pot && Respuestas_quiz](./Servo_Pot%20%26%26%20Respuestas_quiz) | Quiz solution: servo with potentiometer |
| [Teclado4x4_LCDI2C](./Teclado4x4_LCDI2C) | 4×4 keypad test with LCD I2C |

## MATLAB Files

- **HMI_Control_Servo.m** - Graphical interface for interactive servo control
- **Test_Analisis_Servo_Serial.m** - Testing and analysis script for servo response

## General Requirements

### Hardware
- Arduino UNO / Arduino Mega 2560 / ESP32
- 16×2 LCD with I2C module
- 4×4 Keypad
- SG90 Servo Motor
- 5V DC Motor
- H-Bridge (L298N or similar)
- Analog potentiometers
- LEDs and protection resistors

### Software
- Arduino IDE 1.8+
- MATLAB R2019a+ (for HMI scripts)
- Required Libraries:
  - `Wire.h` and `LiquidCrystal_I2C.h` (LCD)
  - `Keypad.h` (4×4 keypad)
  - `Servo.h` (servo motors)
  - `BluetoothSerial.h` (ESP32)

## Getting Started

1. Clone the repository
2. Open the corresponding `.ino` file in Arduino IDE
3. Select the correct board and COM port
4. Verify and upload the code

For MATLAB projects, run the `.m` scripts directly in MATLAB with the Arduino/ESP32 connected.

## Notes

- Each project includes detailed comments about PWM, servos, and control concepts
- Laboratory exercises include explanations of binary conversion, PWM control, and modulation
- Some projects have multiple versions (v1, v2) to compare different approaches

---
**Instructor:** Juan Guillermo Patiño Vengoechea  
**Faculty:** Mechatronics Engineering  
**Course:** Microcontrollers
