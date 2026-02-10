#include <WiFi.h>
#include <BioLogic.h>

const char* ssid = "BioLogic Relay Control";
const char* password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String r1state = "off";
String r2state = "off";
String r3state = "off";
String r4state = "off";

BioLogic board;
// Assign output variables to GPIO pins

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  board.begin();
  pinMode(0,OUTPUT);
  digitalWrite(0, LOW);
  delay(10);
  digitalWrite(0, HIGH);
  delay(100);
  Serial.begin(115200);
  delay(100);
  board.pinMode(r1, OUTPUT);
  board.pinMode(r2, OUTPUT);
  board.pinMode(r3, OUTPUT);
  board.pinMode(r4, OUTPUT);
  // Set outputs to LOW (reles apagados)
  board.digitalWrite(r1, LOW);
  board.digitalWrite(r2, LOW);
  board.digitalWrite(r3, LOW);
  board.digitalWrite(r4, LOW);
  
  WiFi.softAP(ssid, password);
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
    delay(100);
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
            
            // Control de los 4 rel�s
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("r1 on");
             r1state = "on";
              board.digitalWrite(r1, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("r1 off");
              r1state = "off";
              board.digitalWrite(r1, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("r2 on");
              r2state = "on";
              board.digitalWrite(r2, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("r2 off");
              r2state = "off";
              board.digitalWrite(r2, LOW);
            } else if (header.indexOf("GET /25/on") >= 0) {
              Serial.println("r3 on");
              r3state = "on";
              board.digitalWrite(r3, HIGH);
            } else if (header.indexOf("GET /25/off") >= 0) {
              Serial.println("r3 off");
              r3state = "off";
              board.digitalWrite(r3, LOW);
            } else if (header.indexOf("GET /33/on") >= 0) {
              Serial.println("r4 on");
              r4state = "on";
              board.digitalWrite(r4, HIGH);
            } else if (header.indexOf("GET /33/off") >= 0) {
              Serial.println("r4 off");
              r4state = "off";
              board.digitalWrite(r4, LOW);
            } else if (header.indexOf("GET /37/off") >= 0) {
              Serial.println("ALL off");
              r1state = "off";
              r2state = "off";
              r3state = "off";
              r4state = "off";
              board.digitalWrite(r1, LOW);
              board.digitalWrite(r2, LOW);
              board.digitalWrite(r3, LOW);
              board.digitalWrite(r4, LOW);
            } else if (header.indexOf("GET /37/on") >= 0) {
              Serial.println("ALL on");
              r1state = "on";
              r2state = "on";
              r3state = "on";
              r4state = "on";
              board.digitalWrite(r1, HIGH);
              board.digitalWrite(r2, HIGH);
              board.digitalWrite(r3, HIGH);
              board.digitalWrite(r4, HIGH);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            // CSS to style the on/off buttons 
            client.println("<style>html { font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println("body {background: #020024; background: linear-gradient(90deg, rgba(2, 0, 36, 1) 0%, rgba(9,9,121,1) 35%, rgba(2, 16, 221, 0.7) 90%, rgba(4, 89, 216, 0.76)); color: white; box-sizing: border-box; }");
            client.println(".button { background-color: #4CAF50; border: solid; border-color: #4CAF50; border-radius: 8px; color: white; padding: 16px 40px; text-decoration: none; font-size: 20px; margin: 0.5px; cursor: pointer; width: 200px; text-align: center;box-sizing: border-box;}");
            client.println(".button2 {background-color: #555555; border-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>BioLogic Control de 4 Relés</h1>");
            client.println("<p><strong>Modo: Access Point</strong></p>");
            client.println("<p>IP: " + WiFi.softAPIP().toString() + "</p>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>R1 - Estado: " + r1state + "</p>");
            if (r1state=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ENCENDER <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"currentColor\" class=\"icon icon-tabler icons-tabler-filled icon-tabler-bulb\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"/><path d=\"M4 11a1 1 0 0 1 .117 1.993l-.117 .007h-1a1 1 0 0 1 -.117 -1.993l.117 -.007h1z\" /><path d=\"M12 2a1 1 0 0 1 .993 .883l.007 .117v1a1 1 0 0 1 -1.993 .117l-.007 -.117v-1a1 1 0 0 1 1 -1z\" /><path d=\"M21 11a1 1 0 0 1 .117 1.993l-.117 .007h-1a1 1 0 0 1 -.117 -1.993l.117 -.007h1z\" /><path d=\"M4.893 4.893a1 1 0 0 1 1.32 -.083l.094 .083l.7 .7a1 1 0 0 1 -1.32 1.497l-.094 -.083l-.7 -.7a1 1 0 0 1 0 -1.414z\" /><path d=\"M17.693 4.893a1 1 0 0 1 1.497 1.32l-.083 .094l-.7 .7a1 1 0 0 1 -1.497 -1.32l.083 -.094l.7 -.7z\" /><path d=\"M14 18a1 1 0 0 1 1 1a3 3 0 0 1 -6 0a1 1 0 0 1 .883 -.993l.117 -.007h4z\" /><path d=\"M12 6a6 6 0 0 1 3.6 10.8a1 1 0 0 1 -.471 .192l-.129 .008h-6a1 1 0 0 1 -.6 -.2a6 6 0 0 1 3.6 -10.8z\" /></svg></button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">APAGAR <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" class=\"icon icon-tabler icons-tabler-outline icon-tabler-bulb\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"/><path d=\"M3 12h1m8 -9v1m8 8h1m-15.4 -6.4l.7 .7m12.1 -.7l-.7 .7\" /><path d=\"M9 16a5 5 0 1 1 6 0a3.5 3.5 0 0 0 -1 3a2 2 0 0 1 -4 0a3.5 3.5 0 0 0 -1 -3\" /><path d=\"M9.7 17l4.6 0\" /></svg></button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>R2 - Estado: " + r2state + "</p>");
            if (r2state=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ENCENDER <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"currentColor\" class=\"icon icon-tabler icons-tabler-filled icon-tabler-bulb\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"/><path d=\"M4 11a1 1 0 0 1 .117 1.993l-.117 .007h-1a1 1 0 0 1 -.117 -1.993l.117 -.007h1z\" /><path d=\"M12 2a1 1 0 0 1 .993 .883l.007 .117v1a1 1 0 0 1 -1.993 .117l-.007 -.117v-1a1 1 0 0 1 1 -1z\" /><path d=\"M21 11a1 1 0 0 1 .117 1.993l-.117 .007h-1a1 1 0 0 1 -.117 -1.993l.117 -.007h1z\" /><path d=\"M4.893 4.893a1 1 0 0 1 1.32 -.083l.094 .083l.7 .7a1 1 0 0 1 -1.32 1.497l-.094 -.083l-.7 -.7a1 1 0 0 1 0 -1.414z\" /><path d=\"M17.693 4.893a1 1 0 0 1 1.497 1.32l-.083 .094l-.7 .7a1 1 0 0 1 -1.497 -1.32l.083 -.094l.7 -.7z\" /><path d=\"M14 18a1 1 0 0 1 1 1a3 3 0 0 1 -6 0a1 1 0 0 1 .883 -.993l.117 -.007h4z\" /><path d=\"M12 6a6 6 0 0 1 3.6 10.8a1 1 0 0 1 -.471 .192l-.129 .008h-6a1 1 0 0 1 -.6 -.2a6 6 0 0 1 3.6 -10.8z\" /></svg> </button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">APAGAR <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" class=\"icon icon-tabler icons-tabler-outline icon-tabler-bulb\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"/><path d=\"M3 12h1m8 -9v1m8 8h1m-15.4 -6.4l.7 .7m12.1 -.7l-.7 .7\" /><path d=\"M9 16a5 5 0 1 1 6 0a3.5 3.5 0 0 0 -1 3a2 2 0 0 1 -4 0a3.5 3.5 0 0 0 -1 -3\" /><path d=\"M9.7 17l4.6 0\" /></svg></button></a></p>");
            }
            
            // Display current state, and ON/OFF buttons for GPIO 25  
            client.println("<p>R3 - Estado: " + r3state + "</p>");
            if (r3state=="off") {
              client.println("<p><a href=\"/25/on\"><button class=\"button\">ENCENDER <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"currentColor\" class=\"icon icon-tabler icons-tabler-filled icon-tabler-bulb\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"/><path d=\"M4 11a1 1 0 0 1 .117 1.993l-.117 .007h-1a1 1 0 0 1 -.117 -1.993l.117 -.007h1z\" /><path d=\"M12 2a1 1 0 0 1 .993 .883l.007 .117v1a1 1 0 0 1 -1.993 .117l-.007 -.117v-1a1 1 0 0 1 1 -1z\" /><path d=\"M21 11a1 1 0 0 1 .117 1.993l-.117 .007h-1a1 1 0 0 1 -.117 -1.993l.117 -.007h1z\" /><path d=\"M4.893 4.893a1 1 0 0 1 1.32 -.083l.094 .083l.7 .7a1 1 0 0 1 -1.32 1.497l-.094 -.083l-.7 -.7a1 1 0 0 1 0 -1.414z\" /><path d=\"M17.693 4.893a1 1 0 0 1 1.497 1.32l-.083 .094l-.7 .7a1 1 0 0 1 -1.497 -1.32l.083 -.094l.7 -.7z\" /><path d=\"M14 18a1 1 0 0 1 1 1a3 3 0 0 1 -6 0a1 1 0 0 1 .883 -.993l.117 -.007h4z\" /><path d=\"M12 6a6 6 0 0 1 3.6 10.8a1 1 0 0 1 -.471 .192l-.129 .008h-6a1 1 0 0 1 -.6 -.2a6 6 0 0 1 3.6 -10.8z\" /></svg></button></a></p>");
            } else {
              client.println("<p><a href=\"/25/off\"><button class=\"button button2\">APAGAR <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" class=\"icon icon-tabler icons-tabler-outline icon-tabler-bulb\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"/><path d=\"M3 12h1m8 -9v1m8 8h1m-15.4 -6.4l.7 .7m12.1 -.7l-.7 .7\" /><path d=\"M9 16a5 5 0 1 1 6 0a3.5 3.5 0 0 0 -1 3a2 2 0 0 1 -4 0a3.5 3.5 0 0 0 -1 -3\" /><path d=\"M9.7 17l4.6 0\" /></svg></button></a></p>");
            }
            
            // Display current state, and ON/OFF buttons for GPIO 33  
            client.println("<p>R4 - Estado: " + r4state + "</p>");
            if (r4state=="off") {
              client.println("<p><a href=\"/33/on\"><button class=\"button\">ENCENDER <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"currentColor\" class=\"icon icon-tabler icons-tabler-filled icon-tabler-bulb\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"/><path d=\"M4 11a1 1 0 0 1 .117 1.993l-.117 .007h-1a1 1 0 0 1 -.117 -1.993l.117 -.007h1z\" /><path d=\"M12 2a1 1 0 0 1 .993 .883l.007 .117v1a1 1 0 0 1 -1.993 .117l-.007 -.117v-1a1 1 0 0 1 1 -1z\" /><path d=\"M21 11a1 1 0 0 1 .117 1.993l-.117 .007h-1a1 1 0 0 1 -.117 -1.993l.117 -.007h1z\" /><path d=\"M4.893 4.893a1 1 0 0 1 1.32 -.083l.094 .083l.7 .7a1 1 0 0 1 -1.32 1.497l-.094 -.083l-.7 -.7a1 1 0 0 1 0 -1.414z\" /><path d=\"M17.693 4.893a1 1 0 0 1 1.497 1.32l-.083 .094l-.7 .7a1 1 0 0 1 -1.497 -1.32l.083 -.094l.7 -.7z\" /><path d=\"M14 18a1 1 0 0 1 1 1a3 3 0 0 1 -6 0a1 1 0 0 1 .883 -.993l.117 -.007h4z\" /><path d=\"M12 6a6 6 0 0 1 3.6 10.8a1 1 0 0 1 -.471 .192l-.129 .008h-6a1 1 0 0 1 -.6 -.2a6 6 0 0 1 3.6 -10.8z\" /></svg></button></a></p>");
            } else {
              client.println("<p><a href=\"/33/off\"><button class=\"button button2\">APAGAR <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" class=\"icon icon-tabler icons-tabler-outline icon-tabler-bulb\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"/><path d=\"M3 12h1m8 -9v1m8 8h1m-15.4 -6.4l.7 .7m12.1 -.7l-.7 .7\" /><path d=\"M9 16a5 5 0 1 1 6 0a3.5 3.5 0 0 0 -1 3a2 2 0 0 1 -4 0a3.5 3.5 0 0 0 -1 -3\" /><path d=\"M9.7 17l4.6 0\" /></svg></button></a></p>");
            }
            
            // Bot�n para encender/apagar todos
            client.println("<br><hr><br>");
            client.println("<p><strong>Control Total</strong></p>");
            client.println("<p><a href=\"/37/on\"><button class=\"button\" style=\"background-color:#2196F3;\">ENCENDER TODOS <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"currentColor\" class=\"icon icon-tabler icons-tabler-filled icon-tabler-bulb\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"/><path d=\"M4 11a1 1 0 0 1 .117 1.993l-.117 .007h-1a1 1 0 0 1 -.117 -1.993l.117 -.007h1z\" /><path d=\"M12 2a1 1 0 0 1 .993 .883l.007 .117v1a1 1 0 0 1 -1.993 .117l-.007 -.117v-1a1 1 0 0 1 1 -1z\" /><path d=\"M21 11a1 1 0 0 1 .117 1.993l-.117 .007h-1a1 1 0 0 1 -.117 -1.993l.117 -.007h1z\" /><path d=\"M4.893 4.893a1 1 0 0 1 1.32 -.083l.094 .083l.7 .7a1 1 0 0 1 -1.32 1.497l-.094 -.083l-.7 -.7a1 1 0 0 1 0 -1.414z\" /><path d=\"M17.693 4.893a1 1 0 0 1 1.497 1.32l-.083 .094l-.7 .7a1 1 0 0 1 -1.497 -1.32l.083 -.094l.7 -.7z\" /><path d=\"M14 18a1 1 0 0 1 1 1a3 3 0 0 1 -6 0a1 1 0 0 1 .883 -.993l.117 -.007h4z\" /><path d=\"M12 6a6 6 0 0 1 3.6 10.8a1 1 0 0 1 -.471 .192l-.129 .008h-6a1 1 0 0 1 -.6 -.2a6 6 0 0 1 3.6 -10.8z\" /></svg></button></a></p>");
            client.println("<p><a href=\"/37/off\"><button class=\"button button2\">APAGAR TODOS <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" class=\"icon icon-tabler icons-tabler-outline icon-tabler-bulb\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"/><path d=\"M3 12h1m8 -9v1m8 8h1m-15.4 -6.4l.7 .7m12.1 -.7l-.7 .7\" /><path d=\"M9 16a5 5 0 1 1 6 0a3.5 3.5 0 0 0 -1 3a2 2 0 0 1 -4 0a3.5 3.5 0 0 0 -1 -3\" /><path d=\"M9.7 17l4.6 0\" /></svg></button></a></p>");
            
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