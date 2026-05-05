/*
 * BioLogic.h - Librería para controlar la placa BioLogic
 * Diseñada por @teoriademau para programar la placa BioLogic
 * Versión 2.2.0
 * Añadidos: in9/in10, RTC, Puente H
 */

#ifndef BIOLOGIC_H
#define BIOLOGIC_H

#include "Arduino.h"
#include "Wire.h"

#define BIOLOGIC_DEFAULT_ADDRESS 0x40

// Comandos I2C
#define CMD_PIN_MODE      0x01
#define CMD_DIGITAL_WRITE 0x02 
#define CMD_ANALOG_WRITE  0x03  
#define CMD_DIGITAL_READ  0x04  
#define CMD_ANALOG_READ   0x05  
#define CMD_DHT11_READ    0x06  
#define CMD_SERVO_WRITE   0x07
#define CMD_RTC_SET_TIME  0x08
#define CMD_RTC_GET_TIME  0x09
#define CMD_HBRIDGE_CONFIG  0x0A
#define CMD_HBRIDGE_CONTROL 0x0B

// Pines virtuales del extensor
#define r1  0   // PB12
#define r2  1   // PB13
#define r3  2   // PB14
#define r4  3   // PB15
#define q1  4   // PA8 TIM1_CH1
#define q2  5   // PA9 TIM1_CH2
#define q3  6   // PA10 TIM1_CH3
#define q4  7   // PB11 TIM2_CH4
#define in1  8   // PA0 ADC1
#define in2  9   // PA1 ADC2
#define in3  10  // PA2 ADC3
#define in4  11  // PA3 ADC4
#define in5  12  // PA4 ADC5
#define in6  13  // PA5 ADC6
#define in7  14  // PA6 ADC7
#define in8  15  // PA7 ADC8
#define in9  16  // PB0 ADC9
#define in10 17  // PB1 ADC10

// Direcciones del puente H
#define HBRIDGE_STOP    0
#define HBRIDGE_FORWARD 1
#define HBRIDGE_REVERSE 2
#define HBRIDGE_BRAKE   3
#define HBRIDGE_NO_PWM  0xFF   // Para no usar PWM

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
    void _sendCommand3(uint8_t cmd, uint8_t a, uint8_t b, uint8_t c); // para H-bridge
    void _sendCommandEpoch(uint8_t cmd, uint32_t epoch);               // para RTC
    uint8_t _readResponse(uint8_t bytes = 1);
    uint16_t _readResponse16();
    uint32_t _readResponse32();
    
public:    
    BioLogic();
    BioLogic(uint8_t address);
    BioLogic(uint8_t address, uint8_t sdaPin, uint8_t sclPin);
    void begin();
    void begin(uint8_t sdaPin, uint8_t sclPin);
    
    // E/S básicas
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, uint8_t value);
    void analogWrite(uint8_t pin, uint8_t value);
    uint8_t digitalRead(uint8_t pin);
    uint16_t analogRead(uint8_t pin);
    bool readDHT11(uint8_t pin, float &humidity, float &temperature);
    void servoWrite(uint8_t pin, uint8_t angle);
    // Relés (ya existentes)
    void relayOn(uint8_t relayNum);
    void relayOff(uint8_t relayNum);
    void relayToggle(uint8_t relayNum);
    void relayTimed(uint8_t relayNum, uint32_t durationMs);
    // PWM
    void pwmPercent(uint8_t pwmNum, uint8_t percent);
    // Lectura analógica en voltios
    float readVoltage(uint8_t inputNum);
    // RTC
    void rtcSetTime(uint32_t epoch);
    uint32_t rtcGetTime();
    // Puente H
    void hBridgeConfig(uint8_t pinA, uint8_t pinB, uint8_t pinPWM = HBRIDGE_NO_PWM);
    void hBridgeControl(uint8_t direction, uint8_t speed = 0);
    void hBridgeStop();
    void hBridgeForward(uint8_t speed = 0);
    void hBridgeReverse(uint8_t speed = 0);
    void hBridgeBrake();
    // Configuración
    void setAddress(uint8_t newAddress);
    uint8_t getAddress();    
    void setTimeout(uint32_t timeout);
    uint32_t getTimeout();
    void setI2CFrequency(uint32_t frequency);
};

#endif // BIOLOGIC_H