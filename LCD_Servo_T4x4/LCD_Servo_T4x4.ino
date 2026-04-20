//Este codigo permite agregar un angulo por teclado fisico y 
//reproducirlo en servo
#include <Wire.h>                
#include <LiquidCrystal_I2C.h>    
#include <Keypad.h>              
#include <Servo.h>               

// CONFIGURACIÓN DE LA LCD
LiquidCrystal_I2C lcd(0x27,16,2);
// CONFIGURACIÓN DEL SERVO
Servo miServo;
const int pinServo = 13;
// CONFIGURACIÓN DEL TECLADO 4x4
const byte FILAS = 4;
const byte COLUMNAS = 4;
char teclas[FILAS][COLUMNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pinesFilas[FILAS] = {9, 8, 7, 6};
byte pinesColumnas[COLUMNAS] = {5, 4, 3, 2};
Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

// VARIABLES DE TRABAJO
String anguloTexto = "";
int anguloObjetivo = 0;
bool anguloConfirmado = false;

// FUNCIÓN: mostrar encabezado fijo
void mostrarEncabezado() {
  lcd.setCursor(0, 0);
  lcd.print("LAB MICROS ITM ");
}

// FUNCIÓN: mostrar solicitud de ingreso
void mostrarPantallaIngreso() {
  lcd.clear();                    
  mostrarEncabezado();            
  lcd.setCursor(0, 1);
  lcd.print("Ingrese angulo");
}

// FUNCIÓN: actualizar el ángulo escrito en pantalla
void mostrarAnguloDigitado() {
  lcd.setCursor(0, 1);            
  lcd.print("Angulo:          "); 
  lcd.setCursor(8, 1);            
  lcd.print(anguloTexto);
}
// FUNCIÓN: mover el servo de forma directa al ángulo objetivo
void moverServo(int destino) {
  lcd.clear();
  mostrarEncabezado();
  lcd.setCursor(0, 1);
  lcd.print("Servo moviendo");
  miServo.write(destino);
  delay(1000);
  lcd.clear();
  mostrarEncabezado();
  lcd.setCursor(0, 1);
  lcd.print("Mov. terminado");
  delay(1500);
  anguloTexto = "";
  anguloObjetivo = 0;
  anguloConfirmado = false;
  mostrarPantallaIngreso();
}

// SETUP
void setup() {
  lcd.init();
  lcd.backlight();
  miServo.attach(pinServo);
  miServo.write(0);
  mostrarPantallaIngreso();
}

// LOOP PRINCIPAL
void loop() {
  char tecla = teclado.getKey();
  if (tecla) {
    if (tecla >= '0' && tecla <= '9') {
      if (anguloTexto.length() < 3) {
        anguloTexto += tecla;
        mostrarEncabezado();
        mostrarAnguloDigitado();
        anguloConfirmado = false;
      }
    }

    else if (tecla == '*') {
      if (anguloTexto.length() > 0) {
        int valor = anguloTexto.toInt();
        if (valor >= 0 && valor <= 180) {
          anguloObjetivo = valor;
          anguloConfirmado = true;
          lcd.clear();
          mostrarEncabezado();
          lcd.setCursor(0, 1);
          lcd.print("Listo: ");
          lcd.print(anguloObjetivo);
        } 
        else {
          lcd.clear();
          mostrarEncabezado();
          lcd.setCursor(0, 1);
          lcd.print("Rango 0-180");
          delay(1500);
          anguloTexto = "";
          anguloConfirmado = false;
          mostrarPantallaIngreso();
        }
      }
    }

    else if (tecla == '#') {
      if (anguloConfirmado) {
        moverServo(anguloObjetivo);
      } 
      else {
        lcd.clear();
        mostrarEncabezado();
        lcd.setCursor(0, 1);
        lcd.print("Confirme con *");
        delay(1500);
        if (anguloTexto.length() > 0) {
          lcd.clear();
          mostrarEncabezado();
          mostrarAnguloDigitado();
        } else {
          mostrarPantallaIngreso();
        }
      }
    }

    else if (tecla == 'D') {
      anguloTexto = "";
      anguloObjetivo = 0;
      anguloConfirmado = false;
      mostrarPantallaIngreso();
    }
  }
}