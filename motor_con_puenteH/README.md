# Motor con Puente H

Control de motor DC con aceleración/desaceleración gradual y LEDs indicadores.

## Descripción

Sistema que permite controlar la velocidad de un motor DC de forma suave mediante dos pulsadores: uno para acelerar y otro para frenar. LEDs indican el estado del motor.

## Características

- **Aceleración Gradual:** Aumento suave de velocidad (70 ms por paso)
- **Desaceleración Gradual:** Frenado suave sin saltos
- **LEDs Indicadores:** Motor encendido, acelerando, frenando
- **Rango Completo:** 0-255 PWM
- **Anti-rebote:** Debounce incorporado

## Hardware

| Componente | Pin Arduino Mega |
|-----------|-----------------|
| Botón Acelerar | 10 |
| Botón Frenar | 11 |
| LED Motor Encendido | 13 |
| LED Acelerando | 12 |
| LED Frenando | 8 |
| Motor PWM | 9 |

## Funcionamiento

1. **Reposo:** Velocidad = 0, todos los LEDs apagados
2. **Aceleración:** Presiona pulsador acelerar, LED 12 se enciende
3. **Frenado:** Presiona pulsador frenar, LED 8 se enciende
4. **Motor Activo:** LED 13 indica motor en movimiento

## Lógica de Control

- Si acelera y velocidad < 255: aumenta velocidad
- Si frena y velocidad > 0: disminuye velocidad
- Si ambos presionados: prioridad a freno
- Paso: 1 unidad por cada 70 ms

## Código Relevante

```cpp
int tiempo = 70;        // Retardo entre pasos
int velocidad = 0;      // Rango 0-255
int Motor_PWM = 9;      // Pin PWM
```

## Baudrate Serial

```cpp
Serial.begin(9600);
```

---
**Placa:** Arduino Mega 2560  
**Archivo:** `motor_con_puenteH.ino`
