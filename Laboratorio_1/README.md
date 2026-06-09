# Laboratorio 1

Conversor binario a decimal con Arduino Mega 2560.

## Descripción

Sistema educativo que demuestra la conversión de código binario a su equivalente decimal. Lee 4 entradas digitales, las refleja en 4 salidas y calcula el valor decimal resultante.

## Características

- **4 Entradas Digitales:** Representan bits 0-3
- **4 Salidas Digitales:** Reflejan el estado de entradas
- **Cálculo Decimal:** Usa desplazamiento de bits (bitwise shift)
- **Serial Output:** Reporta valor decimal por puerto serial
- **Optimizado:** Uso de arrays y bucles para eficiencia

## Hardware

| Componente | Pin Mega |
|-----------|----------|
| Entrada Bit 0 | 22 |
| Entrada Bit 1 | 23 |
| Entrada Bit 2 | 24 |
| Entrada Bit 3 | 25 |
| Salida Bit 0 | 26 |
| Salida Bit 1 | 27 |
| Salida Bit 2 | 28 |
| Salida Bit 3 | 29 |
| Botón Activación | 30 |

## Funcionamiento

1. Usuario conecta entrada binaria (0000 a 1111)
2. Presiona botón en pin 30
3. Arduino calcula: Decimal = b₀·2⁰ + b₁·2¹ + b₂·2² + b₃·2³
4. Refleja bits en salidas
5. Envía resultado por serial

## Cálculo

```
Entrada binaria: 1010 (ejemplo)
Bit 0: 0 × 2⁰ = 0
Bit 1: 1 × 2¹ = 2
Bit 2: 0 × 2² = 0
Bit 3: 1 × 2³ = 8
Resultado: 0 + 2 + 0 + 8 = 10
```

## Operaciones Bitwise

```cpp
Decimal_Value += (Bit_Binary << j);
// << (shift left) desplaza el bit a su posición correcta
// Equivalente a: Bit_Binary × 2^j
```

## Baudrate

```cpp
Serial.begin(9600);  // Compatible con monitores estándar
```

## Notas Técnicas

- Anti-rebote: 50 ms (delay después de soltar botón)
- Bucles optimizados: evita duplicación de código
- Arrays para escalabilidad

---
**Placa:** Arduino Mega 2560  
**Archivo:** `Laboratorio_1.ino`  
**Baudrate:** 9600
