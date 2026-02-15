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
#define CMD_SERVO_WRITE   0x07

#define r1  0   // PB12 en BioLogic (Pin 22)
#define r2  1   // PB13 en BioLogic (Pin 21)
#define r3  2   // PB14 en BioLogic (Pin 20)
#define r4  3   // PB15 en BioLogic (Pin 19)

#define q1  4   // PA8 en BioLogic (Pin 27) - TIM1_CH1
#define q2  5   // PA9 en BioLogic (Pin 26) - TIM1_CH2
#define q3  6   // PA10 en BioLogic (Pin 25) - TIM1_CH3
#define q4  7   // PB11 en BioLogic (Pin 13) - TIM2_CH4

#define in1  8   // PA0 en BioLogic (Pin 2) - ADC1
#define in2  9   // PA1 en BioLogic (Pin 3) - ADC2
#define in3  10  // PA2 en BioLogic (Pin 4) - ADC3
#define in4  11  // PA3 en BioLogic (Pin 5) - ADC4
#define in5  12  // PA4 en BioLogic (Pin 6) - ADC5
#define in6  13  // PA5 en BioLogic (Pin 7) - ADC6
#define in7  14  // PA6 en BioLogic (Pin 8) - ADC7
#define in8  15  // PA7 en BioLogic (Pin 9) - ADC8

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
    void servoWrite(uint8_t pin, uint8_t angle);
    
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