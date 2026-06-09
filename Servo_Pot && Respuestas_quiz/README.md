# Servo Potenciómetro - Quiz Resuelto

Quiz completo con respuesta: Control de servomotor mediante potenciómetro.

## Descripción

Ejercicio de evaluación resuelto que demuestra el control de un servomotor SG90 usando un potenciómetro analógico como entrada.

## Contenido

- **Servo_Pot_Con_respuestas.ino** - Código completo con explicaciones
- **1.png, 2.png, 3.png, 4.png** - Diagramas y esquemas del circuito

## Características del Código

- **Lectura Analógica:** Potenciómetro en pin analógico
- **Mapeo:** Conversión de 0-1023 a 0-180 grados
- **Servo PWM:** Generación automática de pulsos
- **Suavizado:** Filtrado de ruido analógico
- **Serial Output:** Reporte de ángulo por monitor serial

## Componentes Utilizados

- Arduino UNO
- Servomotor SG90
- Potenciómetro 10k Ω
- Cable de conexión

## Pinaje

| Componente | Pin |
|-----------|-----|
| Potenciómetro | A0 |
| Servo | 9 (PWM) |
| GND | GND |
| +5V | +5V |

## Funcionamiento

1. Gira potenciómetro (0° a 180°)
2. Arduino lee valor analógico (0-1023)
3. Mapea a rango servo (0-180°)
4. Envía pulso PWM al servo
5. Servo se posiciona en el ángulo correspondiente

## Conceptos Clave

- **PWM Servo:** Varía ancho de pulso (1-2 ms cada 20 ms)
- **Mapeo Lineal:** Proporcional entre entrada analógica y ángulo
- **Frecuencia:** ~50 Hz para servo

## Notas de Estudio

Este es un ejercicio resuelto para referencia y verificación de respuestas en quiz académico.

---
**Placa:** Arduino UNO  
**Archivo Principal:** `Servo_Pot_Con_respuestas.ino`  
**Tipo:** Quiz - Ejercicio Evaluado
