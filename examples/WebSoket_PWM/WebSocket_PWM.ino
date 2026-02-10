#include <WiFi.h>
#include <BioLogic.h>

const char* ssid = "BioLogic PWM Control";
const char* password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Variables para PWM
int pwmValue1 = 0;
int pwmValue2 = 0;
int pwmValue3 = 0;
int pwmValue4 = 0;

BioLogic board;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  // Reset de la placa BioLogic
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  delay(10);
  digitalWrite(0, HIGH);
  delay(100);
  
  board.begin();
  
  // Configurar pines en modo PWM según la librería BioLogic
  board.pinMode(q1, PWM_MODE);
  board.pinMode(q2, PWM_MODE);
  board.pinMode(q3, PWM_MODE);
  board.pinMode(q4, PWM_MODE);
  
  // Inicializar PWM en 0%
  board.pwmPercent(q1, 0);
  board.pwmPercent(q2, 0);
  board.pwmPercent(q3, 0);
  board.pwmPercent(q4, 0);
  
  // Configurar WiFi como Access Point
  WiFi.softAP(ssid, password);
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi AP creado.");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  
  server.begin();
  delay(100);
  
  Serial.println("Servidor web iniciado");
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Control de PWM para Q1
            if (header.indexOf("GET /q1pwm/") >= 0) {
              int startIndex = header.indexOf("GET /q1pwm/") + 11;
              int endIndex = header.indexOf(" ", startIndex);
              String valueStr = header.substring(startIndex, endIndex);
              pwmValue1 = valueStr.toInt();
              board.pwmPercent(q1, pwmValue1);
              Serial.print("Q1 PWM establecido a: ");
              Serial.println(pwmValue1);
            }
            // Control de PWM para Q2
            else if (header.indexOf("GET /q2pwm/") >= 0) {
              int startIndex = header.indexOf("GET /q2pwm/") + 11;
              int endIndex = header.indexOf(" ", startIndex);
              String valueStr = header.substring(startIndex, endIndex);
              pwmValue2 = valueStr.toInt();
              board.pwmPercent(q2, pwmValue2);
              Serial.print("Q2 PWM establecido a: ");
              Serial.println(pwmValue2);
            }
            // Control de PWM para Q3
            else if (header.indexOf("GET /q3pwm/") >= 0) {
              int startIndex = header.indexOf("GET /q3pwm/") + 11;
              int endIndex = header.indexOf(" ", startIndex);
              String valueStr = header.substring(startIndex, endIndex);
              pwmValue3 = valueStr.toInt();
              board.pwmPercent(q3, pwmValue3);
              Serial.print("Q3 PWM establecido a: ");
              Serial.println(pwmValue3);
            }
            // Control de PWM para Q4
            else if (header.indexOf("GET /q4pwm/") >= 0) {
              int startIndex = header.indexOf("GET /q4pwm/") + 11;
              int endIndex = header.indexOf(" ", startIndex);
              String valueStr = header.substring(startIndex, endIndex);
              pwmValue4 = valueStr.toInt();
              board.pwmPercent(q4, pwmValue4);
              Serial.print("Q4 PWM establecido a: ");
              Serial.println(pwmValue4);
            }
            // Control de encendido/apagado (ON/OFF como 100%/0%)
            else if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("Q1 on (100%)");
              pwmValue1 = 100;
              board.pwmPercent(q1, 100);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("Q1 off (0%)");
              pwmValue1 = 0;
              board.pwmPercent(q1, 0);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("Q2 on (100%)");
              pwmValue2 = 100;
              board.pwmPercent(q2, 100);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("Q2 off (0%)");
              pwmValue2 = 0;
              board.pwmPercent(q2, 0);
            } else if (header.indexOf("GET /25/on") >= 0) {
              Serial.println("Q3 on (100%)");
              pwmValue3 = 100;
              board.pwmPercent(q3, 100);
            } else if (header.indexOf("GET /25/off") >= 0) {
              Serial.println("Q3 off (0%)");
              pwmValue3 = 0;
              board.pwmPercent(q3, 0);
            } else if (header.indexOf("GET /33/on") >= 0) {
              Serial.println("Q4 on (100%)");
              pwmValue4 = 100;
              board.pwmPercent(q4, 100);
            } else if (header.indexOf("GET /33/off") >= 0) {
              Serial.println("Q4 off (0%)");
              pwmValue4 = 0;
              board.pwmPercent(q4, 0);
            } else if (header.indexOf("GET /37/off") >= 0) {
              Serial.println("ALL off (0%)");
              pwmValue1 = 0;
              pwmValue2 = 0;
              pwmValue3 = 0;
              pwmValue4 = 0;
              board.pwmPercent(q1, 0);
              board.pwmPercent(q2, 0);
              board.pwmPercent(q3, 0);
              board.pwmPercent(q4, 0);
            } else if (header.indexOf("GET /37/on") >= 0) {
              Serial.println("ALL on (100%)");
              pwmValue1 = 100;
              pwmValue2 = 100;
              pwmValue3 = 100;
              pwmValue4 = 100;
              board.pwmPercent(q1, 100);
              board.pwmPercent(q2, 100);
              board.pwmPercent(q3, 100);
              board.pwmPercent(q4, 100);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<title>Control PWM BioLogic</title>");
            
            // CSS to style the on/off buttons 
            client.println("<style>");
            client.println("html { font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println("body {background: #020024; background: linear-gradient(90deg, rgba(2, 0, 36, 1) 0%, rgba(9,9,121,1) 35%, rgba(2, 16, 221, 0.7) 90%, rgba(4, 89, 216, 0.76)); color: white; box-sizing: border-box; padding: 20px;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 12px 30px; text-decoration: none; font-size: 18px; margin: 5px; cursor: pointer; width: 180px; text-align: center;box-sizing: border-box; border-radius: 5px;}");
            client.println(".button2 {background-color: #555555;}");
            client.println(".PWMB{ margin-top: 5%; margin-left: 15%; margin-right: 15%; box-sizing: border-box;position: relative;}");
            client.println("fieldset {border-radius: 10px; margin: 15px; padding: 15px; background: rgba(255, 255, 255, 0.1); border: 2px solid rgba(255, 255, 255, 0.2);}");
            client.println("legend {font-weight: bold; font-size: 18px; color: #fff;}");
            client.println(".slider {width: 90%; height: 30px; margin: 15px 0; -webkit-appearance: none; appearance: none; background: #ddd; outline: none; opacity: 0.7; transition: opacity .2s; border-radius: 10px;}");
            client.println(".slider:hover {opacity: 1;}");
            client.println(".slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; border-radius: 50%; background: #4CAF50; cursor: pointer;}");
            client.println(".slider::-moz-range-thumb {width: 35px; height: 35px; border-radius: 50%; background: #4CAF50; cursor: pointer;}");
            client.println(".reset {background-color: #f44336; color: white; padding: 8px 16px; border: none; border-radius: 4px; cursor: pointer; margin: 8px; font-size: 16px;}");
            client.println(".reset:hover {background-color: #d32f2f;}");
            client.println(".pwm-value {font-size: 28px; font-weight: bold; margin: 15px; color: #4CAF50;}");
            client.println(".channel-box {display: inline-block; width: 45%; margin: 10px; vertical-align: top;}");
            client.println("@media (max-width: 768px) {.channel-box {width: 90%;}}");
            client.println("</style>");
            
            // JavaScript para control PWM
            client.println("<script>");
            client.println("function updatePWM(channel, value) {");
            client.println("  document.getElementById('vPWM' + channel).innerHTML = value + '%';");
            client.println("  document.getElementById('sliderPWM' + channel).value = value;");
            client.println("  fetch('/q' + channel + 'pwm/' + value);");
            client.println("  console.log('Canal ' + channel + ' actualizado a ' + value + '%');");
            client.println("}");
            client.println("function resetPWM(channel) {");
            client.println("  updatePWM(channel, 0);");
            client.println("}");
            client.println("function setMaxPWM(channel) {");
            client.println("  updatePWM(channel, 100);");
            client.println("}");
            client.println("</script>");
            
            client.println("</head>");
            
            // Web Page Heading
            client.println("<body>");
            client.println("<h1>Control PWM BioLogic Board</h1>");
            client.println("<p><strong>Modo: Access Point</strong></p>");
            client.println("<p>IP: " + WiFi.softAPIP().toString() + "</p>");
            client.println("<hr>");
            
            // Contenedor principal para los 4 canales
            client.println("<div style='text-align: center;'>");
            
            // Sección Q1 con control PWM
            client.println("<div class='channel-box'>");
            client.println("<fieldset>");
            client.println("<legend>Q1 - PWM: " + String(pwmValue1) + "%</legend>");
            client.println("<input type='range' min='0' max='100' class='slider' id='sliderPWM1' value='" + String(pwmValue1) + "' oninput='updatePWM(1, this.value)'>");
            client.println("<p class='pwm-value' id='vPWM1'>" + String(pwmValue1) + "%</p>");
            client.println("<button class='reset' onclick='resetPWM(1)'>Reset (0%)</button>");
            client.println("<button class='button' onclick='setMaxPWM(1)'>Máximo (100%)</button>");
            client.println("</fieldset>");
            client.println("</div>");
            
            // Sección Q2 con control PWM
            client.println("<div class='channel-box'>");
            client.println("<fieldset>");
            client.println("<legend>Q2 - PWM: " + String(pwmValue2) + "%</legend>");
            client.println("<input type='range' min='0' max='100' class='slider' id='sliderPWM2' value='" + String(pwmValue2) + "' oninput='updatePWM(2, this.value)'>");
            client.println("<p class='pwm-value' id='vPWM2'>" + String(pwmValue2) + "%</p>");
            client.println("<button class='reset' onclick='resetPWM(2)'>Reset (0%)</button>");
            client.println("<button class='button' onclick='setMaxPWM(2)'>Máximo (100%)</button>");
            client.println("</fieldset>");
            client.println("</div>");
            
            client.println("</div>");
            
            client.println("<div style='text-align: center;'>");
            
            // Sección Q3 con control PWM
            client.println("<div class='channel-box'>");
            client.println("<fieldset>");
            client.println("<legend>Q3 - PWM: " + String(pwmValue3) + "%</legend>");
            client.println("<input type='range' min='0' max='100' class='slider' id='sliderPWM3' value='" + String(pwmValue3) + "' oninput='updatePWM(3, this.value)'>");
            client.println("<p class='pwm-value' id='vPWM3'>" + String(pwmValue3) + "%</p>");
            client.println("<button class='reset' onclick='resetPWM(3)'>Reset (0%)</button>");
            client.println("<button class='button' onclick='setMaxPWM(3)'>Máximo (100%)</button>");
            client.println("</fieldset>");
            client.println("</div>");
            
            // Sección Q4 con control PWM
            client.println("<div class='channel-box'>");
            client.println("<fieldset>");
            client.println("<legend>Q4 - PWM: " + String(pwmValue4) + "%</legend>");
            client.println("<input type='range' min='0' max='100' class='slider' id='sliderPWM4' value='" + String(pwmValue4) + "' oninput='updatePWM(4, this.value)'>");
            client.println("<p class='pwm-value' id='vPWM4'>" + String(pwmValue4) + "%</p>");
            client.println("<button class='reset' onclick='resetPWM(4)'>Reset (0%)</button>");
            client.println("<button class='button' onclick='setMaxPWM(4)'>Máximo (100%)</button>");
            client.println("</fieldset>");
            client.println("</div>");
            
            client.println("</div>");
            
            // Botones para control global
            client.println("<br><hr><br>");
            client.println("<fieldset>");
            client.println("<legend>Control Global</legend>");
            client.println("<p><strong>Acciones en todas las salidas:</strong></p>");
            client.println("<button class='button' style='background-color:#2196F3;' onclick='setAllMax()'>Todos al 100%</button>");
            client.println("<button class='button button2' onclick='resetAll()'>Todos al 0%</button>");
            client.println("</fieldset>");
            
            // JavaScript adicional para control global
            client.println("<script>");
            client.println("function setAllMax() {");
            client.println("  for(let i = 1; i <= 4; i++) {");
            client.println("    updatePWM(i, 100);");
            client.println("  }");
            client.println("}");
            client.println("function resetAll() {");
            client.println("  for(let i = 1; i <= 4; i++) {");
            client.println("    updatePWM(i, 0);");
            client.println("  }");
            client.println("}");
            client.println("</script>");
            
            client.println("<br><hr>");
            client.println("<p><small>BioLogic PWM Controller v1.0</small></p>");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}