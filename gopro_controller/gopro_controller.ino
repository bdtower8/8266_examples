#include <WiFi.h>

// GoPro Settings
char* ssid = "GP12345678";
char* password = "secret1234";

// defaults
const char* host = "10.5.5.9";
const byte broadCastIp[] = { 10, 5, 5, 9 };
const int httpPort = 80;

// use gpio pin 21 for input
const int triggerPin = 21;

// track pin state
bool bWasTriggered = false;

// WoL support
// NOTE : The MAC address can be obtained from the camera by connecting a wireless device
//        to the camera's WiFi access point. Once connected, navigate to http://10.5.5.9/gp/gpControl/info
//        and you should see the MAC address as ap_mac in the JSON response
const int localPort = 7;
const int wolPort = 9;
byte remote_MAC_ADD[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

// keep-alive effort
long lastWake = 0;
long wakeRefresh = 30000;

long lastBruteForce = 0;
long bruteForceTimeout = 300000;

void wakeCamera()
{
  //Create a 102 byte array
  byte magicPacket[102];

  // Variables for cycling through the array
  int Cycle = 0, CycleMacAdd = 0, IndexArray = 0;

  // This for loop cycles through the array
  for ( Cycle = 0; Cycle < 6; Cycle++) {

    // The first 6 bytes of the array are set to the value 0xFF
    magicPacket[IndexArray] = 0xFF;

    // Increment the array index
    IndexArray++;
  }

  // Now we cycle through the array to add the GoPro address
  for ( Cycle = 0; Cycle < 16; Cycle++ ) {
    for ( CycleMacAdd = 0; CycleMacAdd < 6; CycleMacAdd++) {

      magicPacket[IndexArray] = remote_MAC_ADD[CycleMacAdd];

      // Increment the array index
      IndexArray++;
    }
  }
  
  WiFiUDP udp;

  //Begin UDP communication
  udp.begin(localPort);

  //Send the magic packet to wake up the GoPro out of sleep
  delay(250);

  //The magic packet is now broadcast to the GoPro IP address and port
  udp.beginPacket(broadCastIp, wolPort);
  udp.write(magicPacket, sizeof magicPacket);
  udp.endPacket();
  delay(3000);

  // Absolutely necessary to flush port of UDP junk for Wifi client communication
  udp.flush();
  delay(1000);

  //Stop UDP communication
  udp.stop();
  delay(1000);

  Serial.println("WoL Sent");
}

// check to see if the camera is awake
void checkConnection()
{  
  WiFiClient client;
  // connect to the the camera
  if (!client.connect(host, httpPort))
  {
    wakeCamera();
  }

  client.stop();
}

// send a command via HTTP to the camera
void sendCommand(String command)
{  
  WiFiClient client;
  
  // connect to the the camera
  if (client.connect(host, httpPort))
  {
    // send the command
    
    unsigned long timeout = millis();
    
    client.print(String("GET ") + command + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            Serial.println(command);
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        //Serial.print(line);
    }

    client.flush();
    client.stop();
  }
  else
  {
    Serial.println("Failed to run command");
    Serial.println(command);  
  }
}

// trigger a picture via http
void triggerPicture()
{
  sendCommand("/gp/gpControl/command/shutter?p=1");
}

// force keepAlive
void bruteForceKeepAlive()
{
  sendCommand("/gp/gpControl/command/mode?p=0");

  sendCommand("/gp/gpControl/setting/2/17");

  sendCommand("/gp/gpControl/command/shutter?p=1");

  delay(5000);
  
  sendCommand("/gp/gpControl/command/shutter?p=0");
  
  sendCommand("/gp/gpControl/command/mode?p=1");
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

  delay(2000);

  checkConnection();

  bruteForceKeepAlive();
  lastBruteForce = millis();

  // take a quick test picture to ensure comms
  triggerPicture();
}

void loop()
{
  // wake the camera every 5s
  if (millis() - lastWake > wakeRefresh)
  {
    checkConnection();
    lastWake = millis();
  }

  if (millis() - lastBruteForce > bruteForceTimeout)
  {
    bruteForceKeepAlive();
    lastBruteForce = millis();
  }

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
