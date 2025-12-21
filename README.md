# BioLogic Library

Librería Arduino para controlar la placa **BioLogic**, diseñada por Mauricio Molina Valdez (@teoriademau).

## Instalación

Descarga el archivo `BioLogic.zip` e instálalo en Arduino IDE:
`Sketch → Include Library → Add .ZIP Library...`

## Entradas y Salidas por Software

La placa BioLogic ofrece 16 pines virtuales organizados en 3 grupos:

### 🔌 **Salidas Digitales (Relés)**
```
Pin 0: r1 - Salida digital 1
Pin 1: r2 - Salida digital 2  
Pin 2: r3 - Salida digital 3
Pin 3: r4 - Salida digital 4
```

### 🎛️ **Salidas PWM (Control de intensidad)**
```
Pin 4: q1 - Salida PWM 1 (0-100%)
Pin 5: q2 - Salida PWM 2 (0-100%)
Pin 6: q3 - Salida PWM 3 (0-100%)
Pin 7: q4 - Salida PWM 4 (0-100%)
```

### 🔍 **Entradas Digitales/Analógicas**
```
Pin 8:  in1 - Entrada 1 (digital/analógica)
Pin 9:  in2 - Entrada 2 (digital/analógica)
Pin 10: in3 - Entrada 3 (digital/analógica)
Pin 11: in4 - Entrada 4 (digital/analógica)
Pin 12: in5 - Entrada 5 (digital/analógica)
Pin 13: in6 - Entrada 6 (digital/analógica)
Pin 14: in7 - Entrada 7 (digital/analógica)
Pin 15: in8 - Entrada 8 (digital/analógica)
```

## Uso Básico

```cpp
#include <BioLogic.h>

BioLogic board;

void setup() {
  Serial.begin(115200);
  board.begin(8, 9);  // SDA=GPIO8, SCL=GPIO9
  
  board.pinMode(0, OUTPUT);   // r1 como salida
  board.pinMode(4, PWM_MODE); // q1 como PWM
  board.pinMode(8, INPUT_PULLUP); // in1 con pull-up
  
  if (board.isConnected()) {
    Serial.println("BioLogic conectada");
  }
}

void loop() {
  // Control de relés
  board.relayOn(0);  // Encender r1
  delay(1000);
  board.relayOff(0); // Apagar r1
  delay(1000);
  
  // Control PWM
  board.pwmPercent(4, 50); // q1 al 50%
  delay(2000);
  board.pwmPercent(4, 0);  // q1 al 0%
  
  // Lectura
  bool estado = board.digitalRead(8);
  float voltaje = board.readVoltage(9);
  
  Serial.print("Entrada: ");
  Serial.print(estado ? "HIGH" : "LOW");
  Serial.print(" | Voltaje: ");
  Serial.print(voltaje, 2);
  Serial.println("V");
}
```

## Funciones Disponibles

### **Inicialización**
```cpp
board.begin(SDA_PIN, SCL_PIN);  // Inicializar comunicación
board.isConnected();             // Verificar conexión (retorna bool)
```

### **Control Básico (Arduino compatible)**
```cpp
board.pinMode(pin, mode);        // Configurar modo del pin
board.digitalWrite(pin, value);  // Escribir digital (HIGH/LOW)
board.digitalRead(pin);          // Leer digital (retorna HIGH/LOW)
board.analogWrite(pin, value);   // Escribir PWM (0-255)
board.analogRead(pin);           // Leer ADC (0-4095)
```

### **Funciones Específicas BioLogic**
```cpp
board.relayOn(relayNum);        // Encender relé (0-3 para r1-r4)
board.relayOff(relayNum);       // Apagar relé
board.pwmPercent(pwmNum, %);    // Control PWM por % (4-7 para q1-q4)
board.readVoltage(inputNum);    // Leer voltaje (8-15 para in1-in8)
```

### **Utilidades y Diagnóstico**
```cpp
board.setAddress(newAddress);   // Cambiar dirección I2C
board.setTimeout(ms);           // Configurar timeout comunicación
board.getVersion();             // Obtener versión librería
board.getAuthor();              // Obtener autor (@teoriademau)
board.testConnection();         // Test completo de comunicación
```

## Modos de Pines

```cpp
INPUT           // Entrada sin pull-up
OUTPUT          // Salida digital
INPUT_PULLUP    // Entrada con pull-up interna
INPUT_ANALOG    // Entrada analógica (ADC)
PWM_MODE        // Salida PWM
```

## Rangos y Valores

### **Para digitalWrite():**
```cpp
HIGH    // 1, true, 0xFF
LOW     // 0, false, 0x00
```

### **Para analogWrite():**
```cpp
0   → 0% PWM
127 → 50% PWM
255 → 100% PWM
```

### **Para analogRead():**
```cpp
0     → 0V
2048  → 1.65V
4095  → 3.3V
```

### **Para readVoltage():**
```cpp
0.00V a 3.30V  // Precisión: 0.8mV (3.3V / 4095)
```

## Ejemplos Incluidos

Accede a `File → Examples → BioLogic` para ver:
- **Basic_Control** - Control básico de todas las funciones
- **PWM_Fade** - Efectos de fade en PWM
- **Input_Output** - Sistema completo entradas/salidas

## Conexiones

```
ESP32-C3  →  BioLogic
GPIO8     →  SDA
GPIO9     →  SCL
GND       →  GND
(PUEDE SER CONECTADO CON CUALQUIER MICROCONTROLADOR CON LA DIRECCION I2C: 0X40
```

---

**Diseñado por:** Mauricio Molina Valdez (@teoriademau)  
**Placa:** BioLogic (STM32 Bluepill como esclavo I2C)  
**Repositorio:** [https://github.com/maumolinavaldez-pixel/BioLogic](https://github.com/maumolinavaldez-pixel/BioLogic/)  
**Contacto:** https://t.me/teoriademau
