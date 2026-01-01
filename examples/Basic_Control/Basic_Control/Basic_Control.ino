/*
 * Basic_Control.ino - Ejemplo básico de control BioLogic
 * Diseñado por @teoriademau para programar la placa BioLogic
 * 
 * Este ejemplo muestra el control básico de todas las funciones
 * de la placa BioLogic: relés, PWM y lectura de entradas.
 */

#include <BioLogic.h>
#include <Arduino.h>

// Crear objeto de la placa BioLogic
BioLogic board;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("==========================================");
  Serial.println("BIO_LOGIC - CONTROL BÁSICO");
  Serial.println("Diseñado por @teoriademau");
  Serial.println("Placa BioLogic v1.0");
  Serial.println("==========================================");
  Serial.println();
  
  // Inicializar comunicación I2C con la placa BioLogic
  // Para ESP32-C3: GPIO8=SDA, GPIO9=SCL
  board.begin(10, 3);
  Wire.setClock(400000);
  
  delay(1000); // Esperar inicialización de la placa BioLogic
  
  // Verificar conexión con la placa BioLogic
  Serial.print("Conectando con la placa BioLogic... ");
  if (board.isConnected()) {
    Serial.println("OK!");
    Serial.print("  Dirección I2C: 0x");
    Serial.println(board.getAddress(), HEX);
    Serial.print("  Versión librería: ");
    Serial.println(board.getVersion());
    Serial.print("  Autor: ");
    Serial.println(board.getAuthor());
  } else {
    Serial.println("FALLO!");
    Serial.println("Verifica las conexiones:");
    while(1); // Detener si no hay conexión
  }
  
  Serial.println();
  Serial.println("CONFIGURANDO PINES DE LA PLACA BIOLOGIC:");
  Serial.println("----------------------------------------");
  
  // Configurar relés (salidas digitales)
  Serial.println("1. Configurando relés (r1-r4):");
  board.pinMode(r1, OUTPUT);
  board.pinMode(r2, OUTPUT);
  board.pinMode(r3, OUTPUT);
  board.pinMode(r4, OUTPUT);
  Serial.println("   ✓ r1-r4 configurados como OUTPUT");
  
  // Configurar PWM
  Serial.println("2. Configurando salidas PWM (q1-q4):");
  board.pinMode(q1, PWM_MODE);
  board.pinMode(q2, PWM_MODE);
  board.pinMode(q3, PWM_MODE);
  board.pinMode(q4, PWM_MODE);
  Serial.println("   ✓ q1-q4 configurados como PWM");
  
  // Configurar entradas
  Serial.println("3. Configurando entradas (in1-in8):");
  board.pinMode(in1, INPUT_PULLUP);
  board.pinMode(in2, INPUT_PULLUP);
  board.pinMode(in3, INPUT_PULLUP);
  board.pinMode(in4, INPUT_PULLUP);
  board.pinMode(in5, INPUT_PULLUP);
  board.pinMode(in6, INPUT_PULLUP);
  board.pinMode(in7, INPUT_PULLUP);
  board.pinMode(in8, INPUT_PULLUP);
  Serial.println("   ✓ in1-in8 configurados como INPUT_PULLUP");
  
  Serial.println();
  Serial.println("¡CONFIGURACIÓN COMPLETADA!");
  Serial.println("Iniciando demostración en 3 segundos...");
  Serial.println();
  delay(3000);
}

void loop() {
  // ============================================
  // SECCIÓN 1: CONTROL DE RELÉS (r1-r4)
  // ============================================
  Serial.println("=== SECCIÓN 1: CONTROL DE RELÉS ===");
  
  // Encender relés uno por uno
  Serial.println("Encendiendo relés uno por uno:");
  for (int i = 0; i < 4; i++) {
    board.relayOn(i); // Usar r1, r2, r3, r4
    Serial.print("  Relé ");
    Serial.print(i + 1);
    Serial.println(" encendido");
    delay(1000);
  }
  
  delay(2000);
  
  // Apagar todos los relés
  Serial.println("Apagando todos los relés:");
  for (int i = 0; i < 4; i++) {
    board.relayOff(i);
    Serial.print("  Relé ");
    Serial.print(i + 1);
    Serial.println(" apagado");
    delay(500);
  }
  
  delay(1000);
  
  // ============================================
  // SECCIÓN 2: CONTROL PWM (q1-q4)
  // ============================================
  Serial.println();
  Serial.println("=== SECCIÓN 2: CONTROL PWM ===");
  
  // Fade en q1 (0% a 100%)
  Serial.println("Fade en q1 (0% a 100%):");
  for (int percent = 0; percent <= 100; percent += 10) {
    board.pwmPercent(q1, percent);
    Serial.print("  q1: ");
    Serial.print(percent);
    Serial.println("%");
    delay(200);
  }
  
  delay(1000);
  
  // Fade en q2 (100% a 0%)
  Serial.println("Fade en q2 (100% a 0%):");
  for (int percent = 100; percent >= 0; percent -= 10) {
    board.pwmPercent(q2, percent);
    Serial.print("  q2: ");
    Serial.print(percent);
    Serial.println("%");
    delay(200);
  }
  
  delay(1000);
  
  // Configurar q3 y q4 a valores fijos
  Serial.println("Configurando q3 y q4:");
  board.pwmPercent(q3, 25);  // 25%
  board.pwmPercent(q4, 75);  // 75%
  Serial.println("  q3: 25%");
  Serial.println("  q4: 75%");
  
  delay(2000);
  
  // Apagar todos los PWM
  board.pwmPercent(q1, 0);
  board.pwmPercent(q2, 0);
  board.pwmPercent(q3, 0);
  board.pwmPercent(q4, 0);
  Serial.println("Todos los PWM apagados");
  
  delay(1000);
  
  // ============================================
  // SECCIÓN 3: LECTURA DE ENTRADAS DIGITALES
  // ============================================
  Serial.println();
  Serial.println("=== SECCIÓN 3: ENTRADAS DIGITALES ===");
  
  Serial.println("Leyendo estado de entradas in1-in4:");
  for (int i = 0; i < 4; i++) {
    bool estado = board.digitalRead(in1 + i);
    Serial.print("  in");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(estado ? "HIGH (no presionado)" : "LOW (presionado)");
    delay(300);
  }
  
  // Control condicional: si in1 está presionado, encender r1
  bool boton1 = board.digitalRead(in1);
  if (boton1 == LOW) {
    Serial.println("Botón in1 presionado -> Encendiendo r1");
    board.relayOn(r1);
  } else {
    board.relayOff(r1);
  }
  
  delay(2000);
  board.relayOff(r1);
  
  // ============================================
  // SECCIÓN 4: LECTURA ANALÓGICA
  // ============================================
  Serial.println();
  Serial.println("=== SECCIÓN 4: ENTRADAS ANALÓGICAS ===");
  
  // Cambiar in1 a modo analógico
  Serial.println("Configurando in1 como entrada analógica:");
  board.pinMode(in1, INPUT_ANALOG);
  delay(100);
  
  // Leer valor analógico
  uint16_t valorADC = board.analogRead(in1);
  float voltaje = board.readVoltage(in1);
  
  Serial.print("  Valor ADC: ");
  Serial.println(valorADC);
  Serial.print("  Voltaje: ");
  Serial.print(voltaje, 2);
  Serial.println("V");
  
  // Controlar q1 según lectura analógica
  uint8_t brillo = map(valorADC, 0, 4095, 0, 100);
  board.pwmPercent(q1, brillo);
  Serial.print("  q1 ajustado a: ");
  Serial.print(brillo);
  Serial.println("% según lectura analógica");
  
  delay(2000);
  
  // Volver a modo digital
  board.pinMode(in1, INPUT_PULLUP);
  board.pwmPercent(q1, 0);
  
  // ============================================
  // SECCIÓN 5: SISTEMA COMPLETO
  // ============================================
  Serial.println();
  Serial.println("=== SECCIÓN 5: SISTEMA COMPLETO ===");
  
  Serial.println("Demostrando sistema integrado:");
  Serial.println("  - Leer todas las entradas");
  Serial.println("  - Controlar salidas según entradas");
  Serial.println("  - Feedback por Serial");
  
  for (int ciclo = 0; ciclo < 5; ciclo++) {
    Serial.print("Ciclo ");
    Serial.print(ciclo + 1);
    Serial.println(":");
    
    // Leer todas las entradas
    for (int i = 0; i < 8; i++) {
      bool estado = board.digitalRead(in1 + i);
      if (i < 4) { // Solo para in1-in4
        // Controlar relé correspondiente
        board.digitalWrite(i, estado == LOW ? HIGH : LOW);
      }
    }
    
    // Control PWM cíclico
    int pwmVal = (ciclo * 25) % 100;
    board.pwmPercent(q1, pwmVal);
    board.pwmPercent(q2, 100 - pwmVal);
    
    Serial.print("  PWM: q1=");
    Serial.print(pwmVal);
    Serial.print("%, q2=");
    Serial.print(100 - pwmVal);
    Serial.println("%");
    
    delay(1000);
  }
  
  // Limpiar todo al final
  for (int i = 0; i < 4; i++) {
    board.relayOff(i);
  }
  for (int i = 0; i < 4; i++) {
    board.pwmPercent(q1 + i, 0);
  }
  
  Serial.println();
  Serial.println("=== FIN DE DEMOSTRACIÓN ===");
  Serial.println("Reiniciando en 5 segundos...");
  Serial.println("==========================================");
  Serial.println();
  
  delay(5000);
}