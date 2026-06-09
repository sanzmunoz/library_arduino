# Test Análisis Servo Serial (MATLAB)

Script MATLAB para pruebas, análisis y caracterización de servomotores conectados por puerto serial.

## Descripción

Herramienta de análisis que permite capturar datos de posición, velocidad y aceleración de un servomotor en tiempo real, generando reportes y gráficas de rendimiento.

## Características

- **Captura de Datos:** Lectura serial en tiempo real
- **Análisis de Respuesta:** Cálculo de tiempo de asentamiento, overshoot
- **Gráficas Automáticas:** Posición, velocidad, aceleración
- **Exportación:** Datos a CSV para análisis adicional
- **Estadísticas:** Media, desviación, máximos/mínimos

## Requisitos

- MATLAB R2019a+
- Arduino con código que reporta posición por serial
- Puerto COM disponible
- Comunicación a 9600 o 115200 baud

## Uso

```matlab
% 1. Ejecutar script
Test_Analisis_Servo_Serial

% 2. Seleccionar puerto COM
% 3. Ingresar parámetros de prueba
% 4. Ejecutar ensayos
% 5. Revisar gráficas y reportes
```

## Parámetros de Prueba

| Parámetro | Descripción |
|-----------|------------|
| Ángulo Inicial | Posición de partida |
| Ángulo Final | Posición objetivo |
| Tiempo Prueba | Duración de captura |
| Baudrate | Velocidad comunicación |

## Análisis Generado

### Respuesta Temporal

- **Tiempo de Subida:** Tiempo para alcanzar 90% del setpoint
- **Tiempo de Asentamiento:** Tiempo para ±2% del valor final
- **Overshoot:** Porcentaje de sobrepaso
- **Error Estacionario:** Diferencia final vs objetivo

### Gráficas

1. **Posición vs Tiempo:** Trayectoria del servo
2. **Velocidad vs Tiempo:** Tasa de cambio
3. **Aceleración vs Tiempo:** Derivada de velocidad
4. **Errores:** Desviación respecto a setpoint

## Interpretación de Resultados

| Métrica | Ideal | Crítica |
|---------|-------|---------|
| Tiempo Subida | < 1 s | > 3 s |
| Overshoot | 0-5% | > 20% |
| Asentamiento | < 2 s | > 5 s |
| Error Final | < 1° | > 5° |

## Exportación de Datos

```matlab
% Los datos se guardan automáticamente en:
% - servo_data.csv
% - servo_analisis.txt
% - servo_graficas.png
```

## Casos de Uso

1. **Validación:** Verificar especificaciones del servo
2. **Comparación:** Contrastar diferentes modelos
3. **Diagnóstico:** Detectar problemas de respuesta
4. **Documentación:** Generar reportes de pruebas

---
**Archivo:** `Test_Analisis_Servo_Serial.m`  
**Tipo:** Script MATLAB  
**Clase:** Herramienta de Análisis Experimental
