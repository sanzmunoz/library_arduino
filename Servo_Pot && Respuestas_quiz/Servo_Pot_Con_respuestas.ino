/*
*********integrantes del grupo:
                                Cesar Augusto Florez Franco
                                Santiago Muñoz Zapata

1) <Wire.h> // controla la comunicacion con el display y es una libreria que se usa para la 
comunicacion I2C la cual sirve para conectar sensores o dispositivos y en este caso el display.
<LiquidCrystal_I2C.h> //se usa para el display y esta libreria sirve para mostrar elementos en 
el tales como letras numeros o simbolos para que el usuario pueda verlos
<Servo.h> // biblioteca para controlar servomotores, facilita el manejo de posición (0-180°) y 
velocidad sin generar pulsos manualmente. este controla el servo con el cual vamos a realizar la simulacion

2) es un identificador unico  que permite saber que dispositivo especifico le esta enviando datos, 
puede cambiar por que depende del modelo del chip controlador el cual puede ser 0x27 y 0x37F, y en caso 
de un error la pantalla se encuende (luz de fondo), pero no muestra ningún texto, quedándose totalmente 
en blanco o con una fila de cuadros sólidos.

3) ¿Qué problema evita esta técnica frente a usar delay(100)?
// evita el bloqueo del procesador. El mills() permite que el programa siga corriendo otras tareas 
mientras espera que se cumpla el tiempo
¿Qué rol cumple tAnterior?
//actua como punto de referencia y almacena el momento exacto en que se ejecuto por ultiam vez.

4) Estabilidad del sistema
a)
Promediado de lecturas (nMuestras = 8):
Sirve para reducir el ruido del potenciómetro promediando varias lecturas y así obtener un valor más estable.
Umbral de cambio (ucambiogiro = 2°):
Evita que el servo se mueva por cambios muy pequeños, lo que ayuda a que no tiemble y que el LCD no se actualice innecesariamente.
b)
Aumentar nMuestras (por ejemplo a 12 o 16) para filtrar mejor el ruido.
Aumentar ucambiogiro (por ejemplo a 3° o 4°) para que el servo solo reaccione a cambios más grandes.

5)error logico: El error es que la función recibe el parámetro pin, pero dentro usa pinPot, por lo que siempre lee el mismo pin y no el que se pasa como argumento

b) codigo :
int leeranalogprom(int pin, int nMuestras) 
{
  long suma = 0;
  for(int i = 0; i < nMuestras; i++) 
  {
    suma += analogRead(pin); // usa el parámetro recibido
    delay(2);
  }
  return (int)(suma / nMuestras);
}

c)Hace que la función sea genérica, y permite leer cualquier pin analógico (A0, A1, A2) sin modificar la función.

6) Consistencia de unidades y rangos
a) constrain() se usa para asegurar que el ángulo no salga del rango permitido, incluso si map() genera un valor fuera del límite.
b) Si angMin = 10 y angMax = 170, el servo solo se moverá entre 10° y 170°, evitando los extremos del servo
*/


#include<Wire.h> //Librería para comunicación i2c
#include<LiquidCrystal_I2C.h> //Librería control LCD I2C
#include<Servo.h> //Librería para generar la señal PWM del servo

//Parámetros 
//Dirección I2C del LCD 
#define LCD_ADDR 0X27 //Sí no conecta cambiar a 0x3F

//Dimensiones físicas del LCD 16 cols x 2 filas
#define LCD_COLS 16
#define LCD_ROWS 2

//Pines de control
const int pinPot = A0; //Pin analógico del potenciómetro
const int pinServo = 6; //Pin (PWM) de control del servomotor 

// ******************QUIZ PUNTO 8 ****************************
const int pinBoton = 2;
bool hold = false;
bool estadoAnterior = HIGH;

//Rango de ángulos permitidos para el servo
const int angMin = 0;
const int angMax = 180; 

//Parametros de estabilidad, minimizamos ruido del potenciometro
//Umbral de cambio de giro, solo se actualiza si el ángulo cambia al 
//menos 2° 
const int ucambiogiro = 2; 

//Actualiza acada 100 ms ---> 10 veces por segundo
const unsigned long periodoMs = 100; 

//Se suaviza por promedio --> reduce las pequeñas variaciones en la
//lectura analógica
const int nMuestras = 8;

//******************Configuración de Objetos**************
//Creamos el objeto LCD con su dirección y tamaño
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

//Creamos el objeto Servo
Servo servoSG90; 

//*****************Variables de Control*********************
int anguloActual = 0; //Almacena el ángulo actual aplicado al servo

//Se guarda el último instante de actualización, utilizaremos 
//la función "millis()"
unsigned long tAnterior = 0; 

//Nota: un LCD permite crear caracteres de 5x8 pixeles
//Este patrón dibuja un pequeño círculo para representar "°"
byte simboloGrado[8] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000,
};

//********Declaramos las funciones************
//Debemos asegurar que el código sea legible
void iniciarLCD();
void imppantbase();//Imprimir pantalla base
int leerangdesdepot(); //Leer ángulo desde el potenciometro
int leeranalogprom(int pin,int nMuestras); //Leer el valor analogico promedio
void movServo(int angulo); //Mover servo
void impangLCD(int angulo); //Imprimir ángulo en pantalla

//*******************************************************
void setup()
{
  Wire.begin(); //Inicializa el bus I2C
  servoSG90.attach(pinServo);//Se conecta la librería al pin físico
  iniciarLCD();//Se configura el LCD , se enciende Luz y crea el simbolo
  imppantbase();//Imprime el formato fijo de la pantalla --> titulos
  //Mueve el servo y muestra el ángulo inicial (0°) para que el sistema
  //arranque coherente
  movServo(anguloActual); 
  impangLCD(anguloActual);

  // ******************QUIZ PUNTO 8 ****************************
  pinMode(pinBoton, INPUT_PULLUP);
}

void loop()
{
  // ******************QUIZ PUNTO 8 ****************************

    bool estadoActual = digitalRead(pinBoton);

  if(estadoAnterior == HIGH && estadoActual == LOW)
  {
    hold = !hold;     //cambia estado HOLD
    delay(50);        //anti-rebote simple
  }
  //*********************************************************
  estadoAnterior = estadoActual;

  //millis() da el tiempo en "ms" desde que se enciende el Arduino
  //Se usa esta técnica para evitar usara la función delay(),
  //ya que delay() grandes pueden bloquear el programa
  unsigned long tActual = millis();

  if(tActual - tAnterior >= periodoMs)
  {
    tAnterior = tActual; //Se guarda el instante de esta actualización
    //Ahora leemos el potenciometro y lo convertimos a ángulo
    int anguloNuevo = leerangdesdepot();
    //Se debe filtrar los cambios muy pequeños --> ruido
    //Esto evita que el servo "tiemble" y que el LCD refresque sin
    //necesidad
    
    // QUIZ PUNTO 8 ***************************************
  if(!hold && abs(anguloNuevo - anguloActual) >= ucambiogiro)
    {
      anguloActual = anguloNuevo;//Se actualiza variable de estado
      movServo(anguloActual);//Se aplica el ángulo al servo
      impangLCD(anguloActual);//Se imprime el ángulo en el LCD
    }
  }
  //*********************
  lcd.setCursor(12,1);

  if(hold)
    lcd.print("HOLD");
  else
    lcd.print("    ");
    /*********************************************/
}

//****Función para el manejo del LCD
void iniciarLCD()
{
  lcd.init(); //Inicializa el LCD--> establece comunicación i2c
              //y prepara el controlador
  lcd.backlight(); //Enciende la luz de fondo del LCD para que sea visible
  lcd.createChar(0,simboloGrado);//Se crea un carácter en la posición 0
                                //de la memoria del LCD, el cual los 
                                //imprime con lcd.write(byte(0))
}

void imppantbase()
{
  lcd.clear(); //Borra la pantalla completa y pone el cursor en (0,0)
  lcd.setCursor(0,0);//Fila 0: título fijo, este no cambia
  lcd.print("Servo SG90 I2C");
  lcd.setCursor(0,1);//Fila 1: etiqueta fija, la parte 
                      //variable se actualiza luego
  lcd.print("Angulo: "); 
}

void impangLCD(int angulo)
{
  //Queremos escribir justo despues de "Angulo: "
  //La palabra "Angulo: " ocupa 8 caracteres
  lcd.setCursor(8,1); 
   
  //Limpiamos el campo para evitar residuos cuando pasamos
  //por ejemplo de 180 a 9, dejar espacios asegura borrar caracteres 
  //anteriores 
  lcd.print("    "); //4 espacios: suficiente para imprimir "180°"
  
  //Volvemos al inicio del campo
  lcd.setCursor(8,1);

  //Alineación a la derecha para que el número se vea ordenado
  //Sí es menor que 100, imprimimos un espacio; si es menor que 10, otro.
  if(angulo < 100) lcd.print(" ");
  if(angulo < 10) lcd.print(" ");

  //Imprimimos el valor numérico
  lcd.print(angulo);

  //Imprimimos el símbolo de grados creado(carácter 0)
  lcd.write(byte(0));

  //******P9: alarma cuando el ángulo es mayor o igual a 150°***********
  lcd.setCursor(10,0);

  if(angulo >= 150)
  {
    lcd.print("ALTA");
  }
  else
  {
    lcd.print("    ");
  }

  // PUNTO 7 DEL QUIZ *******************************************
  int lectura = leeranalogprom(pinPot,nMuestras);

  lcd.setCursor(13,0); 
  lcd.print("    ");     //limpia espacio
  lcd.setCursor(13,0);
  lcd.print(lectura);
}
//--------------------------------------------------------
//             Función para lectura y servo
//--------------------------------------------------------
int leeranalogprom(int pin,int nMuestras)
{
  //El objertivo es reducir el ruido de "analogRead" promediando 
  //varias muestras. "analogRead" devuelve valores de 0 a 1023 (10 bits)
  long suma = 0;

  //Tomamos "nmuestras" leidas y las sumamos
  for(int i = 0; i < nMuestras; i++)
  {
    suma += analogRead(pinPot);
    // Generamos una pequeña pausa para que no sean lecturas identicas
    //por muestreo instantáneo
    delay(2); //2ms
  } 
  //Promedio = suma / nMuestras
  return (int)(suma / nMuestras);
}
int leerangdesdepot()
{
  //1)Lectura analógica del pot 
  int lectura = leeranalogprom(pinPot,nMuestras);

  //2)Convertimos el rango 0 a 1023 al rango 0 a 180
  //Usaremos la función "map(x, inMin, inMax, outMin, outMax)"
  int angulo = map(lectura,0,1023,angMin,angMax);

  //3)Aseguramos los límites pos seguridad, pos si hay lecturas 
  //fuera de rango
  angulo = constrain(angulo,angMin,angMax);

  return angulo;
}

void movServo(int angulo)
{
  //"servo.write(angulo)" genera el pulso adecuado para llevar el servo
  //al ángulo. Internamente, la librería Servo se encarga del temporizado
  servoSG90.write(angulo);
}
