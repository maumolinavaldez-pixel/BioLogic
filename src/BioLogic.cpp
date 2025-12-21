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

// ============================================
// CONSTRUCTORES
// ============================================

BioLogic::BioLogic() {
    _address = BIOLOGIC_DEFAULT_ADDRESS;
    _sdaPin = 21;        // Pin SDA por defecto para ESP32
    _sclPin = 22;        // Pin SCL por defecto para ESP32
    _initialized = false;
    _timeout = 100;      // Timeout de 100ms por defecto
}

BioLogic::BioLogic(uint8_t address) {
    _address = address;
    _sdaPin = 21;
    _sclPin = 22;
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

// ============================================
// MÉTODOS DE INICIALIZACIÓN
// ============================================

void BioLogic::begin() {
    // Inicializar comunicación I2C
    Wire.begin(_sdaPin, _sclPin);
    Wire.setClock(100000); // Frecuencia estándar 100kHz
    
    _initialized = true;
    
    // Pequeña pausa para inicialización de la placa BioLogic
    delay(100);
    
    #ifdef BIOLOGIC_DEBUG
    Serial.println("BioLogic: Librería inicializada");
    Serial.print("  Dirección: 0x");
    Serial.println(_address, HEX);
    Serial.print("  SDA: GPIO");
    Serial.print(_sdaPin);
    Serial.print(", SCL: GPIO");
    Serial.println(_sclPin);
    #endif
}

void BioLogic::begin(uint8_t sdaPin, uint8_t sclPin) {
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    
    // Inicializar comunicación I2C con pines personalizados
    Wire.begin(_sdaPin, _sclPin);
    Wire.setClock(100000);
    
    _initialized = true;
    delay(100);
    
    #ifdef BIOLOGIC_DEBUG
    Serial.println("BioLogic: Librería inicializada con pines personalizados");
    Serial.print("  SDA: GPIO");
    Serial.print(_sdaPin);
    Serial.print(", SCL: GPIO");
    Serial.println(_sclPin);
    #endif
}

// ============================================
// MÉTODOS PRIVADOS DE COMUNICACIÓN
// ============================================

void BioLogic::_sendCommand(uint8_t cmd, uint8_t pin, uint8_t value) {
    if (!_initialized) {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("BioLogic ERROR: Librería no inicializada. Llama a begin() primero.");
        #endif
        return;
    }
    
    Wire.beginTransmission(_address);
    Wire.write(cmd);      // Comando (0x01-0x05)
    Wire.write(pin);      // Pin virtual (0-15)
    Wire.write(value);    // Valor
    Wire.endTransmission();
    
    // Pequeña pausa para procesamiento en la placa BioLogic
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

// ============================================
// FUNCIONES ARDUINO COMPATIBLES
// ============================================

void BioLogic::pinMode(uint8_t pin, uint8_t mode) {
    if (pin > 15) {
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic ERROR: Pin ");
        Serial.print(pin);
        Serial.println(" no válido. Usa 0-15.");
        #endif
        return;
    }
    
    _sendCommand(0x01, pin, mode);
    
    #ifdef BIOLOGIC_DEBUG
    Serial.print("BioLogic: pinMode(");
    Serial.print(pin);
    Serial.print(", ");
    Serial.print(mode);
    Serial.println(")");
    #endif
}

void BioLogic::digitalWrite(uint8_t pin, uint8_t value) {
    if (pin > 15) {
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic ERROR: Pin ");
        Serial.print(pin);
        Serial.println(" no válido. Usa 0-15.");
        #endif
        return;
    }
    
    _sendCommand(0x02, pin, value);
    
    #ifdef BIOLOGIC_DEBUG
    Serial.print("BioLogic: digitalWrite(");
    Serial.print(pin);
    Serial.print(", ");
    Serial.print(value ? "HIGH" : "LOW");
    Serial.println(")");
    #endif
}

void BioLogic::analogWrite(uint8_t pin, uint8_t value) {
    if (pin > 15) {
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic ERROR: Pin ");
        Serial.print(pin);
        Serial.println(" no válido. Usa 0-15.");
        #endif
        return;
    }
    
    _sendCommand(0x03, pin, value);
    
    #ifdef BIOLOGIC_DEBUG
    Serial.print("BioLogic: analogWrite(");
    Serial.print(pin);
    Serial.print(", ");
    Serial.print(value);
    Serial.print(") [");
    Serial.print(map(value, 0, 255, 0, 100));
    Serial.println("%]");
    #endif
}

uint8_t BioLogic::digitalRead(uint8_t pin) {
    if (pin > 15) {
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic ERROR: Pin ");
        Serial.print(pin);
        Serial.println(" no válido. Usa 0-15.");
        #endif
        return 0;
    }
    
    _sendCommand(0x04, pin);
    delayMicroseconds(1000); // 1ms para procesamiento
    
    Wire.requestFrom(_address, (uint8_t)1);
    uint8_t value = _readResponse(1);
    
    #ifdef BIOLOGIC_DEBUG
    Serial.print("BioLogic: digitalRead(");
    Serial.print(pin);
    Serial.print(") = ");
    Serial.println(value ? "HIGH" : "LOW");
    #endif
    
    return value;
}

uint16_t BioLogic::analogRead(uint8_t pin) {
    if (pin > 15) {
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic ERROR: Pin ");
        Serial.print(pin);
        Serial.println(" no válido. Usa 0-15.");
        #endif
        return 0;
    }
    
    _sendCommand(0x05, pin);
    delayMicroseconds(1000); // 1ms para procesamiento
    
    Wire.requestFrom(_address, (uint8_t)2);
    uint16_t value = _readResponse16();
    
    #ifdef BIOLOGIC_DEBUG
    Serial.print("BioLogic: analogRead(");
    Serial.print(pin);
    Serial.print(") = ");
    Serial.print(value);
    Serial.print(" (");
    Serial.print((value * 3.3) / 4095.0, 2);
    Serial.println("V)");
    #endif
    
    return value;
}

// ============================================
// FUNCIONES ESPECÍFICAS DE BIOLOGIC
// ============================================

void BioLogic::relayOn(uint8_t relayNum) {
    if (relayNum <= r4) {
        digitalWrite(relayNum, HIGH);
        
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic: Relé ");
        Serial.print(relayNum + 1);
        Serial.println(" ENCENDIDO");
        #endif
    } else {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("BioLogic ERROR: Número de relé no válido. Usa 0-3 (r1-r4).");
        #endif
    }
}

void BioLogic::relayOff(uint8_t relayNum) {
    if (relayNum <= r4) {
        digitalWrite(relayNum, LOW);
        
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic: Relé ");
        Serial.print(relayNum + 1);
        Serial.println(" APAGADO");
        #endif
    } else {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("BioLogic ERROR: Número de relé no válido. Usa 0-3 (r1-r4).");
        #endif
    }
}

void BioLogic::relayToggle(uint8_t relayNum) {
    if (relayNum <= r4) {
        uint8_t currentState = digitalRead(relayNum);
        digitalWrite(relayNum, !currentState);
        
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic: Relé ");
        Serial.print(relayNum + 1);
        Serial.print(" alternado a ");
        Serial.println(!currentState ? "ENCENDIDO" : "APAGADO");
        #endif
    }
}

void BioLogic::pwmPercent(uint8_t pwmNum, uint8_t percent) {
    if (pwmNum >= q1 && pwmNum <= q4) {
        if (percent > 100) percent = 100;
        
        uint8_t value = map(percent, 0, 100, 0, 255);
        analogWrite(pwmNum, value);
        
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic: PWM ");
        Serial.print(pwmNum - 3); // Mostrar como q1-q4
        Serial.print(" configurado a ");
        Serial.print(percent);
        Serial.println("%");
        #endif
    } else {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("BioLogic ERROR: Número de PWM no válido. Usa 4-7 (q1-q4).");
        #endif
    }
}

float BioLogic::readVoltage(uint8_t inputNum) {
    if (inputNum >= in1 && inputNum <= in8) {
        uint16_t adcValue = analogRead(inputNum);
        float voltage = (adcValue * 3.3) / 4095.0;
        
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic: Voltaje en entrada ");
        Serial.print(inputNum - 7); // Mostrar como in1-in8
        Serial.print(": ");
        Serial.print(voltage, 2);
        Serial.println("V");
        #endif
        
        return voltage;
    }
    
    #ifdef BIOLOGIC_DEBUG
    Serial.println("BioLogic ERROR: Número de entrada no válido. Usa 8-15 (in1-in8).");
    #endif
    
    return 0.0;
}

// ============================================
// UTILIDADES Y DIAGNÓSTICO
// ============================================

bool BioLogic::isConnected() {
    if (!_initialized) return false;
    
    Wire.beginTransmission(_address);
    uint8_t error = Wire.endTransmission();
    
    bool connected = (error == 0);
    
    #ifdef BIOLOGIC_DEBUG
    if (connected) {
        Serial.println("BioLogic: Placa conectada correctamente");
    } else {
        Serial.print("BioLogic ERROR: No se puede conectar con la placa (error ");
        Serial.print(error);
        Serial.println(")");
    }
    #endif
    
    return connected;
}

void BioLogic::setAddress(uint8_t newAddress) {
    _address = newAddress;
    
    #ifdef BIOLOGIC_DEBUG
    Serial.print("BioLogic: Dirección cambiada a 0x");
    Serial.println(_address, HEX);
    #endif
}

uint8_t BioLogic::getAddress() {
    return _address;
}

String BioLogic::getVersion() {
    return String(BIOLOGIC_VERSION);
}

String BioLogic::getAuthor() {
    return String(BIOLOGIC_AUTHOR);
}

void BioLogic::setTimeout(uint32_t timeout) {
    _timeout = timeout;
    
    #ifdef BIOLOGIC_DEBUG
    Serial.print("BioLogic: Timeout configurado a ");
    Serial.print(timeout);
    Serial.println("ms");
    #endif
}

uint32_t BioLogic::getTimeout() {
    return _timeout;
}

bool BioLogic::testConnection() {
    if (!isConnected()) {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("BioLogic TEST: FALLO - Placa no responde");
        #endif
        return false;
    }
    
    // Test básico de escritura/lectura
    uint8_t testPin = in1;
    uint8_t originalMode = 0; // No podemos leer el modo actual
    
    // Configurar como entrada con pull-up
    pinMode(testPin, INPUT_PULLUP);
    delay(10);
    
    // Leer estado
    uint8_t state = digitalRead(testPin);
    
    #ifdef BIOLOGIC_DEBUG
    Serial.println("BioLogic TEST: Completado");
    Serial.print("  Estado pin de prueba: ");
    Serial.println(state ? "HIGH" : "LOW");
    #endif
    
    return true;
}

// ============================================
// FUNCIONES DE CONFIGURACIÓN AVANZADA
// ============================================

void BioLogic::setI2CFrequency(uint32_t frequency) {
    if (!_initialized) {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("BioLogic ERROR: Librería no inicializada");
        #endif
        return;
    }
    
    Wire.setClock(frequency);
    
    #ifdef BIOLOGIC_DEBUG
    Serial.print("BioLogic: Frecuencia I2C configurada a ");
    Serial.print(frequency);
    Serial.println(" Hz");
    #endif
}

void BioLogic::resetBoard() {
    // Comando especial para reset (si está implementado en el firmware)
    _sendCommand(0xFF, 0xFF, 0xFF);
    
    #ifdef BIOLOGIC_DEBUG
    Serial.println("BioLogic: Comando de reset enviado a la placa");
    #endif
    
    delay(1000); // Esperar a que la placa se reinicie
}