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
// VARIABLES DE LATIDO (Nuevas - agregar al inicio del archivo)
// ============================================
static uint32_t _lastSuccessfulPing = 0;
static bool _autoPingEnabled = true;
static uint32_t _pingInterval = 1000; // 1 segundo por defecto
static uint32_t _lastAutoPing = 0;

// ============================================
// CONSTRUCTORES
// ============================================

BioLogic::BioLogic() {
    _address = BIOLOGIC_DEFAULT_ADDRESS;
    _sdaPin = 10;        // Pin SDA por defecto para ESP32
    _sclPin = 3;         // Pin SCL por defecto para ESP32
    rst = 0;     // Pin por defecto para hacer el Reset por software del STM32
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

// ============================================
// MÉTODOS DE INICIALIZACIÓN
// ============================================

void BioLogic::begin() {
    // Inicializar comunicación I2C
    Wire.begin(_sdaPin, _sclPin);
    Wire.setClock(400000); // Fast Mode 400kHz
    pinMode(rst,OUTPUT);
    digitalWrite(rst, LOW);
    delay(100);
    digitalWrite(rst,HIGH);
    delay(100);
    
    _initialized = true;
    
    // Pequeña pausa para inicialización de la placa BioLogic
    delay(100);
    
    // Realizar primer ping para verificar conexión
    if (ping()) {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("✓ BioLogic: Librería inicializada y conexión verificada");
        Serial.print("  Dirección: 0x");
        Serial.println(_address, HEX);
        Serial.print("  SDA: GPIO");
        Serial.print(_sdaPin);
        Serial.print(", SCL: GPIO");
        Serial.println(_sclPin);
        #endif
    } else {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("⚠ BioLogic: Librería inicializada, pero no se pudo verificar conexión");
        #endif
    }
}

void BioLogic::begin(uint8_t sdaPin, uint8_t sclPin) {
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    
    // Inicializar comunicación I2C con pines personalizados
    Wire.begin(_sdaPin, _sclPin);
    Wire.setClock(400000);
    pinMode(rst,OUTPUT);
    digitalWrite(rst, LOW);
    delay(100);
    digitalWrite(rst,HIGH);
    delay(100);
    
    _initialized = true;
    delay(100);
    
    // Realizar primer ping
    if (ping()) {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("✓ BioLogic: Librería inicializada con pines personalizados");
        Serial.print("  SDA: GPIO");
        Serial.print(_sdaPin);
        Serial.print(", SCL: GPIO");
        Serial.println(_sclPin);
        #endif
    } else {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("⚠ BioLogic: Inicializada con pines personalizados, conexión no verificada");
        #endif
    }
}

// ============================================
// SISTEMA DE LATIDO (HEARTBEAT) - NUEVO
// ============================================

bool BioLogic::ping() {
    return ping(1); // Un solo intento por defecto
}

bool BioLogic::ping(uint8_t retries) {
    if (!_initialized) {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("BioLogic ERROR: Librería no inicializada. Llama a begin() primero.");
        #endif
        return false;
    }
    
    for (uint8_t attempt = 0; attempt < retries; attempt++) {
        if (attempt > 0) {
            #ifdef BIOLOGIC_DEBUG
            Serial.print("BioLogic: Reintento ping ");
            Serial.print(attempt + 1);
            Serial.print("/");
            Serial.println(retries);
            #endif
            delay(50 * attempt); // Backoff exponencial
        }
        
        Wire.beginTransmission(_address);
        Wire.write(CMD_PING);      // Comando de ping
        uint8_t error = Wire.endTransmission();
        
        if (error != 0) {
            #ifdef BIOLOGIC_DEBUG
            Serial.print("BioLogic: Error en transmisión ping (");
            Serial.print(error);
            Serial.println(")");
            #endif
            continue;
        }
        
        delayMicroseconds(500); // Pequeña pausa para procesamiento
        
        Wire.requestFrom(_address, (uint8_t)2);
        uint32_t startTime = millis();
        
        // Esperar respuesta con timeout
        while (Wire.available() < 2 && (millis() - startTime) < _timeout) {
            delayMicroseconds(100);
        }
        
        if (Wire.available() >= 2) {
            uint8_t byte1 = Wire.read();
            uint8_t byte2 = Wire.read();
            
            // Verificar respuesta esperada (0xAA 0x55)
            if (byte1 == 0xAA && byte2 == 0x55) {
                _lastSuccessfulPing = millis();
                #ifdef BIOLOGIC_DEBUG
                Serial.println("✓ BioLogic: Ping exitoso (respuesta 0xAA 0x55)");
                #endif
                return true;
            } else {
                #ifdef BIOLOGIC_DEBUG
                Serial.print("BioLogic: Respuesta inesperada: 0x");
                Serial.print(byte1, HEX);
                Serial.print(" 0x");
                Serial.println(byte2, HEX);
                #endif
            }
        } else {
            #ifdef BIOLOGIC_DEBUG
            Serial.println("BioLogic: Timeout en ping");
            #endif
        }
    }
    
    #ifdef BIOLOGIC_DEBUG
    Serial.println("✗ BioLogic: Ping falló después de todos los intentos");
    #endif
    return false;
}

bool BioLogic::maintainConnection() {
    if (!_autoPingEnabled) return true;
    
    uint32_t currentTime = millis();
    
    // Verificar si es tiempo de enviar otro ping
    if (currentTime - _lastAutoPing >= _pingInterval) {
        _lastAutoPing = currentTime;
        
        // Si no ha habido comunicación exitosa en 2 intervalos, hacer ping
        if (currentTime - _lastSuccessfulPing >= (_pingInterval * 2)) {
            #ifdef BIOLOGIC_DEBUG
            Serial.println("BioLogic: Manteniendo conexión (auto-ping)");
            #endif
            return ping(2); // 2 intentos para auto-ping
        }
    }
    
    return true;
}

uint32_t BioLogic::getLastPingTime() {
    return _lastSuccessfulPing;
}

void BioLogic::setAutoPing(bool enable) {
    _autoPingEnabled = enable;
    
    #ifdef BIOLOGIC_DEBUG
    Serial.print("BioLogic: Auto-ping ");
    Serial.println(enable ? "habilitado" : "deshabilitado");
    #endif
}

void BioLogic::setPingInterval(uint32_t intervalMs) {
    _pingInterval = intervalMs;
    
    #ifdef BIOLOGIC_DEBUG
    Serial.print("BioLogic: Intervalo de ping configurado a ");
    Serial.print(intervalMs);
    Serial.println("ms");
    #endif
}

// ============================================
// MÉTODOS PRIVADOS DE COMUNICACIÓN (Mejorados)
// ============================================

void BioLogic::_sendCommand(uint8_t cmd, uint8_t pin, uint8_t value) {
    if (!_initialized) {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("BioLogic ERROR: Librería no inicializada. Llama a begin() primero.");
        #endif
        return;
    }
    
    // Actualizar timestamp de última comunicación exitosa
    _lastSuccessfulPing = millis();
    
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
// FUNCIONES ARDUINO COMPATIBLES (Mejoradas)
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
// FUNCIONES ESPECÍFICAS DE BIOLOGIC (Mejoradas)
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

void BioLogic::relayTimed(uint8_t relayNum, uint32_t durationMs) {
    if (relayNum <= r4) {
        relayOn(relayNum);
        
        // Enviar pings periódicos mientras el relé está activo
        uint32_t startTime = millis();
        while (millis() - startTime < durationMs) {
            delay(1000); // Esperar 1 segundo
            
            // Mantener conexión con ping
            if (!maintainConnection()) {
                #ifdef BIOLOGIC_DEBUG
                Serial.println("⚠ BioLogic: Pérdida de conexión durante operación temporizada");
                #endif
            }
        }
        
        relayOff(relayNum);
        
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic: Relé ");
        Serial.print(relayNum + 1);
        Serial.print(" activado por ");
        Serial.print(durationMs / 1000.0, 1);
        Serial.println(" segundos");
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
// UTILIDADES Y DIAGNÓSTICO (Mejoradas)
// ============================================

bool BioLogic::isConnected() {
    if (!_initialized) return false;
    
    // Usar ping en lugar de solo verificar transmisión
    return ping(1); // 1 intento rápido
}

bool BioLogic::isConnected(uint8_t retries) {
    if (!_initialized) return false;
    
    return ping(retries);
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
    if (!isConnected(2)) {
        #ifdef BIOLOGIC_DEBUG
        Serial.println("✗ BioLogic TEST: FALLO - Placa no responde a ping");
        #endif
        return false;
    }
    
    // Test básico de escritura/lectura
    uint8_t testPin = in1;
    
    // Configurar como entrada con pull-up
    pinMode(testPin, INPUT_PULLUP);
    delay(10);
    
    // Leer estado
    uint8_t state = digitalRead(testPin);
    
    // Test de escritura en un pin de salida (si es seguro)
    uint8_t outputPin = r1;
    uint8_t originalValue = digitalRead(outputPin);
    
    digitalWrite(outputPin, HIGH);
    delay(50);
    digitalWrite(outputPin, originalValue);
    
    #ifdef BIOLOGIC_DEBUG
    Serial.println("✓ BioLogic TEST: Completado exitosamente");
    Serial.print("  Estado pin de prueba: ");
    Serial.println(state ? "HIGH" : "LOW");
    Serial.print("  Último ping exitoso: hace ");
    Serial.print((millis() - _lastSuccessfulPing) / 1000.0, 1);
    Serial.println(" segundos");
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
    Wire.beginTransmission(_address);
    Wire.write(0xFF);  // Comando de reset
    Wire.write(0xFF);
    Wire.write(0xFF);
    Wire.endTransmission();
    
    #ifdef BIOLOGIC_DEBUG
    Serial.println("BioLogic: Comando de reset enviado a la placa");
    #endif
    
    delay(1000); // Esperar a que la placa se reinicie
}

// ============================================
// FUNCIÓN DE MANTENIMIENTO AUTOMÁTICO (NUEVA)
// ============================================

void BioLogic::autoMaintenance() {
    static uint32_t lastMaintenance = 0;
    uint32_t currentTime = millis();
    
    // Realizar mantenimiento cada 5 segundos
    if (currentTime - lastMaintenance >= 5000) {
        lastMaintenance = currentTime;
        
        // Verificar conexión
        if (!isConnected()) {
            #ifdef BIOLOGIC_DEBUG
            Serial.println("⚠ BioLogic: Pérdida de conexión detectada, intentando recuperar...");
            #endif
            
            // Intentar reconexión
            for (uint8_t i = 0; i < 3; i++) {
                if (ping(2)) {
                    #ifdef BIOLOGIC_DEBUG
                    Serial.println("✓ BioLogic: Conexión recuperada");
                    #endif
                    break;
                }
                delay(100 * (i + 1));
            }
        }
        
        // Mostrar estadísticas de conexión
        #ifdef BIOLOGIC_DEBUG
        Serial.print("BioLogic: Estado conexión - ");
        Serial.print("Último ping exitoso: hace ");
        Serial.print((currentTime - _lastSuccessfulPing) / 1000.0, 1);
        Serial.println(" segundos");
        #endif
    }
}

// ============================================
// FUNCIONES DE DIAGNÓSTICO EXTENDIDO (NUEVAS)
// ============================================

void BioLogic::diagnose() {
    #ifdef BIOLOGIC_DEBUG
    Serial.println("========================================");
    Serial.println("BIO_LOGIC - DIAGNÓSTICO COMPLETO");
    Serial.println("========================================");
    
    // 1. Verificar conexión I2C
    Serial.print("1. Conexión I2C: ");
    Wire.beginTransmission(_address);
    uint8_t error = Wire.endTransmission();
    
    if (error == 0) {
        Serial.println("OK");
    } else {
        Serial.print("ERROR (código: ");
        Serial.print(error);
        Serial.println(")");
    }
    
    // 2. Test de ping
    Serial.print("2. Test de ping: ");
    if (ping(2)) {
        Serial.println("OK (respuesta 0xAA 0x55)");
    } else {
        Serial.println("FALLO");
    }
    
    // 3. Tiempo desde último ping exitoso
    Serial.print("3. Último ping exitoso: hace ");
    Serial.print((millis() - _lastSuccessfulPing) / 1000.0, 1);
    Serial.println(" segundos");
    
    // 4. Estado auto-ping
    Serial.print("4. Auto-ping: ");
    Serial.println(_autoPingEnabled ? "HABILITADO" : "DESHABILITADO");
    if (_autoPingEnabled) {
        Serial.print("   Intervalo: ");
        Serial.print(_pingInterval);
        Serial.println(" ms");
    }
    
    // 5. Test rápido de lectura
    Serial.print("5. Test lectura entrada 1 (in1): ");
    uint16_t analogValue = analogRead(8); // in1 = pin 8
    Serial.print(analogValue);
    Serial.print(" (");
    Serial.print((analogValue * 3.3) / 4095.0, 2);
    Serial.println("V)");
    
    Serial.println("========================================");
    Serial.println("DIAGNÓSTICO COMPLETADO");
    Serial.println("========================================");
    #endif
}