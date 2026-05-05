/*
 * BioLogic.h - Librería genérica para la placa BioLogic (v3.1)
 * Soporta RTC con LSE, alarma programable, fecha/hora desglosada,
 * deep sleep esclavo + wake-up del maestro vía SDA.
 */

#ifndef BIOLOGIC_H
#define BIOLOGIC_H

#include "Arduino.h"
#include "Wire.h"

#define BIOLOGIC_DEFAULT_ADDRESS 0x40

// Comandos I2C
#define CMD_PIN_MODE          0x01
#define CMD_DIGITAL_WRITE     0x02
#define CMD_ANALOG_WRITE      0x03
#define CMD_DIGITAL_READ      0x04
#define CMD_ANALOG_READ       0x05
#define CMD_DHT11_READ        0x06
#define CMD_SERVO_WRITE       0x07
#define CMD_RTC_SET_TIME      0x08
#define CMD_RTC_GET_TIME      0x09
#define CMD_HBRIDGE_CONFIG    0x0A
#define CMD_HBRIDGE_CONTROL   0x0B
#define CMD_RTC_GET_DATETIME  0x0C
#define CMD_RTC_ALARM_STATUS  0x0D
#define CMD_RTC_SET_ALARM     0x0E

// Pines virtuales
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
#define in9  16
#define in10 17

// Direcciones puente H
#define HBRIDGE_STOP    0
#define HBRIDGE_FORWARD 1
#define HBRIDGE_REVERSE 2
#define HBRIDGE_BRAKE   3
#define HBRIDGE_NO_PWM  0xFF

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
    uint8_t _rst;

    void _sendCommand(uint8_t cmd, uint8_t pin, uint8_t value = 0);
    void _sendCommand3(uint8_t cmd, uint8_t a, uint8_t b, uint8_t c);
    void _sendCommandEpoch(uint8_t cmd, uint32_t epoch);
    uint8_t _readResponse(uint8_t bytes = 1);
    uint16_t _readResponse16();
    uint32_t _readResponse32();

public:
    BioLogic();
    BioLogic(uint8_t address);
    BioLogic(uint8_t address, uint8_t sdaPin, uint8_t sclPin);
    void begin();
    void begin(uint8_t sdaPin, uint8_t sclPin);

    // E/S digitales y analógicas
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, uint8_t value);
    void analogWrite(uint8_t pin, uint8_t value);
    uint8_t digitalRead(uint8_t pin);
    uint16_t analogRead(uint8_t pin);

    // Sensores y actuadores
    bool readDHT11(uint8_t pin, float &humidity, float &temperature);
    void servoWrite(uint8_t pin, uint8_t angle);

    // Relés
    void relayOn(uint8_t relayNum);
    void relayOff(uint8_t relayNum);
    void relayToggle(uint8_t relayNum);
    void relayTimed(uint8_t relayNum, uint32_t durationMs);

    // PWM
    void pwmPercent(uint8_t pwmNum, uint8_t percent);
    float readVoltage(uint8_t inputNum);

    // Puente H
    void hBridgeConfig(uint8_t pinA, uint8_t pinB, uint8_t pinPWM = HBRIDGE_NO_PWM);
    void hBridgeControl(uint8_t direction, uint8_t speed = 0);
    void hBridgeStop();
    void hBridgeForward(uint8_t speed = 0);
    void hBridgeReverse(uint8_t speed = 0);
    void hBridgeBrake();

    // RTC – gestión del tiempo
    void rtcSetTime(uint32_t epoch);            // ajustar reloj
    uint32_t rtcGetTime();                      // obtener epoch
    // Obtener fecha/hora desglosada (procesada por el esclavo)
    void rtcGetDateTime(uint8_t &year, uint8_t &month, uint8_t &day,
                        uint8_t &hour, uint8_t &minute, uint8_t &second,
                        uint8_t &weekday);

    // RTC – alarma programable
    void rtcSetAlarm(uint32_t alarmEpoch);      // programar alarma
    uint8_t rtcAlarmStatus();                   // 1=alarma disparada, 0=no

    // Configuración
    void setAddress(uint8_t newAddress);
    uint8_t getAddress();
    void setTimeout(uint32_t timeout);
    uint32_t getTimeout();
    void setI2CFrequency(uint32_t frequency);
};

#endif // BIOLOGIC_H