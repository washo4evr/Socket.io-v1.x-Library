#include "SocketIOClient.h"
#include "Ethernet.h"
#include "SPI.h"
#include <ArduinoJson.h>

SocketIOClient client;

byte mac[] = { 0xAA, 0x00, 0xBE, 0xEF, 0xFE, 0xEE };
char hostname[] = "10.10.0.184";
int port = 3000;

extern String RID;
extern String Rname;
extern String Rcontent;
// websocket message handler: do something with command from server
void ondata(SocketIOClient client, char *data)
{
	Serial.print(data);
}
unsigned long previousMillis = 0; 
long interval = 10000; 


void setup() {
	//pinMode(13, OUTPUT);    //for some ethernet shield on the mega : disables the SD and enables the W5100
	digitalWrite(10, LOW);
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);
	Serial.begin(9600);
  delay(1000);

  
	Ethernet.begin(mac);

	client.setDataArrivedDelegate(ondata);
	if (!client.connect(hostname, port))
		Serial.println(F("Not connected."));
	if (client.connected())
	{
    String JSON;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["message"]= "Connected !!!!";
    root.printTo(JSON);
    client.sendJSON("connection",JSON);
	}
	else
	{
		Serial.println("Connection Error");
		while(1);
	}
	delay(1000);
}

void loop()
{
	unsigned long currentMillis = millis();
	if(currentMillis - previousMillis > interval)
	{
	  previousMillis = currentMillis; 
	  //client.heartbeat(0);
    String JSON;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["message"]= "Time please?";
    root.printTo(JSON);
    Serial.println(JSON);
    client.sendJSON("atime",JSON);
	}
	if (client.monitor(true))
	{
	  Serial.print("JSON is:  ");
		Serial.println(Rcontent);
    String JSON;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(Rcontent);

    const char * nowTime  = root["time"];
    Serial.print(nowTime);
  }
}
