#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
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
String claveCorrecta = "1234"; // Contraseña predefinida
String claveIngresada = "";    // Acumula las teclas presionadas

void setup() {                    
  lcd.init();                     
  lcd.backlight();                
  lcd.clear();                    
  lcd.setCursor(0, 0);            
  lcd.print("Ingrese clave:");    
}

void loop() {
  char tecla = teclado.getKey();

  if (tecla != NO_KEY) { 
    if (tecla == '#') {
      // Confirmar y validar la clave
      lcd.clear();
      lcd.setCursor(0, 0);
      if (claveIngresada == claveCorrecta) {
        lcd.print("Acceso correcto");
      } else {
        lcd.print("Clave incorrecta");
      }
      
      delay(2000); // Pausa de 2 segundos para leer el mensaje
      
      // Reiniciar para un nuevo intento
      claveIngresada = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ingrese clave:");
      
    } else if (claveIngresada.length() < 4) {
      // Acumular dígito y mostrar un * en pantalla para ocultarlo
      claveIngresada += tecla;
      lcd.setCursor(claveIngresada.length() - 1, 1); 
      lcd.print('*'); // Si prefieres ver el número cambia '*' por la variable tecla
    }
  }
}