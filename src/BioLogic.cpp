/*
 * BioLogic.cpp - Implementación de la librería BioLogic
 * Diseñada por @teoriademau para programar la placa BioLogic
 * 
 * La placa BioLogic es un dispositivo basado en STM32 Bluepill
 * que funciona como esclavo I2C con las siguientes características:
 * - 4 Salidas Digitales (r1-r4)
 * - 4 Salidas PWM (q1-q4)
 * - 8 Entradas Digitales/Analógicas (in1-in8)
 */

#include "BioLogic.h"
#include <Arduino.h>

BioLogic::BioLogic() {
    _address = BIOLOGIC_DEFAULT_ADDRESS;
    _sdaPin = 10;
    _sclPin = 3;
    rst = 0; 
    _initialized = false;
    _timeout = 100;      // Timeout de 100ms por defecto
}

BioLogic::BioLogic(uint8_t address) {
    _address = address;
    _sdaPin = 10;
    _sclPin = 3;
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
    Wire.setClock(400000); // Fast Mode 400kHz
    pinMode(rst, OUTPUT);
    digitalWrite(rst, LOW);
    delay(10);
    digitalWrite(rst,HIGH);
    delay(100);
    _initialized = true;
}

void BioLogic::begin(uint8_t sdaPin, uint8_t sclPin) {
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    Wire.begin(_sdaPin, _sclPin);
    Wire.setClock(400000);
    pinMode(rst, OUTPUT);
    digitalWrite(rst, LOW);
    delay(10);
    digitalWrite(rst,HIGH);
    delay(100);
    
    _initialized = true;
    delay(100);
}

void BioLogic::_sendCommand(uint8_t cmd, uint8_t pin, uint8_t value) {
    
    Wire.beginTransmission(_address);
    Wire.write(cmd);      // Comando (0x01-0x05)
    Wire.write(pin);      // Pin virtual (0-15)
    Wire.write(value);    // Valor
    Wire.endTransmission();
    delayMicroseconds(500);
}

uint8_t BioLogic::_readResponse(uint8_t bytes) {
    if (!_initialized) return 0;
    
    uint32_t startTime = millis();
    uint8_t response = 0;
    
    // Esperar respuesta con timeout
    while (Wire.available() < bytes && (millis() - startTime) < _timeout) {
        delayMicroseconds(100);
    }
    
    if (Wire.available() >= bytes) {
        if (bytes == 1) {
            response = Wire.read();
        }
    }
    
    return response;
}

uint16_t BioLogic::_readResponse16() {
    if (!_initialized) return 0;
    
    uint32_t startTime = millis();
    uint16_t response = 0;
    
    // Esperar 2 bytes con timeout
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

void BioLogic::pinMode(uint8_t pin, uint8_t mode) {    
    _sendCommand(0x01, pin, mode);
}

void BioLogic::digitalWrite(uint8_t pin, uint8_t value) {
   
    _sendCommand(0x02, pin, value);

}

void BioLogic::analogWrite(uint8_t pin, uint8_t value) {
    
    _sendCommand(0x03, pin, value);
    
}

uint8_t BioLogic::digitalRead(uint8_t pin) {    
    _sendCommand(0x04, pin);
    delayMicroseconds(1000);
    
    Wire.requestFrom(_address, (uint8_t)1);
    uint8_t value = _readResponse(1);
    
    return value;
}

uint16_t BioLogic::analogRead(uint8_t pin) {
    
    _sendCommand(0x05, pin);
    delayMicroseconds(1000); // 1ms para procesamiento
    
    Wire.requestFrom(_address, (uint8_t)2);
    uint16_t value = _readResponse16();    
    return value;
}

void BioLogic::relayOn(uint8_t relayNum) {
    if (relayNum <= r4) {
        digitalWrite(relayNum, HIGH);   
    } 
}

void BioLogic::relayOff(uint8_t relayNum) {
    if (relayNum <= r4) {
        digitalWrite(relayNum, LOW);
    } 
    }

void BioLogic::relayToggle(uint8_t relayNum) {
    if (relayNum <= r4) {
        uint8_t currentState = digitalRead(relayNum);
        digitalWrite(relayNum, !currentState);
    }
}

void BioLogic::relayTimed(uint8_t relayNum, uint32_t durationMs) {
    if (relayNum <= r4) {
        relayOn(relayNum);
        uint32_t startTime = millis();
        while (millis() - startTime < durationMs) {
        delay(1000);
            }
        }
        
    relayOff(relayNum);
}

void BioLogic::pwmPercent(uint8_t pwmNum, uint8_t percent) {
        //analogWrite(pwmNum, percent);
        //_sendCommand(0x03, pwmNum, percent);
        if (pwmNum >= q1 && pwmNum <= q4) {
        if (percent > 100) percent = 100;
        uint8_t value = map(percent, 0, 100, 0, 255);
        analogWrite(pwmNum, value);
    }
}

float BioLogic::readVoltage(uint8_t inputNum) {
        uint16_t adcValue = analogRead(inputNum);
        float voltage = (adcValue * 3.3) / 4095.0;
        return voltage;
}


void BioLogic::setAddress(uint8_t newAddress) {
    _address = newAddress;
}

uint8_t BioLogic::getAddress() {
    return _address;
}

void BioLogic::setTimeout(uint32_t timeout) {
    _timeout = timeout;
    
}

uint32_t BioLogic::getTimeout() {
    return _timeout;
}

void BioLogic::setI2CFrequency(uint32_t frequency) {
    
    Wire.setClock(frequency);
    
}