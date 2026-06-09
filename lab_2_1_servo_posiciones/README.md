# Lab 2.1 - Servo Posiciones

Control de servomotor SG90 con potenciómetro, LCD I2C y grabación de 4 posiciones.

## Descripción

Sistema que permite:
- Controlar servo en tiempo real con un potenciómetro (0°-180°)
- Visualizar ángulo actual en LCD 16×2 I2C
- Grabar hasta 4 posiciones presionando un botón
- Reproducir las posiciones grabadas

## Características

- **Lectura Analógica:** Potenciómetro A0 mapeado a 0°-180°
- **Visualización:** LCD I2C muestra ángulo y modo
- **Grabación:** Botón en pin 22 graba posición
- **Suavizado:** Promedio de 8 muestras para reducir ruido
- **Estabilidad:** Umbral de 2° para cambios
- **Actualización:** 100 ms (10 Hz)

## Hardware

| Componente | Pin |
|-----------|-----|
| Potenciómetro | A0 |
| Servo | 10 (PWM) |
| Botón Grabar | 22 |
| LCD SDA | 20 |
| LCD SCL | 21 |

## Modos de Operación

1. **Manual:** Control por potenciómetro
2. **Grabación:** Presiona botón para guardar posición actual
3. **Reproducción:** Secuencia de las 4 posiciones grabadas

## Bibliotecas Requeridas

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
```

## Configuración

```cpp
#define LCD_ADDR 0x27     // Cambiar a 0x3F si es necesario
#define LCD_COLS 16
#define LCD_ROWS 2
const int ucambiogiro = 2  // Umbral de cambio (°)
const int nMuestras = 8    // Muestras promediadas
```

## Notas Técnicas

- **Frecuencia Servo:** ~50 Hz
- **Rango de Pulso:** 1000-2000 µs
- **Baudrate:** Lectura local (sin serial)

---
**Placa:** Arduino UNO  
**Archivo:** `lab_2_1_servo_posiciones.ino`
