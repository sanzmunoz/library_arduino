/* * Proyecto: Conversor Binario a Decimal Profesional
 * Placa: Arduino Mega 2560
 * Descripción: Lee 4 entradas, las refleja en 4 salidas y calcula el valor decimal.
 */

// Definición de pines usando constantes (ahorra memoria y es más seguro)
const int Data_Inputs[4]  = {22, 23, 24, 25}; // Entradas en bloque digital superior
const int Data_Outputs[4] = {26, 27, 28, 29}; // Salidas a continuación de las entradas
const int Button = 30;                        // Botón de activación

int State_Button = 0;

void setup() {
  // Inicialización de arrays usando un solo bucle
  for(int i = 0; i < 4; i++) {
    pinMode(Data_Inputs[i], INPUT);
    pinMode(Data_Outputs[i], OUTPUT);
  }
  
  pinMode(Button, INPUT);
  
  // En Mega podemos usar hasta 2,000,000 baudios, pero mantendremos 9600 por compatibilidad
  Serial.begin(9600);
  Serial.println("--- Sistema Mega 2560 Iniciado ---");
}

void loop() {
  State_Button = digitalRead(Button);

  if(State_Button == HIGH) {
    int Decimal_Value = 0; 
    
    for(int j = 0; j < 4; j++) {
      // Lectura del bit actual
      int Bit_Binary = digitalRead(Data_Inputs[j]);
      
      // Desplazamiento de bits (Bitwise shift)
      // El bit j se desplaza a su posición correspondiente (1, 2, 4 u 8)
      Decimal_Value += (Bit_Binary << j);
      
      // Reflejo inmediato en la salida
      digitalWrite(Data_Outputs[j], Bit_Binary);
    }

    // Reporte por Serial
    Serial.print("Binario detectado. Valor Decimal: ");
    Serial.println(Decimal_Value);

    // Anti-rebote (Debounce) y espera a soltar el botón
    while(digitalRead(Button) == HIGH); 
    delay(50); 
  }
}