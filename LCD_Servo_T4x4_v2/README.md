# LCD Servo T4x4 v2

Versión mejorada: Control de servomotor mediante teclado 4×4 con LCD 16×2.

## Descripción

Versión 2 del proyecto LCD_Servo_T4x4 con optimizaciones, mejor interfaz y funcionalidades adicionales.

## Mejoras respecto a v1

- Menú de navegación más intuitivo
- Velocidad de movimiento ajustable
- Grabación de secuencias de movimiento
- Mejor manejo de errores y validación

## Características

- **Teclado 4×4:** Selección de posiciones y funciones
- **LCD I2C 16×2:** Interfaz clara y detallada
- **Servomotor SG90:** Control 0° a 180°
- **Secuencias:** Graba y reproduce movimientos
- **Menús:** Navegación por funciones

## Hardware

Igual a LCD_Servo_T4x4 (ver README de v1)

## Estructura de Menú

```
MENÚ PRINCIPAL
├─ Movimiento Manual
├─ Posiciones Presets
├─ Grabación
└─ Reproducción
```

## Bibliotecas Requeridas

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
```

## Notas de Cambios

- Separación clara de funciones
- Comentarios mejorados
- Validación de entrada robusta

---
**Placa:** Arduino UNO  
**Archivo:** `LCD_Servo_T4x4_v2.ino`  
**Versión:** 2
