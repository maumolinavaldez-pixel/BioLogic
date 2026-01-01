#include <Wire.h>
#include <Arduino.h>


// Configuración de pines (los pines por defecto del ESP32 son 21-SDA, 22-SCL)
const int sdaPin = 10;
const int sclPin = 3;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESCANER I2C ESP32 DevKit v4 ===");
  Serial.println("Inicializando bus I2C...");
  
  Wire.begin(sdaPin, sclPin);
  Wire.setClock(400000); // Frecuencia estándar 100kHz
  
  delay(1000);
}

void escanearI2C() {
  byte error, address;
  int nDispositivos = 0;

  Serial.println("\nIniciando escaneo...");
  Serial.println("Direccion   | HEX    | Decimal");
  Serial.println("-------------------------------");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print("       | ");
      
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print("     | ");
      Serial.println(address, DEC);
      
      nDispositivos++;
    }
  }

  Serial.println("-------------------------------");
  if (nDispositivos == 0) {
    Serial.println("No se encontraron dispositivos I2C");
  } else {
    Serial.print("Total: ");
    Serial.println(nDispositivos);
  }
  
  // Mostrar direcciones I2C comunes
  Serial.println("\nDirecciones I2C comunes:");
  Serial.println("0x68 - MPU6050/MPU9250 (Acelerómetro/Giroscopo)");
  Serial.println("0x76 - BMP280/BME280 (Sensor presión/temperatura/humedad)");
  Serial.println("0x27 - LCD 16x2 con PCF8574");
  Serial.println("0x3C - OLED SSD1306");
  Serial.println("0x50 - EEPROM");
  Serial.println("0x77 - BMP180/BMP085/BME280 (alternativa)");
}

void loop() {
  escanearI2C();
  delay(10000); // Escanea cada 10 segundos
}*