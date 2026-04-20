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
char teclaPresionada = NO_KEY;    
int contadorTeclas = 0; // Cuenta las teclas presionadas
int posicionX = 7;      // Posición en la pantalla LCD (columna)

void setup() {                    
  lcd.init();                     
  lcd.backlight();                
  lcd.clear();                    
  lcd.setCursor(0, 0);            
  lcd.print("Teclado 4x4");       
  lcd.setCursor(0, 1);            
  lcd.print("Tecla: ");           
}

void loop() {
  teclaPresionada = teclado.getKey();

  if (teclaPresionada != NO_KEY) { 
    // Si es la primera tecla, limpiamos la zona de teclas
    if (contadorTeclas == 0) {
      lcd.setCursor(7, 1);
      lcd.print("        "); 
    }

    // Imprimir la tecla en la posición actual
    lcd.setCursor(posicionX, 1);
    lcd.print(teclaPresionada);
    
    // Avanzar posición y aumentar contador
    posicionX++;
    contadorTeclas++;

    // Verificar si llegamos al límite de 8 caracteres
    if (contadorTeclas >= 8) {
      delay(1000); // Pausa para que se alcance a ver el octavo caracter
      
      // Limpiar y restaurar la pantalla para comenzar de nuevo
      lcd.clear();                    
      lcd.setCursor(0, 0);            
      lcd.print("Teclado 4x4");       
      lcd.setCursor(0, 1);            
      lcd.print("Tecla: ");  
      
      // Reiniciar variables
      contadorTeclas = 0;
      posicionX = 7;
    } else {
      delay(250);
    }
  }
}