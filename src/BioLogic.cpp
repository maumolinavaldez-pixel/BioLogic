/*
 * BioLogic.cpp - Implementación de la librería BioLogic (v3.1)
 * Con ajustes para ESP32 (WiFi, pines por defecto 8/9)
 */

#include "BioLogic.h"
#include <Arduino.h>
#include <WiFi.h>

BioLogic::BioLogic() {
    _address = BIOLOGIC_DEFAULT_ADDRESS;
    _sdaPin = 8;
    _sclPin = 9;
    _rst = 0; 
    _initialized = false;
    _timeout = 100;
}

BioLogic::BioLogic(uint8_t address) {
    _address = address;
    _sdaPin = 8;
    _sclPin = 9;
    _initialized = false;
    _timeout = 100;
}

BioLogic::BioLogic(uint8_t address, uint8_t sdaPin, uint8_t sclPin) {
    _address = address;
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    _initialized = false;
    _timeout = 100;
}

void BioLogic::begin() {
    Wire.begin(_sdaPin, _sclPin);
    Wire.setClock(400000);
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
    delay(10);
    digitalWrite(_rst, HIGH);
    delay(100);
    WiFi.setTxPower(WIFI_POWER_13dBm);
    _initialized = true;
}

void BioLogic::begin(uint8_t sdaPin, uint8_t sclPin) {
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    Wire.begin(_sdaPin, _sclPin);
    Wire.setClock(400000);
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
    delay(10);
    digitalWrite(_rst, HIGH);
    delay(100);
    WiFi.setTxPower(WIFI_POWER_13dBm);
    _initialized = true;
}

// ===== Métodos privados de comunicación =====
void BioLogic::_sendCommand(uint8_t cmd, uint8_t pin, uint8_t value) {
    Wire.beginTransmission(_address);
    Wire.write(cmd);
    Wire.write(pin);
    Wire.write(value);
    Wire.endTransmission();
    delayMicroseconds(500);
}

void BioLogic::_sendCommand3(uint8_t cmd, uint8_t a, uint8_t b, uint8_t c) {
    Wire.beginTransmission(_address);
    Wire.write(cmd);
    Wire.write(a);
    Wire.write(b);
    Wire.write(c);
    Wire.endTransmission();
    delayMicroseconds(500);
}

void BioLogic::_sendCommandEpoch(uint8_t cmd, uint32_t epoch) {
    Wire.beginTransmission(_address);
    Wire.write(cmd);
    Wire.write(epoch & 0xFF);
    Wire.write((epoch >> 8) & 0xFF);
    Wire.write((epoch >> 16) & 0xFF);
    Wire.write((epoch >> 24) & 0xFF);
    Wire.endTransmission();
    delayMicroseconds(500);
}

uint8_t BioLogic::_readResponse(uint8_t bytes) {
    if (!_initialized) return 0;
    uint32_t startTime = millis();
    uint8_t response = 0;
    while (Wire.available() < bytes && (millis() - startTime) < _timeout) {
        delayMicroseconds(100);
    }
    if (Wire.available() >= bytes) {
        if (bytes == 1) response = Wire.read();
    }
    return response;
}

uint16_t BioLogic::_readResponse16() {
    if (!_initialized) return 0;
    uint32_t startTime = millis();
    uint16_t response = 0;
    while (Wire.available() < 2 && (millis() - startTime) < _timeout) {
        delayMicroseconds(100);
    }
    if (Wire.available() >= 2) {
        uint8_t highByte = Wire.read();
        uint8_t lowByte = Wire.read();
        response = (highByte << 8) | lowByte;
    }
    return response;
}

uint32_t BioLogic::_readResponse32() {
    if (!_initialized) return 0;
    uint32_t startTime = millis();
    uint32_t response = 0;
    while (Wire.available() < 4 && (millis() - startTime) < _timeout) {
        delayMicroseconds(100);
    }
    if (Wire.available() >= 4) {
        response = Wire.read()
                 | (Wire.read() << 8)
                 | (Wire.read() << 16)
                 | (Wire.read() << 24);
    }
    return response;
}

// ===== E/S básicas =====
void BioLogic::pinMode(uint8_t pin, uint8_t mode) {
    _sendCommand(CMD_PIN_MODE, pin, mode);
}

void BioLogic::digitalWrite(uint8_t pin, uint8_t value) {
    _sendCommand(CMD_DIGITAL_WRITE, pin, value);
}

void BioLogic::analogWrite(uint8_t pin, uint8_t value) {
    _sendCommand(CMD_ANALOG_WRITE, pin, value);
}

uint8_t BioLogic::digitalRead(uint8_t pin) {
    _sendCommand(CMD_DIGITAL_READ, pin);
    delayMicroseconds(1000);
    Wire.requestFrom(_address, (uint8_t)1);
    return _readResponse(1);
}

uint16_t BioLogic::analogRead(uint8_t pin) {
    _sendCommand(CMD_ANALOG_READ, pin);
    delayMicroseconds(1000);
    Wire.requestFrom(_address, (uint8_t)2);
    return _readResponse16();
}

// ===== Sensores y actuadores =====
bool BioLogic::readDHT11(uint8_t pin, float &humidity, float &temperature) {
    if (!_initialized) return false;
    _sendCommand(CMD_DHT11_READ, pin, 0);
    delay(300);
    Wire.requestFrom(_address, (uint8_t)4);
    uint32_t startTime = millis();
    while (Wire.available() < 4 && (millis() - startTime) < _timeout) {
        delayMicroseconds(100);
    }
    if (Wire.available() < 4) return false;
    uint8_t data[4];
    for (int i = 0; i < 4; i++) data[i] = Wire.read();
    if (data[0] == 0xFF && data[1] == 0xFF && data[2] == 0xFF && data[3] == 0xFF) return false;
    humidity = data[0] + data[1] / 10.0;
    temperature = data[2] + data[3] / 10.0;
    return true;
}

void BioLogic::servoWrite(uint8_t pin, uint8_t angle) {
    if (angle > 180) angle = 180;
    _sendCommand(CMD_SERVO_WRITE, pin, angle);
}

// ===== Relés =====
void BioLogic::relayOn(uint8_t relayNum) {
    if (relayNum <= r4) digitalWrite(relayNum, HIGH);
}
void BioLogic::relayOff(uint8_t relayNum) {
    if (relayNum <= r4) digitalWrite(relayNum, LOW);
}
void BioLogic::relayToggle(uint8_t relayNum) {
    if (relayNum <= r4) {
        uint8_t current = digitalRead(relayNum);
        digitalWrite(relayNum, !current);
    }
}
void BioLogic::relayTimed(uint8_t relayNum, uint32_t durationMs) {
    if (relayNum <= r4) {
        relayOn(relayNum);
        uint32_t start = millis();
        while (millis() - start < durationMs) delay(1000);
        relayOff(relayNum);
    }
}

// ===== PWM =====
void BioLogic::pwmPercent(uint8_t pwmNum, uint8_t percent) {
    if (pwmNum >= q1 && pwmNum <= q4) {
        if (percent > 100) percent = 100;
        uint8_t val = map(percent, 0, 100, 0, 255);
        analogWrite(pwmNum, val);
    }
}
float BioLogic::readVoltage(uint8_t inputNum) {
    uint16_t adc = analogRead(inputNum);
    return (adc * 3.3) / 4095.0;
}

// ===== Puente H =====
void BioLogic::hBridgeConfig(uint8_t pinA, uint8_t pinB, uint8_t pinPWM) {
    _sendCommand3(CMD_HBRIDGE_CONFIG, pinA, pinB, pinPWM);
}
void BioLogic::hBridgeControl(uint8_t direction, uint8_t speed) {
    if (direction > HBRIDGE_BRAKE) direction = HBRIDGE_STOP;
    _sendCommand3(CMD_HBRIDGE_CONTROL, direction, speed, 0);
}
void BioLogic::hBridgeStop()        { hBridgeControl(HBRIDGE_STOP, 0); }
void BioLogic::hBridgeForward(uint8_t speed) { hBridgeControl(HBRIDGE_FORWARD, speed); }
void BioLogic::hBridgeReverse(uint8_t speed) { hBridgeControl(HBRIDGE_REVERSE, speed); }
void BioLogic::hBridgeBrake()       { hBridgeControl(HBRIDGE_BRAKE, 0); }

// ===== RTC =====
void BioLogic::rtcSetTime(uint32_t epoch) {
    _sendCommandEpoch(CMD_RTC_SET_TIME, epoch);
    delay(10);
}
uint32_t BioLogic::rtcGetTime() {
    _sendCommand(CMD_RTC_GET_TIME, 0);
    delay(5);
    Wire.requestFrom(_address, (uint8_t)4);
    return _readResponse32();
}

void BioLogic::rtcGetDateTime(uint8_t &year, uint8_t &month, uint8_t &day,
                              uint8_t &hour, uint8_t &minute, uint8_t &second,
                              uint8_t &weekday) {
    _sendCommand(CMD_RTC_GET_DATETIME, 0, 0);
    delay(10);
    Wire.requestFrom(_address, (uint8_t)7);
    uint32_t startTime = millis();
    while (Wire.available() < 7 && (millis() - startTime) < _timeout) {
        delayMicroseconds(100);
    }
    if (Wire.available() >= 7) {
        year    = Wire.read();   // año desde 2000
        month   = Wire.read();
        day     = Wire.read();
        hour    = Wire.read();
        minute  = Wire.read();
        second  = Wire.read();
        weekday = Wire.read();   // 1=lunes, 7=domingo
    }
}

void BioLogic::rtcSetAlarm(uint32_t alarmEpoch) {
    _sendCommandEpoch(CMD_RTC_SET_ALARM, alarmEpoch);
    delay(10);
}

uint8_t BioLogic::rtcAlarmStatus() {
    _sendCommand(CMD_RTC_ALARM_STATUS, 0, 0);
    delayMicroseconds(500);
    Wire.requestFrom(_address, (uint8_t)1);
    return _readResponse(1);
}

// ===== Configuración =====
void BioLogic::setAddress(uint8_t newAddress) { _address = newAddress; }
uint8_t BioLogic::getAddress() { return _address; }
void BioLogic::setTimeout(uint32_t timeout) { _timeout = timeout; }
uint32_t BioLogic::getTimeout() { return _timeout; }
void BioLogic::setI2CFrequency(uint32_t frequency) { Wire.setClock(frequency); }