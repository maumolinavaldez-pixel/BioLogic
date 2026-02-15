/*
 * BioLogic.h - Librería para controlar la placa BioLogic
 * Diseñada por @teoriademau para programar la placa BioLogic
 * Versión 2.1.0
 * 
 */

#ifndef BIOLOGIC_H
#define BIOLOGIC_H

#include "Arduino.h"
#include "Wire.h"

#define BIOLOGIC_DEFAULT_ADDRESS 0x40

#define CMD_PIN_MODE      0x01
#define CMD_DIGITAL_WRITE 0x02 
#define CMD_ANALOG_WRITE  0x03  
#define CMD_DIGITAL_READ  0x04  
#define CMD_ANALOG_READ   0x05  
#define CMD_DHT11_READ    0x06  

#define r1  0 
#define r2  1 
#define r3  2 
#define r4  3 

#define q1  4 
#define q2  5  
#define q3  6   
#define q4  7   

#define in1  8   
#define in2  9  
#define in3  10  
#define in4  11  
#define in5  12  
#define in6  13  
#define in7  14  
#define in8  15  

#ifndef INPUT
  #define INPUT          0x00
#endif
#ifndef OUTPUT
  #define OUTPUT         0x01
#endif
#ifndef INPUT_PULLUP
  #define INPUT_PULLUP   0x02
#endif
#ifndef LOW
  #define LOW   0x00
#endif
#ifndef HIGH
  #define HIGH  0x01
#endif

#define INPUT_ANALOG   0x03
#define PWM_MODE       0x04

class BioLogic {
private:
    uint8_t _address;
    uint8_t _sdaPin;
    uint8_t _sclPin;
    bool _initialized;
    uint32_t _timeout;
    uint8_t rst;
    void _sendCommand(uint8_t cmd, uint8_t pin, uint8_t value = 0);
    uint8_t _readResponse(uint8_t bytes = 1);
    uint16_t _readResponse16();
    
public:    

    BioLogic();
    BioLogic(uint8_t address);
    BioLogic(uint8_t address, uint8_t sdaPin, uint8_t sclPin);
    void begin();
    void begin(uint8_t sdaPin, uint8_t sclPin);
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, uint8_t value);
    void analogWrite(uint8_t pin, uint8_t value);
    uint8_t digitalRead(uint8_t pin);
    uint16_t analogRead(uint8_t pin);
    bool readDHT11(uint8_t pin, float &humidity, float &temperature);
    void relayOn(uint8_t relayNum);
    void relayOff(uint8_t relayNum);
    void relayToggle(uint8_t relayNum);
    void relayTimed(uint8_t relayNum, uint32_t durationMs);
    void pwmPercent(uint8_t pwmNum, uint8_t percent);
    float readVoltage(uint8_t inputNum);
    void setAddress(uint8_t newAddress);
    uint8_t getAddress();    
    void setTimeout(uint32_t timeout);
    uint32_t getTimeout();
    void setI2CFrequency(uint32_t frequency);
};

#endif // BIOLOGIC_H