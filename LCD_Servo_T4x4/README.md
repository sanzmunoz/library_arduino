# LCD Servo T4x4

Control de servomotor mediante teclado 4×4 con visualización en LCD 16×2.

## Descripción

Sistema que permite controlar la posición de un servomotor usando un teclado 4×4 para seleccionar ángulos, con visualización en tiempo real en una LCD I2C.

## Características

- **Teclado 4×4:** Selección de posiciones predefinidas
- **LCD I2C:** Muestra ángulo actual y estado
- **Servomotor SG90:** 0° a 180°
- **Interfaz Intuitiva:** Teclas numeradas y de función

## Hardware

| Componente | Pin Arduino |
|-----------|------------|
| Teclado Filas | 9, 8, 7, 6 |
| Teclado Columnas | 5, 4, 3, 2 |
| Servo | 3 (PWM) |
| LCD SDA | 20 |
| LCD SCL | 21 |

## Mapeado de Teclado

```
[1][2][3][A]
[4][5][6][B]
[7][8][9][C]
[*][0][#][D]
```

## Mapeo de Teclas a Ángulos

| Tecla | Ángulo |
|-------|--------|
| 1 | 20° |
| 2 | 45° |
| 3 | 90° |
| 4 | 135° |
| 5 | 180° |
| A | Centro (90°) |
| D | Posición inicial |

## Bibliotecas Requeridas

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
```

## Instalación de Keypad

Descarga desde Arduino IDE → Sketch → Incluir Librerías → Keypad by Mark Stanley

## Notas

- Anti-rebote incorporado en librería Keypad.h
- LCD dirección I2C: 0x27 (cambiar a 0x3F si es necesario)
- Servo en pin PWM para mejor resolución

---
**Placa:** Arduino UNO  
**Archivo:** `LCD_Servo_T4x4.ino`
