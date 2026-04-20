// Código para Arduino Mega: control de motor con pulsadores y LEDs

// Definición de pines
const int Pin_Acelerar = 10;       // Pulsador aceleración
const int Pin_Frenar = 11;         // Pulsador freno
const int LED_Encendido = 13;      // LED indicador motor encendido
const int LED_Acelerando = 12;     // LED indicador aceleración
const int LED_Frenando = 8;        // LED indicador frenado
const int Motor_PWM = 9;           // Pin PWM para motor

int tiempo = 70;                   // Retardo entre pasos de aceleración/desaceleración
int velocidad = 0;                 // Velocidad actual del motor (0-255)

void setup() {
  // Configurar entradas
  pinMode(Pin_Acelerar, INPUT);
  pinMode(Pin_Frenar, INPUT);

  // Configurar salidas
  pinMode(LED_Encendido, OUTPUT);
  pinMode(LED_Acelerando, OUTPUT);
  pinMode(LED_Frenando, OUTPUT);
  pinMode(Motor_PWM, OUTPUT);

  // Estado inicial
  digitalWrite(LED_Encendido, LOW);
  digitalWrite(LED_Acelerando, LOW);
  digitalWrite(LED_Frenando, LOW);
  analogWrite(Motor_PWM, 0);

  Serial.begin(9600);
  Serial.println("Sistema iniciado");
}

void loop() {
  int btn_acelerar = digitalRead(Pin_Acelerar);
  int btn_frenar = digitalRead(Pin_Frenar);

  // ACELERACIÓN: si presiona acelerar y no presiona freno
  if (btn_acelerar == HIGH && btn_frenar == LOW && velocidad < 255) {
    velocidad++;
    analogWrite(Motor_PWM, velocidad);
    digitalWrite(LED_Encendido, HIGH);
    digitalWrite(LED_Acelerando, HIGH);
    digitalWrite(LED_Frenando, LOW);
    Serial.print("Acelerando: ");
    Serial.println(velocidad);
    delay(tiempo);
  }

  // FRENADO: si presiona freno o si no presiona nada
  else if (btn_frenar == HIGH || (btn_acelerar == LOW && btn_frenar == LOW)) {
    if (velocidad > 0) {
      velocidad--;
      analogWrite(Motor_PWM, velocidad);
      digitalWrite(LED_Acelerando, LOW);
      if (btn_frenar == HIGH) {
        digitalWrite(LED_Frenando, HIGH);
        Serial.print("Frenando: ");
      } else {
        digitalWrite(LED_Frenando, LOW);
        digitalWrite(LED_Encendido, LOW);
        Serial.print("Desaceleración: ");
      }
      Serial.println(velocidad);
      delay(tiempo);
    } else {
      // Motor parado
      digitalWrite(LED_Encendido, LOW);
      digitalWrite(LED_Acelerando, LOW);
      digitalWrite(LED_Frenando, LOW);
    }
  }
}