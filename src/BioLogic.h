/*
 * BioLogic.h - Librería para controlar la placa BioLogic
 * Diseñada por @teoriademau para programar la placa BioLogic
 * Versión 2.0.0
 * 
 */

#ifndef BIOLOGIC_H
#define BIOLOGIC_H

#include "Arduino.h"
#include "Wire.h"

// ============================================
// VERSIÓN Y CRÉDITOS
// ============================================
#define BIOLOGIC_VERSION "2.0.0"
#define BIOLOGIC_AUTHOR "@teoriademau"
#define BIOLOGIC_DESCRIPTION "Librería para la placa BioLogic"

// ============================================
// DIRECCIÓN I2C POR DEFECTO DE LA PLACA BIOLOGIC
// ============================================
#define BIOLOGIC_DEFAULT_ADDRESS 0x40

// ============================================
// CONSTANTES DE COMANDOS I2C
// ============================================
#define CMD_PIN_MODE      0x01  // Configurar modo del pin
#define CMD_DIGITAL_WRITE 0x02  // Escribir valor digital
#define CMD_ANALOG_WRITE  0x03  // Escribir valor PWM
#define CMD_DIGITAL_READ  0x04  // Leer entrada digital
#define CMD_ANALOG_READ   0x05  // Leer entrada analógica
#define CMD_PING          0x06  // Comando de latido/keep-alive (NUEVO)

// ============================================
// PINES VIRTUALES DE LA PLACA BIOLOGIC
// ============================================

// Salidas Digitales (Relés/Contactores)
#define r1  0   // PB12 en BioLogic (Pin 22)
#define r2  1   // PB13 en BioLogic (Pin 21)
#define r3  2   // PB14 en BioLogic (Pin 20)
#define r4  3   // PB15 en BioLogic (Pin 19)

// Salidas PWM (Control de potencia)
#define q1  4   // PA8 en BioLogic (Pin 27) - TIM1_CH1
#define q2  5   // PA9 en BioLogic (Pin 26) - TIM1_CH2
#define q3  6   // PA10 en BioLogic (Pin 25) - TIM1_CH3
#define q4  7   // PB11 en BioLogic (Pin 13) - TIM2_CH4

// Entradas Digitales/Analógicas (Sensores/Interruptores)
#define in1  8   // PA0 en BioLogic (Pin 2) - ADC1
#define in2  9   // PA1 en BioLogic (Pin 3) - ADC2
#define in3  10  // PA2 en BioLogic (Pin 4) - ADC3
#define in4  11  // PA3 en BioLogic (Pin 5) - ADC4
#define in5  12  // PA4 en BioLogic (Pin 6) - ADC5
#define in6  13  // PA5 en BioLogic (Pin 7) - ADC6
#define in7  14  // PA6 en BioLogic (Pin 8) - ADC7
#define in8  15  // PA7 en BioLogic (Pin 9) - ADC8

// ============================================
// CONSTANTES ARDUINO COMPATIBLES
// ============================================
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

// Modos adicionales específicos de BioLogic
#define INPUT_ANALOG   0x03  // Para lecturas ADC
#define PWM_MODE       0x04  // Para salidas PWM

// ============================================
// CLASE PRINCIPAL BIOLOGIC
// ============================================
class BioLogic {
private:
    uint8_t _address;            // Dirección I2C de la placa BioLogic
    uint8_t _sdaPin;             // Pin SDA
    uint8_t _sclPin;             // Pin SCL
    bool _initialized;           // Bandera de inicialización
    uint32_t _timeout;           // Timeout para comunicación
    uint8_t rst;                 // Pin Reset STM32 
    
    // Variables del sistema de latido
    uint32_t _lastSuccessfulPing; // Tiempo del último ping exitoso
    bool _autoPingEnabled;        // Auto-ping habilitado
    uint32_t _pingInterval;       // Intervalo entre pings
    uint32_t _lastAutoPing;       // Último auto-ping enviado
    
    // Métodos privados de comunicación
    void _sendCommand(uint8_t cmd, uint8_t pin, uint8_t value = 0);
    uint8_t _readResponse(uint8_t bytes = 1);
    uint16_t _readResponse16();
    
public:
    // ============================================
    // CONSTRUCTORES
    // ============================================
    
    // Constructor por defecto (dirección 0x40)
    BioLogic();
    
    // Constructor con dirección personalizada
    BioLogic(uint8_t address);
    
    // Constructor completo
    BioLogic(uint8_t address, uint8_t sdaPin, uint8_t sclPin);
    
    // ============================================
    // MÉTODOS DE INICIALIZACIÓN
    // ============================================
    
    // Inicializar con pines por defecto (21, 22 para ESP32)
    void begin();
    
    // Inicializar con pines personalizados
    void begin(uint8_t sdaPin, uint8_t sclPin);
    
    // ============================================
    // SISTEMA DE LATIDO (HEARTBEAT) - NUEVO
    // ============================================
    
    // Enviar comando de latido (ping)
    bool ping();
    
    // Enviar ping con múltiples reintentos
    bool ping(uint8_t retries);
    
    // Mantener conexión automáticamente (debe llamarse periódicamente)
    bool maintainConnection();
    
    // Obtener tiempo del último ping exitoso
    uint32_t getLastPingTime();
    
    // Habilitar/deshabilitar auto-ping automático
    void setAutoPing(bool enable);
    
    // Establecer intervalo entre pings automáticos
    void setPingInterval(uint32_t intervalMs);
    
    // ============================================
    // FUNCIONES ARDUINO COMPATIBLES
    // ============================================
    
    // Configurar modo del pin (igual que Arduino)
    void pinMode(uint8_t pin, uint8_t mode);
    
    // Escribir valor digital (igual que Arduino)
    void digitalWrite(uint8_t pin, uint8_t value);
    
    // Escribir valor PWM (0-255, igual que Arduino)
    void analogWrite(uint8_t pin, uint8_t value);
    
    // Leer entrada digital (igual que Arduino)
    uint8_t digitalRead(uint8_t pin);
    
    // Leer entrada analógica (0-4095, igual que Arduino para STM32)
    uint16_t analogRead(uint8_t pin);
    
    // ============================================
    // FUNCIONES ESPECÍFICAS DE BIOLOGIC
    // ============================================
    
    // Control de relés
    void relayOn(uint8_t relayNum);      // Encender relé (r1-r4)
    void relayOff(uint8_t relayNum);     // Apagar relé
    void relayToggle(uint8_t relayNum);  // Alternar estado del relé
    void relayTimed(uint8_t relayNum, uint32_t durationMs); // Relé temporizado con mantenimiento
    
    // Control PWM por porcentaje (0-100%)
    void pwmPercent(uint8_t pwmNum, uint8_t percent);
    
    // Leer voltaje en entrada (0-3.3V)
    float readVoltage(uint8_t inputNum);
    
    // ============================================
    // UTILIDADES Y DIAGNÓSTICO
    // ============================================
    
    // Verificar conexión con la placa BioLogic
    bool isConnected();
    
    // Verificar conexión con reintentos
    bool isConnected(uint8_t retries);
    
    // Cambiar dirección I2C (si la placa BioLogic lo soporta)
    void setAddress(uint8_t newAddress);
    
    // Obtener dirección actual
    uint8_t getAddress();
    
    // Obtener versión de la librería
    String getVersion();
    
    // Obtener información del autor
    String getAuthor();
    
    // Configurar timeout de comunicación (ms)
    void setTimeout(uint32_t timeout);
    
    // Obtener timeout actual
    uint32_t getTimeout();
    
    // Test de comunicación básico
    bool testConnection();
    
    // Ejecutar diagnóstico completo del sistema
    void diagnose();
    
    // ============================================
    // FUNCIONES DE CONFIGURACIÓN AVANZADA
    // ============================================
    
    // Configurar frecuencia I2C (100kHz, 400kHz, etc.)
    void setI2CFrequency(uint32_t frequency);
    
    // Reset remoto de la placa BioLogic (si implementado)
    void resetBoard();
    
    // ============================================
    // FUNCIONES DE MANTENIMIENTO AUTOMÁTICO
    // ============================================
    
    // Realizar mantenimiento automático periódico
    void autoMaintenance();
};

#endif // BIOLOGIC_H