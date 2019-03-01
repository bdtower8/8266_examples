#include <WiFi.h>

// GoPro Settings
char* ssid = "GP12345678";
char* password = "secret1234";

// defaults
const char* host = "10.5.5.9";
const int httpPort = 80;

// use gpio pin 21 for input
const int triggerPin = 21;

// track pin state
bool bWasTriggered = false;

// trigger a picture via http
void triggerPicture()
{
  // connect to the the camera
  WiFiClient client;
  if(client.connect(host, httpPort))
  {
    Serial.println("Connected");

    //Command for triggering an image
    String triggerURL = "/gp/gpControl/command/shutter?p=1";
    client.print(String("GET ") + triggerURL + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "Connection: close\r\n\r\n");
    
    Serial.println("Triggered");
  }
  else
  {
    // failed to connect
    Serial.println("Failed to connect");
  }

  // disconnect
  client.stop();  
}

void setup() {
  // set the serial baud rate
  Serial.begin(115200);

  // setup the input pin
  pinMode(triggerPin, INPUT);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  // wait for a connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");  
}

void loop()
{
  // get the pin state
  int buttonState = digitalRead(triggerPin);

  // check for falling edge
  if(buttonState == LOW && bWasTriggered) {
    bWasTriggered = false;
    triggerPicture();
  }

  // if receiving high signal
  if(buttonState == HIGH) {
    bWasTriggered = true;
  }

  // if receiving low signal
  if(buttonState == LOW) {
    bWasTriggered = false;
  }

  delay(10);
}
