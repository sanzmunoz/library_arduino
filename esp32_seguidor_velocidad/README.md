# Sensor de Velocidad y Aceleración con ESP32 + TCRT5000 por Bluetooth

Proyecto en Arduino IDE para **ESP32 clásico** que mide en tiempo real la velocidad tangencial (**in/s**), la aceleración tangencial (**in/s²**) y la distancia recorrida acumulada (**in**, $\approx 0.4947\text{ in}$ por sección y $\approx 7.9158\text{ in}$ por vuelta completa) del **piñón conducido** de la transmisión con reducción 1/4 ($z_2 = 64$ dientes, módulo $m = 1.0\text{ mm}$, diámetro primitivo $d_2 = 64.0\text{ mm}$, radio $r_2 \approx 1.260\text{ in}$), detectando las **16 secciones** dibujadas directamente sobre él mediante un sensor óptico **TCRT5000** (salida digital `D0`) y enviando los datos por **Bluetooth Classic (SPP)** a una aplicación Android en tiempo real (10 Hz / cada 100 ms).

> **Nota sobre el diámetro de referencia:** Por defecto el código usa el diámetro primitivo $d_2 = 64.0\text{ mm}$ (`DIAMETRO_PINON_MM = 64.0f`). Si prefieres calcular sobre el diámetro exterior del piñón conducido ($d_{a2} = 66.0\text{ mm}$), solo cambia `DIAMETRO_PINON_MM` a `66.0f`.

Además, [esp32_seguidor_velocidad.ino](esp32_seguidor_velocidad.ino) conserva dos bloques comentados listos para activar si necesitas ver unidades angulares (**RPM**, **rad/s**, **rad/s²**) o unidades lineales métricas (**m/s** y **m/s²** para una rueda de **65 mm**).

---

## 1. Conexiones (ESP32 + TCRT5000)

> **¡IMPORTANTE sobre el voltaje!**
> Alimenta el módulo TCRT5000 con **3.3 V** (`3V3` del ESP32), **NO con 5 V (`VIN`)**.
> Los pines GPIO del ESP32 trabajan a **3.3 V** y no toleran 5 V de entrada en `D0`.

| Pin en módulo TCRT5000 | Pin en ESP32 | Notas |
|---|---|---|
| `VCC` | `3V3` | Alimentación segura de 3.3 V |
| `GND` | `GND` | Tierra común |
| `D0` | `GPIO 27` | Configurable con `PIN_SENSOR` en el código |
| `A0` | *(sin conectar)* | No se usa en modo interrupción digital |

---

## 2. Calibración del sensor TCRT5000

1. Coloca el sensor frente a las **16 secciones** dibujadas sobre el piñón conducido a una distancia aproximada de **2 a 5 mm**.
2. Gira el piñón lentamente con la mano mientras observas el LED indicador de detección en el módulo TCRT5000.
3. Ajusta el potenciómetro azul del módulo hasta que el LED cambie de estado (encienda/apague) de forma limpia en cada una de las 16 secciones sin parpadeos falsos.

---

## 3. Aplicaciones recomendadas en Google Play Store

### Opción 1 (Recomendada): **Serial Bluetooth Terminal** *(Kai Morich)*
Ideal para ver los valores en tiempo real sin configuración compleja, enviar comandos y guardar registros.
- Muestra automáticamente las líneas en formato legible:
  `Vel: 7.92 in/s | Acel: 1.45 in/s2 | Dist: 15.83 in`
- Permite enviar comandos de un carácter desde la app:
  - `t` → Modo texto legible (por defecto)
  - `c` → Modo CSV (`vel_ins,acel_ins2,dist_in`)
  - `r` → Reiniciar la distancia acumulada a `0.00 in`
  - `h` → Mostrar ayuda de comandos

### Opción 2 (Para gráficas en tiempo real): **Bluetooth Serial Plotter** / **Bluetooth Graphics**
Si quieres graficar velocidad, aceleración y distancia en vivo:
1. Conecta la app a `ESP32_Tacometro`.
2. Envía el comando `c` para activar el formato CSV (`vel_ins,acel_ins2,dist_in`).
3. Configura la app para leer **3 canales** separados por coma (`,`).

---

## 4. Carga en Arduino IDE y emparejamiento

1. Abre `esp32_seguidor_velocidad.ino` en **Arduino IDE**.
2. En **Herramientas → Placa**, selecciona tu placa ESP32 clásica (por ejemplo, **ESP32 Dev Module**).
3. Sube el código al ESP32. Puedes verificar mensajes de diagnóstico en el Monitor Serie a **115200 baudios**.
4. En tu teléfono Android ve a **Ajustes → Bluetooth**, busca el dispositivo **`ESP32_Tacometro`** y vincúlalo (emparejar).
5. Abre **Serial Bluetooth Terminal**:
   - Menú lateral (`≡`) → **Devices** → pestaña **Bluetooth Classic** → selecciona **`ESP32_Tacometro`**.
   - Toca el icono de conectar y verás las lecturas actualizándose en tiempo real.

---

## 5. Activar modos opcionales comentados

En [esp32_seguidor_velocidad.ino](esp32_seguidor_velocidad.ino) tienes dos bloques comentados dentro de `loop()` (puedes activar cualquiera de forma independiente):

- **Modo angular (`RPM`, `rad/s`, `rad/s²`):**
  1. Descomenta `BLOQUE COMENTADO 1: VELOCIDAD Y ACELERACIÓN ANGULAR` dentro de `loop()`.
- **Modo lineal métrico (`m/s` y `m/s²` con rueda de 65 mm):**
  1. Descomenta `DIAMETRO_RUEDA_MM` y `RADIO_RUEDA_M` al inicio del archivo (`radio = 0.0325 m`).
  2. Descomenta `BLOQUE COMENTADO 2: VELOCIDAD Y ACELERACIÓN LINEAL MÉTRICA` dentro de `loop()`.

Luego vuelve a compilar y subir el sketch al ESP32.
