/*---------------------------------------------------
  Control de motor DC por Bluetooth con ESP32
  Driver L298N
---------------------------------------------------*/

#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// Pines del motor
const int IN1 = 26;
const int IN2 = 27;
const int ENA = 25;

// Configuración PWM
const int canalPWM = 0;
const int frecuenciaPWM = 5000;
const int resolucionPWM = 8;

void setup()
{
  Serial.begin(115200);

  // Iniciar Bluetooth
  SerialBT.begin("ESP32_ControlMotor_PWM");

  // Configurar pines
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Configurar PWM
  ledcAttach(
    ENA,
    frecuenciaPWM,
    resolucionPWM
  );

  // Motor apagado inicialmente
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  analogWrite(ENA, 0);

  Serial.println("Sistema iniciado");
}

void loop()
{
  // Revisar si llegó dato Bluetooth
  if (SerialBT.available())
  {
    String comando = SerialBT.readStringUntil('\n');

    comando.trim();

    // =========================
    // ADELANTE
    // =========================
    if (comando == "F")
    {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);

      Serial.println("Motor adelante");
    }

    // =========================
    // ATRÁS
    // =========================
    else if (comando == "B")
    {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);

      Serial.println("Motor atras");
    }

    // =========================
    // STOP
    // =========================
    else if (comando == "S")
    {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);

      Serial.println("Motor detenido");
    }

    // =========================
    // VELOCIDAD
    // Ejemplo: V150
    // =========================
    else if (comando.startsWith("V"))
    {
      int velocidad = comando.substring(1).toInt();

      velocidad = constrain(velocidad, 0, 255);

      analogWrite(ENA, velocidad);

      Serial.println("Velocidad: " + String(velocidad));
    }

    // =========================
    // COMANDO INVÁLIDO
    // =========================
    else
    {
      Serial.println("Comando no valido");
    }
  }
}