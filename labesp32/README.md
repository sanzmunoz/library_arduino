# LabESP32

Control de motor DC por Bluetooth desde un dispositivo móvil o PC con ESP32.

## Descripción

Sistema que permite controlar de forma inalámbrica la velocidad y dirección de un motor DC mediante comunicación Bluetooth usando un ESP32.

## Características

- **Bluetooth Serial:** ESP32 emite como dispositivo "ESP32_ControlMotor_PWM"
- **Control de Dirección:** Adelante, Atrás, Detener
- **Control de Velocidad:** PWM 0-255
- **Driver L298N:** Para control del motor DC
- **Serial Monitor:** 115200 baud para debugging

## Hardware

| Componente | Pin ESP32 |
|-----------|-----------|
| Motor IN1 | 26 |
| Motor IN2 | 27 |
| Motor ENA (PWM) | 25 |

## Comandos Bluetooth

| Comando | Descripción |
|---------|------------|
| `F` | Adelante (forward) |
| `B` | Atrás (backward) |
| `S` | Detener (stop) |
| `V150` | Velocidad 150 (rango 0-255) |

## Ejemplo de Uso

1. Empareja Bluetooth: "ESP32_ControlMotor_PWM"
2. Abre terminal serial (115200 baud)
3. Envía comandos:
   ```
   F      (motor adelante)
   V200   (velocidad 200)
   B      (motor atrás)
   S      (detener)
   ```

## Configuración PWM

```cpp
const int canalPWM = 0;
const int frecuenciaPWM = 5000;      // 5 kHz
const int resolucionPWM = 8;         // 8 bits (0-255)
```

## Bibliotecas Requeridas

```cpp
#include <BluetoothSerial.h>
```

## Notas

- Baudrate Serial: 115200
- Validación de rango PWM: constrain(velocidad, 0, 255)
- Compatible con cualquier dispositivo con Bluetooth Classic

---
**Placa:** ESP32  
**Archivo:** `labesp32.ino`  
**Nombre Dispositivo:** ESP32_ControlMotor_PWM
