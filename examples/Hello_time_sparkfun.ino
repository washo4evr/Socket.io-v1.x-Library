/*  DISCLAIMER : IT MAY NOT WORK... 
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */
#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SparkFunESP8266WiFi.h>


StaticJsonBuffer<200> jsonBuffer;
int connectedWF = 0;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "firma-transport.ro";

SocketIOClient client;
const char* ssid     = "SSID";
const char* password = "WPA KEY";

char host[] = "192.168.0.5";
int port = 1234;
extern String RID;
extern String Rname;
extern String Rcontent;

unsigned long previousMillis = 0;
long interval = 5000;
unsigned long lastreply = 0;
unsigned long lastsend = 0;
String JSON;
JsonObject& root = jsonBuffer.createObject();
void setup() {

if (esp8266.begin()) {
       Serial.println("ESP8266 ready to go!");
       connectToWifi();
   }
   else {
       Serial.println("Unable to communicate with the ESP8266 :(");
       
       
       delay(2000);

  root["sensor"] = "gps";
  root["time"] = 1351824120;
  JsonArray& data = root.createNestedArray("data");
  data.add(double_with_n_digits(48.756080, 6));
  data.add(double_with_n_digits(2.302038, 6));
  root.printTo(JSON);
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
if (client.connected())
  {
    client.send("connection", "message", "Connected !!!!");
  }
}

void connectToWifi() {
  int retVal;
   retVal = esp8266.connect("ssid", "pass");
   if (retVal < 0)
      { 
          Serial.println("Not connected");                   
      }
      else {
        IPAddress myIP = esp8266.localIP(); 
        Serial.println("Connected to internet OK"); 
        connectedWF = 1;         
      }   
}    




void loop() {
if ( connectedWF == 0 ) {
        connectToWifi();
   }
unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {
    previousMillis = currentMillis;
    //client.heartbeat(0);
    

  
  
  Serial.println(JSON);
    client.send("atime", "message", "Time please?");
    client.sendJSON("JSON", JSON);
    
    lastsend = millis();
  }
  if (client.monitor())
  {
    lastreply = millis(); 
    Serial.println(RID);
    if (RID == "atime" && Rname == "time")
    {
      Serial.print("Il est ");
      Serial.println(Rcontent);
    }
  }
}
