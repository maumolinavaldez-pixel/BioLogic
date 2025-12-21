# BioLogic Library

Librería Arduino para controlar la placa **BioLogic** (STM32 como esclavo I2C) desde ESP32-C3.

## Instalación

Descarga el archivo `BioLogic.zip` e instálalo en Arduino IDE:
`Sketch → Include Library → Add .ZIP Library...`

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

```cpp
// Inicialización
board.begin(SDA_PIN, SCL_PIN);
board.isConnected();

// Control básico (Arduino compatible)
board.pinMode(pin, mode);
board.digitalWrite(pin, value);
board.digitalRead(pin);
board.analogWrite(pin, value);
board.analogRead(pin);

// Funciones específicas
board.relayOn(relayNum);        // 0-3
board.relayOff(relayNum);
board.pwmPercent(pwmNum, %);    // 4-7, 0-100%
board.readVoltage(inputNum);    // 8-15, retorna 0-3.3V

// Utilidades
board.setAddress(newAddress);
board.setTimeout(ms);
```

## Modos de Pines

```cpp
INPUT           // Entrada sin pull-up
OUTPUT          // Salida digital
INPUT_PULLUP    // Entrada con pull-up interna
INPUT_ANALOG    // Entrada analógica (ADC)
PWM_MODE        // Salida PWM
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
```

---

Diseñado por **@teoriademau** para programar la placa **BioLogic**.

Para más información: [https://github.com/teoriademau/BioLogic](https://github.com/maumolinavaldez-pixel/BioLogic/)
                       https://t.me/teoriademau
