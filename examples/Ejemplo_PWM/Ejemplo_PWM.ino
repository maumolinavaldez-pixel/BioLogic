#include <BioLogic.h>
#include <Arduino.h>

BioLogic board;

void setup() {
  Serial.begin(115200);
  board.begin();
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  delay(100);
  digitalWrite(0, HIGH);
  delay(100);
  board.pinMode(q1, PWM_MODE);
  delay(100);
}

void loop(){
  for(int a = 0; a<= 100; a=a+25){
  board.pwmPercent(q1, a);
  Serial.println(a);
  delay(1000);
   }
  }