/*
 * PWM_Fade.ino - Efectos PWM para la placa BioLogic
 * Diseñado por @teoriademau para programar la placa BioLogic
 * 
 * Este ejemplo muestra efectos avanzados de PWM en los 4 canales
 * de la placa BioLogic (q1-q4).
 */

#include <BioLogic.h>

// Crear objeto de la placa BioLogic
BioLogic board;

// Variables para efectos PWM
float fase1 = 0.0;
float fase2 = 0.0;
float fase3 = 0.0;
float fase4 = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("==========================================");
  Serial.println("BIO_LOGIC - EFECTOS PWM");
  Serial.println("Diseñado por @teoriademau");
  Serial.println("Placa BioLogic v1.0");
  Serial.println("==========================================");
  Serial.println();
  
  // Inicializar comunicación I2C
  board.begin(8, 9);
  delay(1000);
  
  // Verificar conexión
  if (!board.isConnected()) {
    Serial.println("ERROR: No se pudo conectar con la placa BioLogic");
    while(1);
  }
  
  // Configurar todos los canales PWM
  board.pinMode(q1, PWM_MODE);
  board.pinMode(q2, PWM_MODE);
  board.pinMode(q3, PWM_MODE);
  board.pinMode(q4, PWM_MODE);
  
  Serial.println("EFECTOS CONFIGURADOS:");
  Serial.println("  q1: Onda sinusoidal (lenta)");
  Serial.println("  q2: Onda sinusoidal (rápida)");
  Serial.println("  q3: Onda triangular");
  Serial.println("  q4: Onda cuadrada");
  Serial.println();
  Serial.println("Iniciando efectos en 2 segundos...");
  delay(2000);
}

void loop() {
  // ============================================
  // EFECTO 1: Onda sinusoidal lenta (q1)
  // ============================================
  fase1 += 0.02;
  if (fase1 > TWO_PI) fase1 = 0;
  
  // Seno de 0 a 100%
  int pwm1 = 50 + (sin(fase1) * 50);
  board.pwmPercent(q1, pwm1);
  
  // ============================================
  // EFECTO 2: Onda sinusoidal rápida (q2)
  // ============================================
  fase2 += 0.05;
  if (fase2 > TWO_PI) fase2 = 0;
  
  // Seno desplazado 90°
  int pwm2 = 50 + (sin(fase2 + HALF_PI) * 50);
  board.pwmPercent(q2, pwm2);
  
  // ============================================
  // EFECTO 3: Onda triangular (q3)
  // ============================================
  fase3 += 0.03;
  if (fase3 > TWO_PI) fase3 = 0;
  
  // Onda triangular
  float triValue;
  if (fase3 < PI) {
    triValue = fase3 / PI; // 0 a 1
  } else {
    triValue = 2 - (fase3 / PI); // 1 a 0
  }
  int pwm3 = triValue * 100;
  board.pwmPercent(q3, pwm3);
  
  // ============================================
  // EFECTO 4: Onda cuadrada (q4)
  // ============================================
  fase4 += 0.01;
  if (fase4 > TWO_PI) fase4 = 0;
  
  // Onda cuadrada (25% ciclo de trabajo)
  int pwm4 = (sin(fase4) > 0) ? 100 : 0;
  board.pwmPercent(q4, pwm4);
  
  // ============================================
  // VISUALIZACIÓN POR SERIAL
  // ============================================
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    Serial.print("q1: ");
    Serial.print(pwm1);
    Serial.print("% | q2: ");
    Serial.print(pwm2);
    Serial.print("% | q3: ");
    Serial.print(pwm3);
    Serial.print("% | q4: ");
    Serial.print(pwm4);
    Serial.println("%");
    
    // Visualización ASCII
    Serial.print("[");
    for (int i = 0; i < pwm1 / 5; i++) Serial.print("#");
    for (int i = pwm1 / 5; i < 20; i++) Serial.print(" ");
    Serial.print("] [");
    
    for (int i = 0; i < pwm2 / 5; i++) Serial.print("#");
    for (int i = pwm2 / 5; i < 20; i++) Serial.print(" ");
    Serial.print("] [");
    
    for (int i = 0; i < pwm3 / 5; i++) Serial.print("#");
    for (int i = pwm3 / 5; i < 20; i++) Serial.print(" ");
    Serial.print("] [");
    
    for (int i = 0; i < pwm4 / 5; i++) Serial.print("#");
    for (int i = pwm4 / 5; i < 20; i++) Serial.print(" ");
    Serial.println("]");
    
    lastPrint = millis();
  }
  
  // Pequeño delay para control de velocidad
  delay(20);
}

// Función para efecto de respiración (fade in/out suave)
void efectoRespiracion() {
  Serial.println();
  Serial.println("=== EFECTO RESPIRACIÓN ===");
  
  for (int rep = 0; rep < 3; rep++) {
    Serial.print("Respiración ");
    Serial.print(rep + 1);
    Serial.println(":");
    
    // Fade in suave (inhalar)
    for (int i = 0; i <= 100; i++) {
      board.pwmPercent(q1, i);
      board.pwmPercent(q2, i);
      board.pwmPercent(q3, i);
      board.pwmPercent(q4, i);
      delay(30);
    }
    
    delay(500); // Mantener al máximo
    
    // Fade out suave (exhalar)
    for (int i = 100; i >= 0; i--) {
      board.pwmPercent(q1, i);
      board.pwmPercent(q2, i);
      board.pwmPercent(q3, i);
      board.pwmPercent(q4, i);
      delay(30);
    }
    
    delay(1000); // Pausa entre respiraciones
  }
}
