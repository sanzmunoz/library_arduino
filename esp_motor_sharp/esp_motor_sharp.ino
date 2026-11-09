/*
 * ==============================================================================
 * Control de Motores TB6612FNG con ESP32 y Sensor Infrarrojo SHARP 2Y0A02 F35
 * Versión Mejorada: Detección Continua con Histéresis y Filtro de Zona Ciega
 * ==============================================================================
 * 
 * ¿POR QUÉ SE APAGABA EL SENSOR A CIERTA DISTANCIA?
 * ------------------------------------------------
 * El sensor Sharp GP2Y0A02YK0F (2Y0A02) es de LARGO ALCANCE (20 cm a 150 cm).
 * Su curva de respuesta tiene forma de campana:
 *   - A 150 cm entrega ~0.4 V
 *   - A 40 cm entrega ~1.4 V
 *   - A 20 cm entrega su PICO MÁXIMO (~2.5 V a 2.7 V)
 *   - A 10 cm o menos (ZONA CIEGA): ¡El voltaje cae drásticamente a 1.3V - 1.6V!
 *
 * Si el umbral anterior exigía más de 2.10V o menos de 20cm calculados,
 * al acercar el objeto a 10 cm el voltaje caía y el código creía erróneamente
 * que el objeto se había ido lejos, apagando los motores.
 *
 * MEJORAS IMPLEMENTADAS EN ESTA VERSIÓN:
 *  1. Rango ampliado de activación: Permite detectar desde 10 cm hasta 45 cm.
 *  2. Histéresis: Umbral de encendido y umbral de apagado diferenciados para
 *     evitar titileos o cortes en la frontera de detección.
 *  3. Persistencia de Zona Ciega: Cuando el objeto se acerca mucho (< 15 cm),
 *     el sistema recuerda que el objeto está encima y mantiene los motores
 *     encendidos de forma continua sin cortarse.
 *  4. Monitoreo claro en el Monitor Serie para calibrar a tu gusto.
 * ==============================================================================
 */

// --- PINES MOTORES (TB6612FNG) ---
const int PIN_PWMA = 15;
const int PIN_AIN2 = 2;
const int PIN_AIN1 = 4;
const int PIN_STBY = 5;
const int PIN_BIN1 = 18;
const int PIN_BIN2 = 19;
const int PIN_PWMB = 21;

// --- SENSOR SHARP Y BOTÓN BOOT ---
const int PIN_SHARP = 34; // Pin analógico ADC1 (GPIO 34 / D34)
const int PIN_BOOT  = 0;  // Botón físico BOOT del ESP32

// --- PARÁMETROS DE CONTROL Y VELOCIDAD ---
const int PWM_FREQ = 20000;         // 20 kHz (frecuencia ultrasónica silenciosa)
const int VELOCIDAD_AVANCE = 190;   // Velocidad al detectar objeto (0 a 255)

// --- UMBRALES DE DETECCIÓN CON HISTÉRESIS (Ajustables) ---
// Al objeto acercarse a ~10-35 cm, el voltaje sube típicamente por encima de 1.30V.
const float VOLTAJE_ACTIVAR    = 1.40; // Voltaje mínimo para ENCENDER motores (detecta aprox 10cm - 40cm)
const float VOLTAJE_DESACTIVAR = 1.00; // Voltaje por debajo del cual se APAGAN motores (hacia 50cm - 60cm)

// Persistencia en milisegundos: si el objeto entra de lleno en la zona ciega (< 10 cm),
// mantiene los motores encendidos durante este tiempo adicional para no apagarse de golpe.
const unsigned long TIEMPO_PERSISTENCIA_MS = 800;

// Variables de estado
bool sistemaActivo = false;         // Controlado por el botón BOOT
bool motoresEncendidos = false;     // Estado actual de los motores
unsigned long ultimaDeteccionMs = 0; // Marca de tiempo de la última detección válida

// ==============================================================================
// PROTOTIPOS
// ==============================================================================
void setMotorA(int speed);
void setMotorB(int speed);
void brakeMotorA();
void brakeMotorB();
void robotForward(int speed);
void robotStop();
void robotBrake();
void robotStandby(bool enable);
float leerSensorSharpVoltaje();
float calcularDistanciaAproxCM(float voltaje);
void verificarBotonBoot();

// ==============================================================================
// SETUP
// ==============================================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=======================================================");
  Serial.println("  ESP32 + TB6612FNG + Sensor SHARP 2Y0A02 F35 (v2)     ");
  Serial.println("=======================================================");

  pinMode(PIN_BOOT, INPUT_PULLUP);

  // Inicializar puente H apagado
  pinMode(PIN_STBY, OUTPUT);
  digitalWrite(PIN_STBY, LOW);

  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);

  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);

  analogWriteFrequency(PIN_PWMA, PWM_FREQ);
  analogWriteFrequency(PIN_PWMB, PWM_FREQ);
  analogWrite(PIN_PWMA, 0);
  analogWrite(PIN_PWMB, 0);

  // Configurar ADC del ESP32 (rango hasta ~3.3V)
  analogSetAttenuation(ADC_11db);
  pinMode(PIN_SHARP, INPUT);

  Serial.println("\n[SISTEMA EN ESPERA]");
  Serial.println("-> Presiona el boton 'BOOT' en la placa para ACTIVAR.");
  Serial.println("-> Abre el Monitor Serie para ver el voltaje en tiempo real.");
}

// ==============================================================================
// LOOP
// ==============================================================================
void loop() {
  // Comprobar pulsación del botón BOOT para armar/desarmar
  verificarBotonBoot();

  if (!sistemaActivo) {
    if (motoresEncendidos) {
      robotBrake();
      motoresEncendidos = false;
    }
    robotStop();
    robotStandby(false);
    delay(50);
    return;
  }

  // Lectura con filtro de promedio
  float voltaje = leerSensorSharpVoltaje();
  float distAprox = calcularDistanciaAproxCM(voltaje);
  unsigned long ahora = millis();

  // Imprimir lecturas para monitoreo
  Serial.print("Voltaje: ");
  Serial.print(voltaje, 2);
  Serial.print(" V | Dist. Aprox: ");
  if (distAprox > 0) {
    Serial.print(distAprox, 0);
    Serial.print(" cm");
  } else {
    Serial.print("---");
  }

  // --- LÓGICA DE DETECCIÓN CON HISTÉRESIS Y RETENCIÓN ---
  if (voltaje >= VOLTAJE_ACTIVAR) {
    // Señal fuerte: objeto claramente detectado en la zona deseada
    ultimaDeteccionMs = ahora;
    motoresEncendidos = true;
  } else if (motoresEncendidos && (ahora - ultimaDeteccionMs < TIEMPO_PERSISTENCIA_MS)) {
    // Si los motores ya estaban encendidos y el voltaje cae súbitamente
    // (el objeto se acercó tanto que entró a la zona ciega < 10 cm),
    // mantenemos el avance activo gracias a la persistencia temporal.
    Serial.print(" [ZONA CERCANA / PERSISTENCIA]");
  } else if (voltaje < VOLTAJE_DESACTIVAR) {
    // Señal débil: el objeto realmente se alejó o fue retirado
    motoresEncendidos = false;
  }

  // Aplicar acción a los motores
  if (motoresEncendidos) {
    Serial.println(" ===> [MOTORES ENCENDIDOS ADELANTE]");
    robotForward(VELOCIDAD_AVANCE);
  } else {
    Serial.println(" ===> [DETENIDO / LIBRE]");
    robotStop();
  }

  delay(40); // Intervalo de respuesta rápido
}

// ==============================================================================
// LECTURA DEL SENSOR SHARP (Promedio de 20 muestras)
// ==============================================================================
float leerSensorSharpVoltaje() {
  const int MUESTRAS = 20;
  long sumaRaw = 0;

  for (int i = 0; i < MUESTRAS; i++) {
    sumaRaw += analogRead(PIN_SHARP);
    delayMicroseconds(150);
  }

  float promedioRaw = (float)sumaRaw / MUESTRAS;
  float voltaje = (promedioRaw / 4095.0) * 3.3;
  return voltaje;
}

/**
 * Estimación de distancia basada en la curva empírica del GP2Y0A02YK0F.
 * Nota: Válida para la rama normal de 20 cm a 150 cm.
 */
float calcularDistanciaAproxCM(float voltaje) {
  if (voltaje < 0.35) return -1.0; // Muy lejos o sin objeto
  return (62.5 * pow(voltaje, -1.10));
}

// ==============================================================================
// GESTIÓN DEL BOTÓN BOOT (ARMADO / DESARMADO)
// ==============================================================================
void verificarBotonBoot() {
  if (digitalRead(PIN_BOOT) == LOW) {
    delay(50); // Antirrebote
    if (digitalRead(PIN_BOOT) == LOW) {
      while (digitalRead(PIN_BOOT) == LOW) delay(10); // Esperar a que se suelte

      sistemaActivo = !sistemaActivo;

      if (sistemaActivo) {
        Serial.println("\n***************************************************");
        Serial.println("  [SISTEMA ACTIVADO]: Iniciando en 1 segundo...    ");
        Serial.println("***************************************************");
        delay(1000);
        robotStandby(true);
        ultimaDeteccionMs = 0;
        motoresEncendidos = false;
      } else {
        Serial.println("\n***************************************************");
        Serial.println("  [SISTEMA DESACTIVADO]: Motores apagados.         ");
        Serial.println("***************************************************");
        motoresEncendidos = false;
        robotBrake();
        robotStandby(false);
      }
    }
  }
}

// ==============================================================================
// CONTROL DE MOTORES
// ==============================================================================
void setMotorA(int speed) {
  speed = constrain(speed, -255, 255);
  if (speed > 0) {
    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, LOW);
    analogWrite(PIN_PWMA, speed);
  } else if (speed < 0) {
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, HIGH);
    analogWrite(PIN_PWMA, abs(speed));
  } else {
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, LOW);
    analogWrite(PIN_PWMA, 0);
  }
}

void setMotorB(int speed) {
  speed = constrain(speed, -255, 255);
  if (speed > 0) {
    digitalWrite(PIN_BIN1, HIGH);
    digitalWrite(PIN_BIN2, LOW);
    analogWrite(PIN_PWMB, speed);
  } else if (speed < 0) {
    digitalWrite(PIN_BIN1, LOW);
    digitalWrite(PIN_BIN2, HIGH);
    analogWrite(PIN_PWMB, abs(speed));
  } else {
    digitalWrite(PIN_BIN1, LOW);
    digitalWrite(PIN_BIN2, LOW);
    analogWrite(PIN_PWMB, 0);
  }
}

void brakeMotorA() {
  digitalWrite(PIN_AIN1, HIGH);
  digitalWrite(PIN_AIN2, HIGH);
  analogWrite(PIN_PWMA, 0);
}

void brakeMotorB() {
  digitalWrite(PIN_BIN1, HIGH);
  digitalWrite(PIN_BIN2, HIGH);
  analogWrite(PIN_PWMB, 0);
}

void robotForward(int speed) {
  setMotorA(speed);
  setMotorB(speed);
}

void robotStop() {
  setMotorA(0);
  setMotorB(0);
}

void robotBrake() {
  brakeMotorA();
  brakeMotorB();
}

void robotStandby(bool enable) {
  digitalWrite(PIN_STBY, enable ? HIGH : LOW);
}
