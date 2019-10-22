# Socket.io Library for v1.x and v2.x
Socket.io Library for Arduino
## Navgation
- [News](#news)
- [Download](#download)
- [Contributors](#contributors)
- [Examples](#examples)
  - [W5100](#example-w5100)
  - [ESP8266](#example-ESP8266)
  - [ESP32](#example-ESP32)

---
## <div id="news" />News
1. Works with W5100 & ENC28J60 as well as ESP8266 and ESP32 (beta)
2. Library will not work when the ESP8266 is being driven by a Uno.
3. Support for JSON was added using [this library](https://github.com/bblanchon/ArduinoJson).
4. Instead of using client.send, you can now use client.sendJSON
5. Thanks to @funkyguy4000, users can now use #define ESP8266, #define W5100 or #define ENC28J60
no need to edit socketio.h anymore
6. headers are not yet handled, soon
7. incoming messages can now be longer than 125 bytes (at least up to 255 bytes)

---
## <div id="download" />Download Releases
Download in releases and include zip file in arduino ide.
[Release 1.0](/releases/tag/1.0)
---

REST API added: ```getREST(path)```, ```postREST(path, type, data)```, ```putREST(path, type, data)```, ```deleteREST(path)```

---
## <div id="examples" /> Examples

### <div id="example-w5100" /> W5100

```
  #define W5100
  #include <SocketIOClient.h>

  SocketIOClient client;
  byte mac[] = { 0xAA, 0x00, 0xBE, 0xEF, 0xFE, 0xEE };
  char hostname[] = "192.168.0.5";
  int port = 1234;

  extern String RID;
  extern String Rname;
  extern String Rcontent;

  unsigned long previousMillis = 0; 
  long interval = 10000; 
  void setup() {
    pinMode(10, OUTPUT);    //for some ethernet shield on the mega : disables the SD and enables the W5100
    digitalWrite(10, LOW);
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    Serial.begin(9600);

    Ethernet.begin(mac);

    if (!client.connect(hostname, port))
      Serial.println(F("Not connected."));

    if (client.connected()){
      client.send("connection", "message", "Connected !!!!");
    }else{
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
      client.send("atime", "message", "Time please?");
    }
    if (client.monitor())
    {
      Serial.println(RID);
      if (RID == "atime" && Rname == "time")
      {
        Serial.print("Time is ");
        Serial.println(Rcontent);
      }
    }
  }
```
### <div id="example-ESP8266" /> ESP8266

```
  #define ESP8266
  #include <SocketIOClient.h>

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
    if (client.connected()){
      client.send("connection", "message", "Connected !!!!");
    }
  }

  void loop() {
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
```
### <div id="example-ESP32" /> ESP32

```
  #define ESP32
  #include <SocketIOClient.h>

  SocketIOClient client;
  const char* ssid     = "xxxxxxxxxxxxxxxxx";
  const char* password = "xxxxxxxxxxxxx";

  char host[] = "192.168.1.1";
  int port = 1234;
  extern String RID;
  extern String Rname;
  extern String Rcontent;

  unsigned long previousMillis = 0;
  long interval = 5000;
  unsigned long lastreply = 0;
  unsigned long lastsend = 0;

  void setup() {
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
    if (client.connected()){
      client.send("connection", "message", "Connected !!!!");
    }
  }

  void loop() {
  unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > interval)
    {
      previousMillis = currentMillis;
      //client.heartbeat(0);
      Serial.print("**************************************  ");
      Serial.println(lastsend);
      client.send("atime", "message", "Time please?");
      lastsend = millis();
      Serial.print("**************************************  ");
      Serial.println(lastsend);
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
```
---



## <div id="contributors" />Contributors

1. [washo4evr](https://github.com/washo4evr)
1. [all-lala](https://github.com/al-lala)
1. [andreghisleni](https://github.com/andreghisleni/)
