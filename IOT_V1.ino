#include <WiFi.h>

const char* ssid = "";
const char* password = "";
//tu było moje hasło oraz moja sieć Wi-fi, dzięki której połączyłem płytke Arduino z siecią 

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(2, OUTPUT); 
  pinMode(A1, INPUT); 
  digitalWrite(2, HIGH); 

  pinMode(4, OUTPUT); 
  digitalWrite(4, LOW); 

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    String dataValue = "";
    bool isDataLine = false;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            int analogValue = analogRead(A1);
            Serial.print("Analog value from A1: ");
            Serial.println(analogValue);

            if (dataValue.length() > 0) {
              Serial.print("Received data: ");
              Serial.println(dataValue);
              if (dataValue == "1") {
                digitalWrite(4, HIGH);
              } else {
                digitalWrite(4, LOW); 
              }
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<!DOCTYPE HTML><html><body>");
            client.println("<form method='GET'>");
            client.println("Enter data: <input type='text' name='data'><input type='submit'></form>");
            client.println("<p>Analog value from A1: " + String(analogValue) + "</p>"); 
            client.println("<script>");
            client.println("setTimeout(function() { location.reload(); }, 100);"); 
            client.println("</script>");
            client.println("</body></html>");
            break;
          } else {
            if (currentLine.startsWith("GET /?data=")) {
              int dataStart = currentLine.indexOf('=') + 1;
              int dataEnd = currentLine.indexOf(' ', dataStart);
              dataValue = currentLine.substring(dataStart, dataEnd);
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}
