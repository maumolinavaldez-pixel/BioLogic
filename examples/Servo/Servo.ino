#include <Wire.h>
#include <BioLogic.h>

BioLogic bio; 

void setup() {
  Serial.begin(115200);
  bio.begin();
  delay(100);
  Serial.println("--- BioLogic System Initialized ---");
}

void loop() {
  Serial.println("Iniciando ciclo de prueba...");
  Serial.println("Moviendo Servo en in1 a 0 grados...");
  bio.servoWrite(in1, 0);
  delay(1000);
  Serial.println("Moviendo Servo en in1 a 90 grados...");
  bio.servoWrite(in1, 90);
  delay(1000);
  Serial.println("Moviendo Servo en in1 a 180 grados...");
  bio.servoWrite(in1, 180);
  delay(1000);
  Serial.println("Ciclo completado. Esperando 2 segundos...");
  Serial.println("---------------------------------------");
  delay(2000);
  bio.servoWrite(in1, 0);
}