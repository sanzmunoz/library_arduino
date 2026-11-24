#include <Arduino.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth Classic no esta habilitado. Selecciona una placa ESP32 clasica (ej. ESP32 Dev Module) en Arduino IDE.
#endif

// ============================================================================
// CONFIGURACIÓN PRINCIPAL
// ============================================================================
const uint8_t PIN_SENSOR = 27;               // Pin conectado a la salida D0 del TCRT5000
const uint16_t ESPACIOS_PINON = 16;          // 16 secciones sensadas dibujadas sobre el piñón conducido
const char* BT_NOMBRE = "ESP32_Tacometro";   // Nombre visible en el Bluetooth del teléfono

// Geometría del piñón conducido (Reducción 1/4: z2 = 64 dientes, m = 1.0 mm)
// Diámetro primitivo d2 = 64.0 mm (cambia a 66.0f si deseas usar el diámetro exterior da2)
const float DIAMETRO_PINON_MM = 64.0f;
const float MM_POR_PULGADA = 25.4f;
const float RADIO_PINON_IN = (DIAMETRO_PINON_MM / 2.0f) / MM_POR_PULGADA; // ~1.25984 in
const float DISTANCIA_POR_PULSO_IN = (2.0f * (float)PI * RADIO_PINON_IN) / (float)ESPACIOS_PINON; // ~0.49474 in/pulso

const uint32_t INTERVALO_MUESTREO_US = 1000000UL; // 100 ms (10 Hz) de envío en tiempo real
const uint32_t DEBOUNCE_US = 500UL;              // Antirrebote por software (soporta hasta ~7500 RPM)
const uint32_t TIMEOUT_PARADA_US = 500000UL;     // 0.5 s sin pulsos -> se considera detenido

// Coeficientes de suavizado exponencial (EMA): 0.0 (muy suave) a 1.0 (sin filtro)
const float ALPHA_FILTRO_VEL = 0.4f;
const float ALPHA_FILTRO_ACEL = 0.3f;

// ============================================================================
// CONFIGURACIÓN MODO LINEAL MÉTRICO (OPCIONAL - COMENTADO)
// Descomenta las dos líneas de abajo si deseas calcular velocidad en m/s y
// aceleración en m/s^2 para una rueda de 65 mm de diámetro.
// ============================================================================
/*
const float DIAMETRO_RUEDA_MM = 65.0f;
const float RADIO_RUEDA_M = (DIAMETRO_RUEDA_MM / 1000.0f) / 2.0f; // 0.0325 m
*/

// ============================================================================
// OBJETOS Y VARIABLES COMPARTIDAS CON LA INTERRUPCIÓN
// ============================================================================
BluetoothSerial SerialBT;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t lastPulseUs = 0;
volatile uint64_t sumPeriodUs = 0;
volatile uint32_t periodCount = 0;
volatile uint32_t totalPulses = 0;
volatile bool hasPrevPulse = false;

// Estado del cálculo en loop()
uint32_t lastReportUs = 0;
float omegaFiltrada = 0.0f;
float omegaAnterior = 0.0f;
float alphaFiltrada = 0.0f;
bool modoCsv = false;

/**
 * Rutina de interrupción (ISR) en cada flanco de bajada del TCRT5000.
 * Usa exclusivamente aritmética entera y sección crítica para evitar condiciones de carrera.
 */
void IRAM_ATTR onSensorPulse() {
  uint32_t now = micros();
  portENTER_CRITICAL_ISR(&mux);

  if (hasPrevPulse) {
    uint32_t dt = now - lastPulseUs;
    if (dt < DEBOUNCE_US) {
      portEXIT_CRITICAL_ISR(&mux);
      return;
    }
    if (dt <= TIMEOUT_PARADA_US) {
      sumPeriodUs += dt;
      periodCount++;
    }
  }

  totalPulses++;
  lastPulseUs = now;
  hasPrevPulse = true;

  portEXIT_CRITICAL_ISR(&mux);
}

/**
 * Procesa comandos simples enviados desde la app Bluetooth del teléfono:
 *  't' / 'T' -> Modo texto legible (por defecto: in/s, in/s^2 e in)
 *  'c' / 'C' -> Modo CSV (vel_ins,acel_ins2,dist_in) para gráficas o registro
 *  'r' / 'R' -> Reinicia el contador de distancia acumulada a 0.00 in
 *  'h' / 'H' -> Muestra la ayuda
 */
void procesarComandosBT() {
  while (SerialBT.available() > 0) {
    char cmd = (char)SerialBT.read();
    if (cmd == '\r' || cmd == '\n') {
      continue;
    }

    switch (cmd) {
      case 't':
      case 'T':
        modoCsv = false;
        SerialBT.println("Modo texto legible activado");
        break;

      case 'c':
      case 'C':
        modoCsv = true;
        SerialBT.println("vel_ins,acel_ins2,dist_in");
        break;

      case 'r':
      case 'R':
        portENTER_CRITICAL(&mux);
        totalPulses = 0;
        portEXIT_CRITICAL(&mux);
        SerialBT.println("Distancia reiniciada a 0.00 in");
        break;

      case 'h':
      case 'H':
        SerialBT.println("Comandos: 't' = texto | 'c' = CSV (vel_ins,acel_ins2,dist_in) | 'r' = reiniciar dist. | 'h' = ayuda");
        break;

      default:
        break;
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_SENSOR), onSensorPulse, FALLING);

  if (!SerialBT.begin(BT_NOMBRE)) {
    Serial.println("Error al inicializar Bluetooth Classic.");
    while (true) {
      delay(1000);
    }
  }

  lastReportUs = micros();

  Serial.print("Tacometro ESP32 iniciado. Nombre Bluetooth: ");
  Serial.println(BT_NOMBRE);
  Serial.println("Envia 't' (texto), 'c' (CSV), 'r' (reiniciar dist.) o 'h' (ayuda) por Bluetooth.");
}

void loop() {
  procesarComandosBT();

  uint32_t checkUs = micros();
  if ((uint32_t)(checkUs - lastReportUs) < INTERVALO_MUESTREO_US) {
    return;
  }

  // Copia atómica de acumuladores e instante actual dentro de la sección crítica
  uint32_t nowUs;
  uint32_t localLastPulseUs;
  uint64_t localSumUs;
  uint32_t localCount;
  uint32_t localTotalPulses;
  bool localHasPrev;

  portENTER_CRITICAL(&mux);
  nowUs = micros();
  localLastPulseUs = lastPulseUs;
  localSumUs = sumPeriodUs;
  localCount = periodCount;
  localTotalPulses = totalPulses;
  localHasPrev = hasPrevPulse;
  sumPeriodUs = 0;
  periodCount = 0;

  uint32_t elapsedSincePulse = nowUs - localLastPulseUs;
  if (localHasPrev && elapsedSincePulse > TIMEOUT_PARADA_US) {
    hasPrevPulse = false;
    localHasPrev = false;
  }
  portEXIT_CRITICAL(&mux);

  float dtSec = (float)((uint32_t)(nowUs - lastReportUs)) * 1e-6f;
  lastReportUs = nowUs;
  if (dtSec <= 0.0f) {
    dtSec = (float)INTERVALO_MUESTREO_US * 1e-6f;
  }

  // Cálculo de velocidad angular instantánea (rad/s)
  float omegaRaw = 0.0f;
  if (!localHasPrev) {
    omegaRaw = 0.0f;
  } else if (localCount > 0) {
    float periodoPromedioSec = ((float)localSumUs / (float)localCount) * 1e-6f;
    float revPorSeg = 1.0f / (periodoPromedioSec * (float)ESPACIOS_PINON);
    omegaRaw = revPorSeg * 2.0f * (float)PI;
  } else {
    // Entre pulsos lentos (aún sin timeout): acota la velocidad máxima posible
    float elapsedSec = (float)elapsedSincePulse * 1e-6f;
    float omegaMaxPosible = 0.0f;
    if (elapsedSec > 0.0f) {
      omegaMaxPosible = (2.0f * (float)PI) / (elapsedSec * (float)ESPACIOS_PINON);
    }
    omegaRaw = (omegaFiltrada < omegaMaxPosible) ? omegaFiltrada : omegaMaxPosible;
  }

  // Filtro EMA sobre la velocidad angular
  if (!localHasPrev) {
    omegaFiltrada = 0.0f;
  } else {
    omegaFiltrada = ALPHA_FILTRO_VEL * omegaRaw + (1.0f - ALPHA_FILTRO_VEL) * omegaFiltrada;
  }

  // Cálculo de aceleración angular (rad/s^2) con filtro EMA
  float alphaRaw = (omegaFiltrada - omegaAnterior) / dtSec;
  if (!localHasPrev && omegaAnterior == 0.0f) {
    alphaFiltrada = 0.0f;
  } else {
    alphaFiltrada = ALPHA_FILTRO_ACEL * alphaRaw + (1.0f - ALPHA_FILTRO_ACEL) * alphaFiltrada;
  }
  omegaAnterior = omegaFiltrada;

  // Velocidad, aceleración tangenciales y distancia acumulada del piñón conducido (in/s, in/s^2 e in)
  float velPinon_ins = omegaFiltrada * RADIO_PINON_IN;
  float acelPinon_ins2 = alphaFiltrada * RADIO_PINON_IN;
  float distPinon_in = (float)localTotalPulses * DISTANCIA_POR_PULSO_IN;

  // Formato por defecto: in/s, in/s^2 e in
  char buffer[160];
  if (modoCsv) {
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f\r\n",
             velPinon_ins, acelPinon_ins2, distPinon_in);
  } else {
    snprintf(buffer, sizeof(buffer),
             "Vel: %.2f in/s | Acel: %.2f in/s2 | Dist: %.2f in\r\n",
             velPinon_ins, acelPinon_ins2, distPinon_in);
  }

  // ==========================================================================
  // BLOQUE COMENTADO 1: VELOCIDAD Y ACELERACIÓN ANGULAR (RPM, rad/s y rad/s^2)
  // Para activarlo, quita las marcas de comentario de bloque de abajo:
  // ==========================================================================
  /*
  float rpm = (omegaFiltrada * 60.0f) / (2.0f * (float)PI);

  if (modoCsv) {
    // Formato CSV angular: rpm,rad_s,rad_s2
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f\r\n",
             rpm, omegaFiltrada, alphaFiltrada);
  } else {
    snprintf(buffer, sizeof(buffer),
             "RPM: %.2f | Vel: %.2f rad/s | Acel: %.2f rad/s2\r\n",
             rpm, omegaFiltrada, alphaFiltrada);
  }
  */

  // ==========================================================================
  // BLOQUE COMENTADO 2: VELOCIDAD Y ACELERACIÓN LINEAL MÉTRICA (m/s y m/s^2 - RUEDA 65 mm)
  // Para activarlo:
  //   1. Descomenta DIAMETRO_RUEDA_MM y RADIO_RUEDA_M al inicio del archivo.
  //   2. Quita las marcas de comentario de bloque de abajo:
  // ==========================================================================
  /*
  float velLineal_ms = omegaFiltrada * RADIO_RUEDA_M;
  float acelLineal_ms2 = alphaFiltrada * RADIO_RUEDA_M;

  if (modoCsv) {
    // Formato CSV métrico: vel_ms,acel_ms2
    snprintf(buffer, sizeof(buffer), "%.3f,%.3f\r\n",
             velLineal_ms, acelLineal_ms2);
  } else {
    snprintf(buffer, sizeof(buffer),
             "Vel: %.3f m/s | Acel: %.3f m/s2\r\n",
             velLineal_ms, acelLineal_ms2);
  }
  */

  // Envío en tiempo real por USB Serial y por Bluetooth Classic
  Serial.print(buffer);
  if (SerialBT.hasClient()) {
    SerialBT.print(buffer);
  }
}
