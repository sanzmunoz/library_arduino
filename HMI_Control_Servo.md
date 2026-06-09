# HMI Control Servo (MATLAB)

Interfaz gráfica de usuario (HMI) para control interactivo de servomotor desde MATLAB.

## Descripción

Script MATLAB que crea una interfaz gráfica para controlar servomotores conectados a Arduino. Permite enviar comandos de posición, velocidad y direcciones de movimiento.

## Características

- **Interfaz Gráfica:** Sliders, botones y campos de entrada
- **Comunicación Serial:** Conecta con Arduino automáticamente
- **Visualización:** Gráficas de posición vs tiempo
- **Control en Tiempo Real:** Ajuste inmediato de parámetros
- **Rango:** 0° a 180°

## Requisitos

- MATLAB R2019a+
- Arduino IDE con código compatible
- Arduino Mega 2560 o UNO
- Comunicación Serial habilitada

## Uso Básico

```matlab
% 1. Ejecutar script
HMI_Control_Servo

% 2. Seleccionar puerto COM
% El script detecta automáticamente puertos disponibles

% 3. Controlar servo
% - Mover slider: cambio suave de posición
% - Botón "Enviar": envía valor actual
% - Botón "Reset": vuelve a 90°

% 4. Cerrar interfaz
% Presiona X o ejecuta "clear all"
```

## Funcionalidades Principales

| Control | Función |
|---------|---------|
| Slider Posición | Selecciona ángulo 0-180° |
| Campo Velocidad | Define aceleración del servo |
| Botón Enviar | Transmite ángulo a Arduino |
| Botón Reset | Vuelve a 90° (centro) |
| Parar | Detiene movimiento actual |

## Gráficas

- **Posición vs Tiempo:** Muestra trayectoria del servo
- **Error de Seguimiento:** Diferencia entre setpoint y posición real
- **Velocidad:** Tasa de cambio de posición

## Configuración

```matlab
% Puerto COM (modificar según tu sistema)
puerto_COM = 'COM3';
baudrate = 9600;

% Rango del servo
angulo_min = 0;
angulo_max = 180;
```

## Protocolo de Comunicación

Arduino espera formato: `<ángulo>` (ejemplo: `90` para 90°)

```matlab
% Enviar desde MATLAB
writeline(serialport, "90");

% Arduino recibe y posiciona servo
```

## Troubleshooting

| Problema | Solución |
|----------|----------|
| Puerto no detectado | Verificar drivers FTDI |
| Servo no responde | Revisar conexión serial en Arduino |
| Gráficas vacías | Esperar 10 datos antes de graficar |

---
**Archivo:** `HMI_Control_Servo.m`  
**Tipo:** Script MATLAB  
**Versión:** Interfaz gráfica completa
