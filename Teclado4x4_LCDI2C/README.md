# Teclado 4x4 LCD I2C

Prueba y demostración de teclado 4×4 con pantalla LCD 16×2.

## Descripción

Proyecto educativo que demuestra la integración de un teclado 4×4 con una LCD I2C. Permite capturar y visualizar las teclas presionadas.

## Contenido

- **Teclado4x4_E1.ino** - Ejercicio 1: Captura básica de teclas
- **Teclado4x4_E2.ino** - Ejercicio 2: Funcionalidad mejorada

## Características (E1)

- **Detección de Teclas:** Librería Keypad.h
- **Visualización:** LCD I2C muestra teclas en tiempo real
- **Ciclo Continuo:** Captura hasta 8 caracteres, reinicia automáticamente
- **Identificación:** Muestra valor hexadecimal de cada tecla

## Hardware

| Componente | Pin Arduino |
|-----------|------------|
| Teclado Filas | 9, 8, 7, 6 |
| Teclado Columnas | 5, 4, 3, 2 |
| LCD SDA | A4 (20 en Mega) |
| LCD SCL | A5 (21 en Mega) |

## Mapeado de Teclado

```
[1][2][3][A]
[4][5][6][B]
[7][8][9][C]
[*][0][#][D]
```

## Flujo E1

1. Muestra "Teclado 4x4" en línea 1
2. Espera presión de tecla
3. Captura hasta 8 caracteres
4. Reinicia automáticamente después de pausa

## Bibliotecas Requeridas

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
```

## Configuración LCD

```cpp
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C y dimensiones
```

## Notas Técnicas

- Anti-rebote: Incorporado en librería Keypad
- LCD dirección: 0x27 (cambiar a 0x3F si es necesario)
- Delay de captura: 250 ms entre caracteres
- Pausa final: 1000 ms antes de reiniciar

---
**Placa:** Arduino UNO  
**Archivos:** 
- `Teclado4x4_E1.ino` (Ejercicio 1)
- `Teclado4x4_E2.ino` (Ejercicio 2)
