#include <ESP8266WiFi.h>

const char* ssid = "";
const char* password = "";

float fValueStore = 0.0f;

WiFiServer server(80);

void setup() {
  // set the serial baud rate
  Serial.begin(115200);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // grab the new value from the request  
  String sSearchTerm = "/VALUE=";
  if(request.indexOf(sSearchTerm) != -1) {
    String trimmed = request.substring(request.indexOf(sSearchTerm) + sSearchTerm.length());
    Serial.println("New value = " + trimmed);
    fValueStore = trimmed.toFloat();
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.print("The stored value is now: ");
  client.print(fValueStore);
  client.println("<br><br>");  
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
