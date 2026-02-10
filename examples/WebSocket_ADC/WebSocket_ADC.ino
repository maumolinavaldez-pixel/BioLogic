#include <WiFi.h>
#include <WebServer.h>
#include <BioLogic.h>

// Configuración WiFi
const char* ssid = "BioLogic ADC";
const char* password = "12345678";

WebServer server(80);

BioLogic bio;

// Variables del sensor
int sensorValue = 0;

void setup() {
  delay(2000);
  Serial.begin(115200);
  
  // Inicializar BioLogic
  bio.begin();
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  delay(10);
  digitalWrite(0, HIGH);
  delay(100);
  
  // Configurar pin analógico
  bio.pinMode(in8, INPUT_ANALOG);
  
  Serial.println("BioLogic Started");
  
  // Iniciar WiFi
  WiFi.softAP(ssid, password);
  
  Serial.println("");
  Serial.println("WiFi Access Point iniciado");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
  
  // Configurar rutas web
  server.on("/", handleRoot);
  server.on("/data", handleData);
  
  server.begin();
  Serial.println("Servidor web iniciado");
  delay(100);
}

void loop() {
  server.handleClient();
  
  // Leer sensor cada 100ms
  static unsigned long lastRead = 0;
  if (millis() - lastRead >= 100) {
    lastRead = millis();
    sensorValue = bio.analogRead(in8);
    
    // Mostrar en serial
    Serial.print("Valor Analogico: ");
    Serial.println(sensorValue);
  }
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>BioLogic ADC</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 50px;
      background-color: #f0f0f0;
    }
    .container {
      background: white;
      padding: 30px;
      border-radius: 10px;
      box-shadow: 0 0 10px rgba(0,0,0,0.1);
      max-width: 500px;
      margin: 0 auto;
    }
    h1 {
      color: #333;
    }
    .value {
      font-size: 48px;
      font-weight: bold;
      color: #2196F3;
      margin: 20px 0;
    }
    .bar {
      width: 100%;
      height: 30px;
      background: #ddd;
      border-radius: 15px;
      margin: 20px 0;
      overflow: hidden;
    }
    .fill {
      height: 100%;
      background: linear-gradient(90deg, #2196F3, #21CBF3);
      width: 0%;
      transition: width 0.5s;
    }
    .info {
      color: #666;
      margin-top: 20px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>📊 BioLogic ADC</h1>
    <p>Valor actual del sensor:</p>
    <div class="value" id="value">0</div>
    <div class="bar">
      <div class="fill" id="bar"></div>
    </div>
    <p id="percentage">0% (0V)</p>
    <div class="info">
      <p>Rango: 0-1023 (10 bits)</p>
      <p>IP: )rawliteral" + WiFi.softAPIP().toString() + R"rawliteral(</p>
    </div>
  </div>

  <script>
    function updateData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          // Actualizar valor
          document.getElementById('value').textContent = data.value;
          
          // Calcular porcentaje con ADC de 10 bits (0-1023)
          let percent = (data.value / 1023 * 100).toFixed(1);
          let voltage = (data.value / 1023 * 3.3).toFixed(2);
          
          // Actualizar barra
          document.getElementById('bar').style.width = percent + '%';
          
          // Actualizar texto
          document.getElementById('percentage').textContent = 
            percent + '% (' + voltage + 'V)';
        })
        .catch(error => console.error('Error:', error));
    }
    
    // Actualizar cada segundo
    setInterval(updateData, 1000);
    
    // Actualizar inmediatamente
    updateData();
  </script>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html", html);
}

// JSON con datos del sensor
void handleData() {
  String json = "{\"value\":" + String(sensorValue) + "}";
  server.send(200, "application/json", json);
}