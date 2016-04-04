#include <ESP8266WiFi.h>

char* ssid = "";
char* password = "";

IPAddress server(192,168,0,21);
WiFiClient client;

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

  // post a value to the server
  if(client.connect(server, 80)) {
    client.println("POST /VALUE=1.7 HTTP/1.1");
    client.println();
  }
}

void loop() {
}
