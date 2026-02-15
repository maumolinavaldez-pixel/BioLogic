#include <BioLogic.h>
#include <Arduino.h>

BioLogic board;

#define DHT_PIN in8

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando comunicación con BioLogic...");
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  delay(50);
  digitalWrite(0, HIGH);
  delay(100);
  board.begin(); // Inicializa I2C y resetea el STM32
  board.setTimeout(500); // 500 ms de timeout máximo
  Serial.println("Listo. Leyendo sensor DHT11...");
  Serial.println();
}

void loop() {
  float humedad = 0.0;
  float temperatura = 0.0;

  // Leer el sensor DHT11 conectado al pin definido
  bool exito = board.readDHT11(DHT_PIN, humedad, temperatura);

  if (exito) {
    Serial.print("Humedad: ");
    Serial.print(humedad, 1); // Mostrar con 1 decimal
    Serial.print(" %\t");
    Serial.print("Temperatura: ");
    Serial.print(temperatura, 1);
    Serial.println(" °C");
  } else {
    Serial.println("Error al leer el sensor DHT11. Verifique:");
    Serial.println("- Conexión del sensor (DATA al pin correspondiente)");
    Serial.println("- Resistencia pull-up de 4.7kΩ a 10kΩ entre DATA y VCC");
    Serial.println("- Alimentación (3.3V o 5V según el módulo)");
    Serial.println("- Que el STM32 esté correctamente alimentado y comunicándose");
  }

  // Esperar 2 segundos antes de la siguiente lectura
  delay(500);
}