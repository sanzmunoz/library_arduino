  // C++ code
/*
Control de servomotor SG90, por potenciometro y 
visualización en un LCD 16X2 i2c el ángulo de cambio (0° - 180°)
CON Modo de grabación con botón para guardar y reproducir 4 ángulos.
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
const int pinServo = 10; //Pin (PWM) de control del servomotor 
const int pinBoton = 22; //pin para boton

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

//Se guarda el último instante de actualización
//la función "millis()"
unsigned long tAnterior = 0; 

//variables para modo grabación de angulos
int angulosGuardados[4]; //Array para almacenar los 4 ángulos
int contadorGrabacion = 0; //Contador de valores guardados (0-4)
bool enModoGrabacion = false; //Flag para modo grabación
bool esperandoConfirmacion = false; //Para esperar la confirmación final

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

//Nuevas funciones
void modoGrabacion(); //Maneja el almacenamiento de 4 valores
void reproducirSecuencia(); //Reproduce la secuencia guardada

//*******************************************************
void setup()
{
  Wire.begin(); //Inicializa el bus I2C
  servoSG90.attach(pinServo);//Se conecta la librería al pin físico
  pinMode(pinBoton, INPUT_PULLUP); //Configura el botón con pull-up interno
  iniciarLCD();//Se configura el LCD , se enciende Luz y crea el simbolo
  imppantbase();//Imprime el formato fijo de la pantalla --> titulos
  //Mueve el servo y muestra el ángulo inicial (0°) para que el sistema
  //arranque coherente
  movServo(anguloActual); 
  impangLCD(anguloActual);
}

void loop()
{
  //Leer estado del botón (con debounce simple)
  bool botonPresionado = (digitalRead(pinBoton) == LOW);
  if (botonPresionado) {
    delay(50); //Debounce simple
    if (digitalRead(pinBoton) == LOW) { //Confirma presión
      if (!enModoGrabacion) {
        //Entrar en modo grabación
        enModoGrabacion = true;
        contadorGrabacion = 0;
        esperandoConfirmacion = false;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Modo Grabacion");
        lcd.setCursor(0,1);
        lcd.print("presione boton");
      } else {
        //Manejar modo grabación
        modoGrabacion();
      }
    }
  }

  //Modo normal solo si no estamos grabando
  if (!enModoGrabacion) {
    //millis() da el tiempo en "ms" desde que se enciende el Arduino
    //Se usa esta técnica para evitar usara la función delay(), un delay() grande pueden bloquear el programa
    unsigned long tActual = millis();

    if(tActual - tAnterior >= periodoMs)
    {
      tAnterior = tActual; //Se guarda el instante de esta actualización
      //Ahora leemos el potenciometro y lo convertimos a ángulo
      int anguloNuevo = leerangdesdepot();
      //Se filtran los ruidos
      //Esto evita que el servo "tiemble" y que el LCD refresque sin necesidad
      if(abs(anguloNuevo - anguloActual) >= ucambiogiro)
      {
        anguloActual = anguloNuevo;//Se actualiza variable de estado
        movServo(anguloActual);//Se aplica el ángulo al servo
        impangLCD(anguloActual);//Se imprime el ángulo en el LCD
      }
    }
  }
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
  lcd.print("Servo MG90");
  lcd.setCursor(0,1);//Fila 1: etiqueta fija, la parte 
                      //variable se actualiza luego
  lcd.print("Angulo: "); 
}

void impangLCD(int angulo)
{
  //Queremos escribir justo despues de "Angulo: "
  lcd.setCursor(8,1); 
   
  //Limpiamos el campo para evitar residuos cuando pasamos
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
}

//Nueva función
void modoGrabacion() {
  if (contadorGrabacion < 4) {
    //Guardar el ángulo actual
    angulosGuardados[contadorGrabacion] = leerangdesdepot();
    contadorGrabacion++;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Guardado: ");
    lcd.print(contadorGrabacion);
    lcd.print("/4");
    lcd.setCursor(0,1);
    lcd.print("Siguiente ajuste");
  } else if (!esperandoConfirmacion) {
    //Después del 4to, mostrar mensaje
    esperandoConfirmacion = true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Presione una vez");
    lcd.setCursor(0,1);
    lcd.print("mas");
  } else {
    //Confirmación final: reproducir
    esperandoConfirmacion = false;
    enModoGrabacion = false;
    reproducirSecuencia();
  }
}

void reproducirSecuencia() {
  //Mostrar valores en LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Valores:");
  
  lcd.setCursor(0,1);
  for (int i = 0; i < 4; i++) {
    lcd.print(angulosGuardados[i]);
    if (i < 3) lcd.print(",");
  }
  //Reproducir en servo
  for (int i = 0; i < 4; i++) {
    movServo(angulosGuardados[i]);
    delay(1000); //Espera 1 segundo entre movimientos
  }
  //Volver al modo normal
  imppantbase();
  impangLCD(anguloActual);
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