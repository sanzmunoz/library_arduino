/*
 * ==============================================================================
 * Control de Motores DC con ESP32 DEVKIT V1 y Puente H TB6612FNG
 * Inicio seguro con Botón BOOT (GPIO 0) y Parada de Emergencia
 * ==============================================================================
 * 
 * Conexiones asignadas:
 *  - PWMA : GPIO 15 (D15) -> Control de velocidad Motor A (PWM)
 *  - AIN2 : GPIO 2  (D2)  -> Dirección Motor A
 *  - AIN1 : GPIO 4  (D4)  -> Dirección Motor A
 *  - STBY : GPIO 5  (D5)  -> Modo Standby (HIGH = Activo, LOW = Apagado)
 *  - BIN1 : GPIO 18 (D18) -> Dirección Motor B
 *  - BIN2 : GPIO 19 (D19) -> Dirección Motor B
 *  - PWMB : GPIO 21 (D21) -> Control de velocidad Motor B (PWM)
 * 
 * Botón de control:
 *  - Botón BOOT: GPIO 0 (Integrado en la placa ESP32, activo en LOW)
 * 
 * Conexiones de alimentación:
 *  - ESP32 GND <---> TB6612FNG GND <---> Negativo (-) Batería externa (GND COMÚN)
 *  - ESP32 3V3 o VIN <---> TB6612FNG VCC (Lógica: 3.3V o 5V)
 *  - Batería Externa (+) <---> TB6612FNG VM (Alimentación motores: 6V - 12V)
 * ==============================================================================
 */

// --- DEFINICIÓN DE PINES ---
const int PIN_PWMA = 15;
const int PIN_AIN2 = 2;
const int PIN_AIN1 = 4;
const int PIN_STBY = 5;
const int PIN_BIN1 = 18;
const int PIN_BIN2 = 19;
const int PIN_PWMB = 21;

// Botón integrado en la placa ESP32
const int PIN_BOOT = 0;

// Configuración de PWM
const int PWM_FREQ = 20000;      // 20 kHz (frecuencia ultrasónica: elimina el silbido de los motores)
const int PWM_RESOLUTION = 8;    // 8 bits (velocidad de 0 a 255)

// ==============================================================================
// PROTOTIPOS DE FUNCIONES
// ==============================================================================
void setMotorA(int speed);
void setMotorB(int speed);
void brakeMotorA();
void brakeMotorB();
void robotForward(int speed);
void robotBackward(int speed);
void robotTurnLeft(int speed);
void robotTurnRight(int speed);
void robotStop();
void robotBrake();
void robotStandby(bool enable);
bool safeDelay(unsigned long ms);
void ejecutarRutinaPrueba();

// ==============================================================================
// SETUP
// ==============================================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=======================================================");
  Serial.println("  Controlador TB6612FNG con ESP32 (Modo Seguro BOOT)   ");
  Serial.println("=======================================================");

  // 1. Configurar botón BOOT con resistencia pull-up interna
  pinMode(PIN_BOOT, INPUT_PULLUP);

  // 2. Apagar STBY al arrancar (modo seguro)
  pinMode(PIN_STBY, OUTPUT);
  digitalWrite(PIN_STBY, LOW);

  // 3. Configurar pines de dirección como salidas
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);

  // 4. Configurar pines de PWM
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);

  analogWriteFrequency(PIN_PWMA, PWM_FREQ);
  analogWriteFrequency(PIN_PWMB, PWM_FREQ);

  analogWrite(PIN_PWMA, 0);
  analogWrite(PIN_PWMB, 0);

  Serial.println("\n[SISTEMA LISTO EN ESPERA]");
  Serial.println(">>> Presiona el boton 'BOOT' en la placa ESP32 para iniciar.");
  Serial.println(">>> (Tambien sirve como FRENO DE EMERGENCIA si lo presionas durante la prueba)");
}

// ==============================================================================
// LOOP
// ==============================================================================
void loop() {
  // Aseguramos que los motores sigan detenidos mientras esperamos el botón
  robotStop();
  robotStandby(false);

  // Esperar a que el usuario presione el botón BOOT (GPIO 0 pasa a LOW)
  if (digitalRead(PIN_BOOT) == LOW) {
    delay(50); // Antirrebote (debounce)
    if (digitalRead(PIN_BOOT) == LOW) {
      Serial.println("\n[BOTON BOOT DETECTADO]");
      Serial.println("Esperando que sueltes el boton...");
      while (digitalRead(PIN_BOOT) == LOW) {
        delay(10); // Esperar a que se suelte el botón
      }
      
      Serial.println("¡Iniciando motores en 1 segundo! Preparate...");
      delay(1000);

      // Salir de Standby y ejecutar la prueba
      robotStandby(true);
      ejecutarRutinaPrueba();

      // Al finalizar o abortar la prueba, apagamos motores y volvemos a esperar
      robotBrake();
      robotStandby(false);
      Serial.println("\n[PRUEBA FINALIZADA O DETENIDA]");
      Serial.println("Presiona el boton BOOT de nuevo cuando quieras volver a probar.");
    }
  }

  delay(20);
}

// ==============================================================================
// RUTINA DE PRUEBAS
// ==============================================================================
void ejecutarRutinaPrueba() {
  Serial.println("\n>>> [1/6] Adelante al 75% (2 seg)");
  robotForward(190);
  if (!safeDelay(2000)) return;

  Serial.println(">>> Parada suave (Coast) (1 seg)");
  robotStop();
  if (!safeDelay(1000)) return;

  Serial.println(">>> [2/6] Reversa al 60% (2 seg)");
  robotBackward(150);
  if (!safeDelay(2000)) return;

  Serial.println(">>> Freno activo (Brake) (1 seg)");
  robotBrake();
  if (!safeDelay(1000)) return;

  Serial.println(">>> [3/6] Giro a la derecha (1.5 seg)");
  robotTurnRight(180);
  if (!safeDelay(1500)) return;

  Serial.println(">>> Freno activo (1 seg)");
  robotBrake();
  if (!safeDelay(1000)) return;

  Serial.println(">>> [4/6] Giro a la izquierda (1.5 seg)");
  robotTurnLeft(180);
  if (!safeDelay(1500)) return;

  Serial.println(">>> Parada suave (1 seg)");
  robotStop();
  if (!safeDelay(1000)) return;

  Serial.println(">>> [5/6] Aceleracion progresiva hacia adelante");
  for (int v = 60; v <= 255; v += 15) {
    robotForward(v);
    if (!safeDelay(100)) return;
  }
  if (!safeDelay(1000)) return;

  Serial.println(">>> [6/6] Freno total inmediato");
  robotBrake();
  if (!safeDelay(1500)) return;
}

/**
 * Espera con detección de parada de emergencia.
 * Si presionas el botón BOOT en cualquier momento durante el delay,
 * cancela la rutina de inmediato y frena los motores.
 * @return true si completó el tiempo, false si fue abortado por el botón BOOT.
 */
bool safeDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    if (digitalRead(PIN_BOOT) == LOW) {
      delay(50); // Antirrebote
      if (digitalRead(PIN_BOOT) == LOW) {
        Serial.println("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        Serial.println("  [PARADA DE EMERGENCIA]: Boton BOOT presionado!   ");
        Serial.println("  Motores frenados inmediatamente.                 ");
        Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        robotBrake();
        while (digitalRead(PIN_BOOT) == LOW) delay(10); // Esperar a que suelte el botón
        return false; // Aborta la rutina
      }
    }
    delay(10);
  }
  return true;
}

// ==============================================================================
// FUNCIONES DE CONTROL DE MOTORES
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

void robotForward(int speed)   { setMotorA(speed);  setMotorB(speed);  }
void robotBackward(int speed)  { setMotorA(-speed); setMotorB(-speed); }
void robotTurnLeft(int speed)  { setMotorA(-speed); setMotorB(speed);  }
void robotTurnRight(int speed) { setMotorA(speed);  setMotorB(-speed); }
void robotStop()               { setMotorA(0);      setMotorB(0);      }
void robotBrake()              { brakeMotorA();     brakeMotorB();     }

void robotStandby(bool enable) {
  digitalWrite(PIN_STBY, enable ? HIGH : LOW);
}
