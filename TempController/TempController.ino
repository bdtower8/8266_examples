// A simple temperature controller that sends an email if the sensor stops updating data
//  - Entering 'http://192.168.0.21/update?TEMP=25.45&TARGET=18.00' into a browser updates the temperature controller

#include <ESP8266WebServer.h>

const int iFridgePin = 15;
const int iHeaterPin = 16;

const char* ssid = "";
const char* password = "";

float fSensorValueTemperature = 0.0f;
float fAngleX = 0.0f;
float fAngleY = 0.0f;
float fAngleZ = 0.0f;

float fSafeMarginC = 0.5f;
float fTargetTemperature = 18.3f;

ESP8266WebServer server(80);

void enableHeater(bool bEnable) {
  if(bEnable) {
    Serial.println("Enabling heater");
    digitalWrite(iHeaterPin, HIGH);
  } else {
    Serial.println("Disabling heater");
    digitalWrite(iHeaterPin, LOW);
  }  
}

void enableFridge(bool bEnable) {
  if(bEnable) {
    Serial.println("Enabling fridge");
    digitalWrite(iFridgePin, HIGH);
  } else {
    Serial.println("Disabling fridge");
    digitalWrite(iFridgePin, LOW);
  }  
}

void handleUpdate() {
  bool bNewData = false;
  
  if(server.hasArg("TEMP")) {
    fSensorValueTemperature = server.arg("TEMP").toFloat();
    bNewData = true;
  }

  if(server.hasArg("TARGET")) {
    fTargetTemperature = server.arg("TARGET").toFloat();
    bNewData = true;
  }

  String sResponse = "Current Temperature: " + String(fSensorValueTemperature) + "\nTarget Temperature: " + String(fTargetTemperature);

  // if we have new data
  if(bNewData) {
    // update the temperature controller
    updateTemperatureController();

    sResponse += "\n*Updated*";
  }

  Serial.println(sResponse);  
  server.send(200, "text/plain", sResponse);
}

void handleRoot() {
  String sResponse = "Current Temperature: " + String(fSensorValueTemperature) + "\nTarget Temperature: " + String(fTargetTemperature);
  Serial.println(sResponse);
  server.send(200, "text/plain", sResponse);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args();++i) {
    message += " " + server.argName (i) + ": " + server.arg (i) + "\n";
  }

  server.send (404, "text/plain", message);
}

void updateTemperatureController() {
  Serial.println("New data, updating controller");
  
  // if it's too cold, turn on heater
  if(fSensorValueTemperature < fTargetTemperature) {
    enableHeater(true);
    enableFridge(false);
  } else {
    // if it's too warm, turn on fridge
    if(fSensorValueTemperature > (fTargetTemperature + fSafeMarginC)) {
      enableFridge(true);
      enableHeater(false);
    } else {
      // if it's within the safe margin, turn everything off
      enableFridge(false);
      enableHeater(false);        
    }
  }  
}

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
  Serial.print("WiFi connected @ ");
  Serial.println(WiFi.localIP());

  // init gpio
  pinMode(iFridgePin, OUTPUT);
  pinMode(iHeaterPin, OUTPUT);

  // turn off everything
  enableHeater(false);
  enableFridge(false);

  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.onNotFound(handleNotFound);

  server.begin();
  
  Serial.println("Ready to go...");
}

void loop() {
  
  server.handleClient();
  
}
