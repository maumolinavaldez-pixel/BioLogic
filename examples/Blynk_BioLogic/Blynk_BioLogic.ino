#include <BioLogic.h>
#include <Arduino.h>

BioLogic board;

void setup(){
  Serial.begin(115200);
  delay(1000);
  board.begin(10, 3); //SDA , SCL
  delay(100);
  board.pinMode(r1, OUTPUT);
  board.pinMode(r2, OUTPUT);
  board.pinMode(r3, OUTPUT);
  board.pinMode(r4, OUTPUT);
  board.pinMode(q1, OUTPUT);
  board.pinMode(q2, OUTPUT);
  board.pinMode(q3, OUTPUT);
  board.pinMode(q4, OUTPUT);
}

void loop(){
   board.digitalWrite(r1, HIGH);
   board.digitalWrite(r2, HIGH);
   board.digitalWrite(r3, HIGH);
   board.digitalWrite(r4, HIGH);
   board.digitalWrite(q1, HIGH);
   board.digitalWrite(q2, HIGH);
   board.digitalWrite(q3, HIGH);
   board.digitalWrite(q4, HIGH);
   Serial.println("Pulso1");
   delay(1000);
   board.digitalWrite(r1, LOW);
   board.digitalWrite(r2, LOW);
   board.digitalWrite(r3, LOW);
   board.digitalWrite(r4, LOW);
   board.digitalWrite(q1, LOW);
   board.digitalWrite(q2, LOW);
   board.digitalWrite(q3, LOW);
   board.digitalWrite(q4, LOW);
   Serial.println("Pulso2");
   delay(1000);
}