// Versión 2: Guarda 4 ángulos en slots A, B, C, D
// Flujo: seleccionar slot (A/B/C/D) → digitar ángulo → confirmar con * → ejecutar con #

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

// CONFIGURACIÓN DE LA LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

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
byte pinesFilas[FILAS]    = {9, 8, 7, 6};
byte pinesColumnas[COLUMNAS] = {5, 4, 3, 2};
Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

// SLOTS DE ÁNGULOS  (índice 0=A, 1=B, 2=C, 3=D)
int  angulos[4]    = {-1, -1, -1, -1};  // -1 = vacío
bool guardado[4]   = {false, false, false, false};

// ESTADO ACTUAL
int    slotActual      = -1;   // slot seleccionado (-1 = ninguno)
String anguloTexto     = "";
bool   anguloConfirmado = false;

// ── HELPERS ──────────────────────────────────────────────────────────────────

void mostrarEncabezado() {
  lcd.setCursor(0, 0);
  lcd.print("LAB MICROS ITM  ");
}

// Pantalla de bienvenida: muestra qué slots tienen ángulo guardado
void mostrarPantallaInicio() {
  lcd.clear();
  mostrarEncabezado();
  lcd.setCursor(0, 1);
  // Muestra un indicador por slot, ej: "A:90 B:-- C:45 D:--"
  // (caben 4 slots en 16 caracteres: "A:xxx B:xxx ...") 
  // Simplificado a 2 slots por renglón para mayor legibilidad
  lcd.print("Elige slot:A B C D");
}

// Pantalla luego de seleccionar un slot
void mostrarPantallaSlot() {
  lcd.clear();
  mostrarEncabezado();
  lcd.setCursor(0, 1);
  char nombreSlot = 'A' + slotActual;
  if (guardado[slotActual]) {
    lcd.print("Slot ");
    lcd.print(nombreSlot);
    lcd.print(":");
    lcd.print(angulos[slotActual]);
    lcd.print((char)223);   // símbolo °
    lcd.print(" *=nuevo");
  } else {
    lcd.print("Slot ");
    lcd.print(nombreSlot);
    lcd.print(": vacio");
  }
}

void mostrarAnguloDigitado() {
  char nombreSlot = 'A' + slotActual;
  lcd.setCursor(0, 1);
  lcd.print("Slot ");
  lcd.print(nombreSlot);
  lcd.print(": ");
  lcd.print(anguloTexto);
  lcd.print("   ");  // limpia caracteres sobrantes
}

void moverServo(int destino) {
  lcd.clear();
  mostrarEncabezado();
  lcd.setCursor(0, 1);
  lcd.print("Moviendo a ");
  lcd.print(destino);
  lcd.print((char)223);
  miServo.write(destino);
  delay(1000);
  lcd.clear();
  mostrarEncabezado();
  lcd.setCursor(0, 1);
  lcd.print("Listo! ");
  lcd.print(destino);
  lcd.print((char)223);
  delay(1500);
  // Regresa a la pantalla del slot activo
  mostrarPantallaSlot();
}

// ── SETUP ────────────────────────────────────────────────────────────────────
void setup() {
  lcd.init();
  lcd.backlight();
  miServo.attach(pinServo);
  miServo.write(0);
  mostrarPantallaInicio();
}

// ── LOOP ─────────────────────────────────────────────────────────────────────


void loop() {
  char tecla = teclado.getKey();
  if (!tecla) return;

  // ── Selección de slot con A / B / C / D ──────────────────────────────────
  if (tecla == 'A' || tecla == 'B' || tecla == 'C' || tecla == 'D') {
    slotActual       = tecla - 'A';   // A→0, B→1, C→2, D→3
    anguloTexto      = "";
    anguloConfirmado = false;
    mostrarPantallaSlot();
    return;
  }

  // Si aún no se ha seleccionado un slot, ignorar otras teclas
  if (slotActual < 0) {
    lcd.clear();
    mostrarEncabezado();
    lcd.setCursor(0, 1);
    lcd.print("Elige A B C o D");
    delay(1200);
    mostrarPantallaInicio();
    return;
  }

  // ── Dígitos 0-9 ──────────────────────────────────────────────────────────
  if (tecla >= '0' && tecla <= '9') {
    if (anguloTexto.length() < 3) {
      anguloTexto += tecla;
      anguloConfirmado = false;
      mostrarEncabezado();
      mostrarAnguloDigitado();
    }
    return;
  }

  // ── * : confirmar y GUARDAR en el slot ───────────────────────────────────
  if (tecla == '*') {
    if (anguloTexto.length() > 0) {
      int valor = anguloTexto.toInt();
      if (valor >= 0 && valor <= 180) {
        angulos[slotActual]  = valor;
        guardado[slotActual] = true;
        anguloConfirmado     = true;
        lcd.clear();
        mostrarEncabezado();
        lcd.setCursor(0, 1);
        char nombreSlot = 'A' + slotActual;
        lcd.print("Slot ");
        lcd.print(nombreSlot);
        lcd.print(" = ");
        lcd.print(valor);
        lcd.print((char)223);
        lcd.print(" OK");
        delay(1500);
        mostrarPantallaSlot();
      } else {
        lcd.clear();
        mostrarEncabezado();
        lcd.setCursor(0, 1);
        lcd.print("Rango: 0-180");
        delay(1500);
        anguloTexto      = "";
        anguloConfirmado = false;
        mostrarPantallaSlot();
      }
    }
    return;
  }

  // ── # : EJECUTAR ángulo del slot actual ──────────────────────────────────
  if (tecla == '#') {
    if (guardado[slotActual]) {
      moverServo(angulos[slotActual]);
    } else {
      lcd.clear();
      mostrarEncabezado();
      lcd.setCursor(0, 1);
      lcd.print("Slot vacio!");
      delay(1500);
      mostrarPantallaSlot();
    }
    return;
  }
}
