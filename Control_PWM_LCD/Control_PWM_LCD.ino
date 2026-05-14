#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// CONFIGURACIÓN DE LA LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// PINES DEL MOTOR
const int pinPWMmotor = 9;
const int pinIN1 = 24;
const int pinIN2 = 25;

// VECTORES DE PINES
const int pinInterruptor[4] = {50, 51, 52, 53};
const int pinLed[4] = {40, 41, 42, 43};

// PINES DE LOS BOTONES
const int pinBotonInicio = 46;
const int pinBotonParo = 47;

// VARIABLES DE CONTROL DEL SISTEMA
bool sistemaActivo = false;
int etapaActual = 0;
int pwmActual = 0;

// VECTORES PARA DETECTAR FLANCO
bool estadoAnterior[4] = {HIGH, HIGH, HIGH, HIGH};
bool estadoAnteriorInicio = HIGH;
bool estadoAnteriorParo = HIGH;

// FUNCIÓN: ACTUALIZAR LCD
void mostrarEstadoLCD(String mensajeLinea1, String mensajeLinea2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mensajeLinea1);
  lcd.setCursor(0, 1);
  lcd.print(mensajeLinea2);
}

// FUNCIÓN: APAGAR TODOS LOS LEDS
void apagarTodosLosLeds() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(pinLed[i], LOW);
  }
}

// FUNCIÓN: ENCENDER LED SEGÚN ETAPA
void encenderLedEtapa(int etapa) {
  apagarTodosLosLeds();
  if (etapa > 0 && etapa <= 4) {
    digitalWrite(pinLed[etapa - 1], HIGH);
  }
}

// FUNCIÓN: DETENER MOTOR Y REINICIAR SISTEMA
void detenerSistema() {
  analogWrite(pinPWMmotor, 0);
  pwmActual = 0;
  etapaActual = 0;
  sistemaActivo = false;
  apagarTodosLosLeds();
  mostrarEstadoLCD("Motor detenido", "Ciclo: 0%");
}

// FUNCIÓN: REVISAR SI SE PRESIONÓ EL BOTÓN DE PARO
bool paroSolicitado() {
  if (digitalRead(pinBotonParo) == LOW) {
    delay(30); // pequeño anti-rebote
    if (digitalRead(pinBotonParo) == LOW) {
      detenerSistema();
      return true;
    }
  }
  return false;
}

// FUNCIÓN: LLEVAR EL MOTOR GRADUALMENTE AL CICLO DESEADO
// porcentajeObjetivo -> 25, 50, 75 o 100
// etapaObjetivo      -> 1, 2, 3 o 4
// ----------------------------------------------------
void llevarMotorACiclo(int porcentajeObjetivo, int etapaObjetivo) {
  int pwmObjetivo = map(porcentajeObjetivo, 0, 100, 0, 255);

  // Mostrar mensaje inicial en LCD
  mostrarEstadoLCD("Subiendo motor", "Objetivo: " + String(porcentajeObjetivo) + "%");

  // Si el PWM actual es menor, subimos progresivamente
  for (int valorPWM = pwmActual; valorPWM <= pwmObjetivo; valorPWM++) {
    if (paroSolicitado()) {
      return;
    }

    analogWrite(pinPWMmotor, valorPWM);
    pwmActual = valorPWM;
    int porcentajeTemporal = map(valorPWM, 0, 255, 0, 100);

    lcd.setCursor(0, 0);
    lcd.print("Ciclo actual:   ");
    lcd.setCursor(13, 0);
    lcd.print("   ");
    lcd.setCursor(13, 0);
    lcd.print(porcentajeTemporal);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Etapa: ");
    lcd.print(etapaObjetivo);
    lcd.print("          ");

    delay(20);
  }

  // Una vez se alcanza el valor final, se actualiza la etapa
  etapaActual = etapaObjetivo;
  encenderLedEtapa(etapaActual);
  mostrarEstadoLCD("Ciclo alcanzado", "Ciclo: " + String(porcentajeObjetivo) + "%");
  delay(700);
}

// FUNCIÓN SETUP
void setup() {
  // Configuración de pines del motor
  pinMode(pinPWMmotor, OUTPUT);
  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);

  // Dirección fija del motorrevisa mi codigo arduino, se pueden reducir lineas con ciclos y vectores, que me recomiendas?
  digitalWrite(pinIN1, HIGH);
  digitalWrite(pinIN2, LOW);

  // Configurar todos los interruptores con ciclo
  for (int i = 0; i < 4; i++) {
    pinMode(pinInterruptor[i], INPUT_PULLUP);
    pinMode(pinLed[i], OUTPUT);
  }

  pinMode(pinBotonInicio, INPUT_PULLUP);
  pinMode(pinBotonParo, INPUT_PULLUP);

  apagarTodosLosLeds();

  // Inicialización de LCD
  lcd.init();
  lcd.backlight();
  mostrarEstadoLCD("Sistema listo", "Pulse INICIO");

  // Asegurar que el motor arranque apagado
  analogWrite(pinPWMmotor, 0);
}

// FUNCIÓN LOOP
void loop() {
  // --------------------------------------------------
  // LECTURA DE BOTÓN DE INICIO
  // --------------------------------------------------
  bool estadoInicio = digitalRead(pinBotonInicio);

  if (estadoAnteriorInicio == HIGH && estadoInicio == LOW) {
    delay(30); // anti-rebote
    if (digitalRead(pinBotonInicio) == LOW) {
      sistemaActivo = true;
      mostrarEstadoLCD("Sistema activo", "Esperando ciclo");
      delay(500);
    }
  }
  estadoAnteriorInicio = estadoInicio;

  // --------------------------------------------------
  // LECTURA DE BOTÓN DE PARO
  // --------------------------------------------------
  bool estadoParo = digitalRead(pinBotonParo);

  if (estadoAnteriorParo == HIGH && estadoParo == LOW) {
    delay(30); // anti-rebote
    if (digitalRead(pinBotonParo) == LOW) {
      detenerSistema();
      delay(300);
    }
  }
  estadoAnteriorParo = estadoParo;

  // --------------------------------------------------
  // SI EL SISTEMA NO ESTÁ ACTIVO, NO HACE NADA
  // --------------------------------------------------
  if (!sistemaActivo) {
    return;
  }

  // LECTURA Y PROCESAMIENTO DE INTERRUPTORES CON CICLO
  for (int i = 0; i < 4; i++) {
    bool estadoActual = digitalRead(pinInterruptor[i]);

    if (estadoAnterior[i] == HIGH && estadoActual == LOW) {
      delay(30);
      if (digitalRead(pinInterruptor[i]) == LOW) {
        if (etapaActual == i) {
          llevarMotorACiclo((i + 1) * 25, i + 1);
        } else {
          mostrarEstadoLCD("Primero haga", "ciclo " + String(i * 25) + "%");
          delay(800);
        }
      }
    }
    estadoAnterior[i] = estadoActual;
  }
}