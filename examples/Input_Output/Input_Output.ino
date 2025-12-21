/*
 * Input_Output.ino - Sistema completo de entradas y salidas
 * Diseñado por @teoriademau para programar la placa BioLogic
 * 
 * Este ejemplo crea un sistema interactivo donde las entradas
 * controlan las salidas de la placa BioLogic.
 */

#include <BioLogic.h>

// Crear objeto de la placa BioLogic
BioLogic board;

// Variables para el estado del sistema
bool sistemaActivo = true;
unsigned long ultimaActualizacion = 0;
const unsigned long INTERVALO_ACTUALIZACION = 100; // ms

// Estados de las entradas
bool estadosEntradas[8] = {false};
uint16_t valoresAnalogicos[8] = {0};

// Estados de las salidas
bool estadosRelays[4] = {false};
uint8_t valoresPWM[4] = {0};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("==========================================");
  Serial.println("BIO_LOGIC - SISTEMA ENTRADAS/SALIDAS");
  Serial.println("Diseñado por @teoriademau");
  Serial.println("Placa BioLogic v1.0");
  Serial.println("==========================================");
  Serial.println();
  
  // Inicializar comunicación I2C 
  // Direccion: 0x40
  board.begin(8, 9);
  delay(1000);
  
  // Verificar conexión
  if (!board.isConnected()) {
    Serial.println("ERROR: No se pudo conectar con la placa BioLogic");
    Serial.println("Sistema desactivado");
    sistemaActivo = false;
    return;
  }
  
  Serial.println("✓ Placa BioLogic conectada correctamente");
  
  // Configurar todos los pines de la placa BioLogic
  configurarPines();
  
  Serial.println();
  Serial.println("SISTEMA CONFIGURADO:");
  Serial.println("-------------------");
  Serial.println("ENTRADAS (in1-in8):");
  Serial.println("  in1-in4: Botones/interruptores (INPUT_PULLUP)");
  Serial.println("  in5-in8: Sensores analógicos (INPUT_ANALOG)");
  Serial.println();
  Serial.println("SALIDAS (r1-r4, q1-q4):");
  Serial.println("  r1-r4: Relés/salidas digitales");
  Serial.println("  q1-q4: Salidas PWM (0-100%)");
  Serial.println();
  Serial.println("LÓGICA DEL SISTEMA:");
  Serial.println("  - Botones in1-in4 controlan relés r1-r4");
  Serial.println("  - Sensores in5-in8 controlan PWM q1-q4");
  Serial.println("  - Monitorización continua por Serial");
  Serial.println();
  Serial.println("Iniciando sistema en 3 segundos...");
  delay(3000);
}

void loop() {
  if (!sistemaActivo) {
    Serial.println("Sistema desactivado. Verifica la conexión con la placa BioLogic.");
    delay(5000);
    return;
  }
  
  // Actualizar a intervalo fijo
  unsigned long ahora = millis();
  if (ahora - ultimaActualizacion >= INTERVALO_ACTUALIZACION) {
    ultimaActualizacion = ahora;
    
    // 1. LEER TODAS LAS ENTRADAS
    leerEntradas();
    
    // 2. PROCESAR LÓGICA DEL SISTEMA
    procesarLogica();
    
    // 3. ACTUALIZAR SALIDAS
    actualizarSalidas();
    
    // 4. MOSTRAR ESTADO
    mostrarEstado();
  }
  
  // Verificar conexión periódicamente
  static unsigned long ultimaVerificacion = 0;
  if (ahora - ultimaVerificacion >= 5000) {
    ultimaVerificacion = ahora;
    if (!board.isConnected()) {
      Serial.println("ALERTA: Pérdida de conexión con la placa BioLogic");
      sistemaActivo = false;
    }
  }
}

// ============================================
// FUNCIONES DEL SISTEMA
// ============================================

void configurarPines() {
  Serial.print("Configurando pines de la placa BioLogic... ");
  
  // Configurar entradas
  for (int i = 0; i < 4; i++) {
    board.pinMode(in1 + i, INPUT_PULLUP); // Botones digitales
  }
  for (int i = 4; i < 8; i++) {
    board.pinMode(in1 + i, INPUT_ANALOG); // Sensores analógicos
  }
  
  // Configurar salidas
  for (int i = 0; i < 4; i++) {
    board.pinMode(r1 + i, OUTPUT);   // Relés
    board.pinMode(q1 + i, PWM_MODE); // PWM
  }
  
  Serial.println("OK");
}

void leerEntradas() {
  // Leer entradas digitales (in1-in4)
  for (int i = 0; i < 4; i++) {
    estadosEntradas[i] = (board.digitalRead(in1 + i) == LOW);
  }
  
  // Leer entradas analógicas (in5-in8)
  for (int i = 4; i < 8; i++) {
    valoresAnalogicos[i] = board.analogRead(in1 + i);
  }
}

void procesarLogica() {
  // Lógica 1: Botones controlan relés directamente
  for (int i = 0; i < 4; i++) {
    estadosRelays[i] = estadosEntradas[i];
  }
  
  // Lógica 2: Sensores analógicos controlan PWM
  for (int i = 0; i < 4; i++) {
    // Convertir ADC (0-4095) a porcentaje PWM (0-100)
    valoresPWM[i] = map(valoresAnalogicos[i + 4], 0, 4095, 0, 100);
    
    // Lógica especial para q1: Invertir si botón 1 está presionado
    if (i == 0 && estadosEntradas[0]) {
      valoresPWM[i] = 100 - valoresPWM[i];
    }
  }
  
  // Lógica 3: Combinaciones especiales
  // Si in1 e in2 están presionados, apagar todo
  if (estadosEntradas[0] && estadosEntradas[1]) {
    for (int i = 0; i < 4; i++) {
      estadosRelays[i] = false;
      valoresPWM[i] = 0;
    }
  }
  
  // Si in3 e in4 están presionados, encender todo
  if (estadosEntradas[2] && estadosEntradas[3]) {
    for (int i = 0; i < 4; i++) {
      estadosRelays[i] = true;
      valoresPWM[i] = 100;
    }
  }
}

void actualizarSalidas() {
  // Actualizar relés
  for (int i = 0; i < 4; i++) {
    if (estadosRelays[i]) {
      board.relayOn(r1 + i);
    } else {
      board.relayOff(r1 + i);
    }
  }
  
  // Actualizar PWM
  for (int i = 0; i < 4; i++) {
    board.pwmPercent(q1 + i, valoresPWM[i]);
  }
}

void mostrarEstado() {
  static int contador = 0;
  contador++;
  
  // Mostrar cada 10 actualizaciones (cada segundo aprox)
  if (contador % 10 == 0) {
    Serial.println();
    Serial.println("=== ESTADO DEL SISTEMA ===");
    Serial.println("ENTRADAS:");
    
    // Mostrar botones
    Serial.print("  Botones: ");
    for (int i = 0; i < 4; i++) {
      Serial.print("in");
      Serial.print(i + 1);
      Serial.print(":");
      Serial.print(estadosEntradas[i] ? "ON " : "OFF ");
    }
    Serial.println();
    
    // Mostrar sensores
    Serial.print("  Sensores: ");
    for (int i = 4; i < 8; i++) {
      float voltaje = (valoresAnalogicos[i] * 3.3) / 4095.0;
      Serial.print("in");
      Serial.print(i + 1);
      Serial.print(":");
      Serial.print(voltaje, 1);
      Serial.print("V ");
    }
    Serial.println();
    
    Serial.println("SALIDAS:");
    
    // Mostrar relés
    Serial.print("  Relés: ");
    for (int i = 0; i < 4; i++) {
      Serial.print("r");
      Serial.print(i + 1);
      Serial.print(":");
      Serial.print(estadosRelays[i] ? "ON " : "OFF ");
    }
    Serial.println();
    
    // Mostrar PWM
    Serial.print("  PWM: ");
    for (int i = 0; i < 4; i++) {
      Serial.print("q");
      Serial.print(i + 1);
      Serial.print(":");
      Serial.print(valoresPWM[i]);
      Serial.print("% ");
    }
    Serial.println();
    
    Serial.println("===========================");
  }
}

// Función para mostrar ayuda
void mostrarAyuda() {
  Serial.println();
  Serial.println("AYUDA DEL SISTEMA:");
  Serial.println("-----------------");
  Serial.println("Conexiones físicas recomendadas:");
  Serial.println("  in1-in4: Botones a GND (activación por LOW)");
  Serial.println("  in5-in8: Potenciómetros (central a entrada, extremos a 3.3V y GND)");
  Serial.println("  r1-r4:  LEDs con resistencia 220Ω a GND");
  Serial.println("  q1-q4:  LEDs con resistencia 220Ω a GND");
  Serial.println();
  Serial.println("Combinaciones especiales:");
  Serial.println("  in1 + in2: Apagar todo");
  Serial.println("  in3 + in4: Encender todo");
  Serial.println("  in1: Invertir q1");
  Serial.println();
}