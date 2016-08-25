#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// wifi credentials
char* ssid = "";
char* password = "";

// initial state account information
const char* accessKey = "";
const char* bucketKey = "";

// server address
const char *serverAddress = "groker.initialstate.com";
const char *tableLoc = "/api/events";

// the gpio of the pushbutton
const int buttonPin = 2;

// require minButtonPressDelay ms between posts
const int minButtonPressDelay = 3000;

// timestamp the button press
uint32_t lastPress = 0;

// initialize serial, pushbutton as input, wifi
void setup() {
  // set the serial baud rate
  Serial.begin(115200);

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  // init the wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

// wait for button press to post data
void loop() {
  // if enough time has passed
  if((millis() - lastPress) > minButtonPressDelay) {
    // check if the button has been pressed      
    if (digitalRead(buttonPin) != HIGH) {
      // post the event data
      postData("value", 2.0f);
      
      // track this button press
      lastPress = millis();
    }
  }
}

// post the data
void postData(const char* signalName, float signalValue) {
  Serial.println("Starting Post");

  // create the json objects for transmission
  StaticJsonBuffer<500> jsonBufferParent;
  JsonObject& jsonParent = jsonBufferParent.createObject();
  jsonParent["key"] = signalName;
  jsonParent["value"] = signalValue;

  // prepare the json object for transmission
  String jsonOut;
  jsonParent.printTo(jsonOut);

  // create a secure https client
  WiFiClientSecure client;
  if(client.connect(serverAddress, 443)) {
    client.print("POST ");
    client.print(tableLoc);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverAddress);    
    client.println("Content-Type: application/json");
    client.println("User-Agent:ESP8266");
    client.print("X-IS-AccessKey: ");
    client.println(accessKey);
    client.print("X-IS-BucketKey: ");
    client.println(bucketKey);
    
    // add the payload
    client.print("Content-Length: ");
    client.println(jsonParent.measureLength());
    client.println();
    client.println(jsonOut);
    client.println();  

    // read the response from the network
    String reply = client.readString();

    Serial.println();  
    Serial.println("HTTP Reply:");
    Serial.println(reply);
    
  } else {
    Serial.print("Failed to connect to server: ");
    Serial.println(serverAddress);
  }
  
  Serial.println("Post Completed");
}
