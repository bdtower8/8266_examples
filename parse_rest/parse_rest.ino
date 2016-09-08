#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// wifi credentials
char* ssid = "";
char* password = "";

// parse server information
const char* appId = "";
const char* restKey = "";

const char *serverAddress = "";
const char *tableLoc = "/parse/classes/";
const char *tableName = "";
const char *columnName = "";

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

  // get all the data
  getData();
}

// wait for button press to post data
void loop() {
  // if enough time has passed
  if((millis() - lastPress) > minButtonPressDelay) {
    // check if the button has been pressed      
    if (digitalRead(buttonPin) != HIGH) {
      // post the event data
      postData(columnName, 1.0f);
      
      // track this button press
      lastPress = millis();
    }
  }
}

void getData() {  
  Serial.println("Starting Get");
  
  WiFiClient client;
  if(client.connect(serverAddress, 80)) {
    client.print("GET ");    
    client.print(tableLoc);
    client.print(tableName);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverAddress);
    client.println("Content-Type: application/json");
    
    client.print("X-Parse-Application-Id: ");
    client.println(appId);
    client.print("X-Parse-REST-API-Key: ");
    client.println(restKey);
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

// post the data
void postData(const char* signalName, float signalValue) {
  Serial.println("Starting Post");

  // create the json objects for transmission
  StaticJsonBuffer<500> jsonBufferParent;
  JsonObject& jsonParent = jsonBufferParent.createObject();
  jsonParent[signalName] = signalValue;

  // prepare the json object for transmission
  String jsonOut;
  jsonParent.printTo(jsonOut);

  // create a secure https client
  WiFiClient client;
  if(client.connect(serverAddress, 80)) {
    client.print("POST ");    
    client.print(tableLoc);
    client.print(tableName);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverAddress);
    client.println("Content-Type: application/json");
    
    client.print("X-Parse-Application-Id: ");
    client.println(appId);
    client.print("X-Parse-REST-API-Key: ");
    client.println(restKey);
    
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

