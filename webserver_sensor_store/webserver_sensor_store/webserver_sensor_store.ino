// a minimal webserver that stores some sensor data and a time stamp
// simply entering:
// 'http://192.168.0.21/DATE=20160405&TIME=144630&TEMP=25.45&ANGLE=45.09'
// into a browser would set the values accordingly

#include <ESP8266WiFi.h>

const char* ssid = "";
const char* password = "";

int iTimestampDate = 0;
int iTimestampTime = 0;
float fSensorValueAngle = 0.0f;
float fSensorValueTemperature = 0.0f;

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

  // grab the values from the request
  // this code relies on the fact that .toFloat() and .toInt()
  String sSearchTerm;

  // temperature
  sSearchTerm = "TEMP=";
  if(request.indexOf(sSearchTerm) != -1) {
    String trimmed = request.substring(request.indexOf(sSearchTerm) + sSearchTerm.length());
    Serial.println("New value = " + trimmed);
    fSensorValueTemperature = trimmed.toFloat();
  }

  // angle
  sSearchTerm = "ANGLE=";
  if(request.indexOf(sSearchTerm) != -1) {
    String trimmed = request.substring(request.indexOf(sSearchTerm) + sSearchTerm.length());
    Serial.println("New value = " + trimmed);
    fSensorValueAngle = trimmed.toFloat();
  }

  // date
  sSearchTerm = "DATE=";
  if(request.indexOf(sSearchTerm) != -1) {
    String trimmed = request.substring(request.indexOf(sSearchTerm) + sSearchTerm.length());
    Serial.println("New value = " + trimmed);
    iTimestampDate = trimmed.toInt();
  }
  
  // time
  sSearchTerm = "TIME=";
  if(request.indexOf(sSearchTerm) != -1) {
    String trimmed = request.substring(request.indexOf(sSearchTerm) + sSearchTerm.length());
    Serial.println("New value = " + trimmed);
    iTimestampTime = trimmed.toInt();
  }
  

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.print("The stored temperature is now: ");
  client.print(fSensorValueTemperature);
  client.println("<br><br>");
  client.print("The stored angle is now: ");
  client.print(fSensorValueAngle);
  client.println("<br><br>");
  client.print("The stored timestamp is now: ");
  client.print(iTimestampDate);
  client.print(" ");
  client.print(iTimestampTime);
  client.println("<br><br>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
