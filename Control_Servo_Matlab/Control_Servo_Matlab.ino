//Profesor Juan Guillermo Patiño Vengoechea
//Facultad de Ingenirias - Mecatrónica
//Microcotroladores ----- 24/02/2026
//********************************************************************************************
//                                 This is the Way
//********************************************************************************************
#include <Servo.h>  
/*
  =========================  EXPLICACIÓN PWM  =========================

  ¿Qué es PWM?
  - PWM (Pulse Width Modulation / Modulación por Ancho de Pulso) es una técnica donde
    una señal digital alterna entre HIGH y LOW, y controlamos "algo" cambiando el
    TIEMPO que permanece en HIGH en cada ciclo (ancho de pulso).

  Dos usos comunes (y muy importantes de diferenciar):
  1) PWM "clásico" (LED / motor DC con analogWrite):
     - La frecuencia suele ser fija (ej. ~490 Hz en Arduino UNO en varios pines).
     - Se cambia el DUTY CYCLE (porcentaje de tiempo en HIGH) para variar potencia promedio.
       Ej.: LED más brillante o motor más rápido.

  2) Señal para SERVOS (control por ANCHO DE PULSO):
     - Aquí lo que importa NO es la potencia promedio, sino el ANCHO del pulso en microsegundos.
     - El servo recibe un pulso cada ~20 ms (≈50 Hz).
     - El ancho típico del pulso es:
         ~1000 µs (1.0 ms)  -> cerca de 0°
         ~1500 µs (1.5 ms)  -> cerca de 90°
         ~2000 µs (2.0 ms)  -> cerca de 180°

  Importante:
  - En Arduino UNO, los pines PWM para analogWrite() son 3, 5, 6, 9, 10, 11.
  - PERO la librería Servo.h NO usa analogWrite(); genera internamente los pulsos del servo.
  - Aun así, por buenas prácticas, usamos el pin 9 o cualquiera que sea PWM.

  ================================================================================ 
*/

Servo Griper;                      // Objeto servo: permite enviar posiciones al servo.

const int Pin_Servo_Griper = 9;    // Pin de SEÑAL del servo (pulsos generados por Servo.h)

void setup()
{
  Serial.begin(9600);
  // Inicia la comunicación serial con el PC (MATLAB).
  // MATLAB enviará números (ángulos) y Arduino los leerá para mover el servo.

  Griper.attach(Pin_Servo_Griper);
  // "Conecta" el servo a este pin.
  // Desde aquí, Griper.write(angulo) genera los pulsos (~50 Hz) con ancho 1–2 ms aprox.
}

void loop()
{
  if (Serial.available() > 0)
  // Si hay datos esperando en el buffer serial, entonces podemos leer.
  {
    int Angle = Serial.parseInt();
    // Lee un entero del flujo serial.
    // parseInt busca dígitos y construye el número (ej: "90\n" -> 90).
    // Si no llegan más dígitos, puede esperar un tiempo (timeout) antes de devolver.

    if (Angle >= 0 && Angle <= 180)
    // Validamos rango típico del servo para evitar comandos inválidos.
    {
      Griper.write(Angle);
      // Envía la posición al servo.
      // Internamente se traduce a un ancho de pulso (aprox. 1000–2000 µs).

      Serial.println("Ángulo de movimiento: " + String(Angle));
      // Mensaje de confirmación hacia MATLAB (útil para depuración).
    }
  }
}
