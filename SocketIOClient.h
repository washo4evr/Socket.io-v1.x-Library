/*
socket.io-arduino-client: a Socket.IO client for the Arduino
Based on the Kevin Rohling WebSocketClient & Bill Roy Socket.io Lbrary
Copyright 2015 Florent Vidal
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:
JSON support added using https://github.com/bblanchon/ArduinoJson
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Arduino.h"

#if defined(W5100)
#include <Ethernet.h>
#include "SPI.h"					//For W5100
#elif defined(ENC28J60)
#include <UIPEthernet.h>
#include "SPI.h"					//For ENC28J60
#elif defined(ESP8266)
#include <ESP8266WiFi.h>				//For ESP8266
#elif defined(ESP32)
#include <WiFi.h>					//For ESP32
#elif (!defined(ESP32) && !defined(ESP8266) && !defined(W5100) && !defined(ENC28J60))	//If no interface is defined
#error "Please specify an interface such as W5100, ENC28J60, ESP8266 or ESP32"
#error "above your includes like so : #define ESP8266 "
#endif

// Length of static data buffers
#define DATA_BUFFER_LEN 120
#define SID_LEN 24

class SocketIOClient {
public:
	bool connect(char hostname[], int port = 80);
	bool connectHTTP(char hostname[], int port = 80);
	bool connected();
	void disconnect();
	bool reconnect(char hostname[], int port = 80);
	bool monitor();
	void sendMessage(String message = "");
	void send(String RID, String Rname, String Rcontent);
	void sendNSP();
	void sendJSON(String RID, String JSON);
	void heartbeat(int select);
	void clear();
	void getREST(String path);
	void postREST(String path, String type, String data);
	void putREST(String path, String type, String data);
	void deleteREST(String path);
private:
	void parser(int index);
	void sendHandshake(char hostname[]);
#if defined(W5100) || defined(ENC28J60)
	EthernetClient client;				//For ENC28J60 or W5100
#endif
#if defined(ESP8266) || defined(ESP32)
	WiFiClient client;				//For ESP8266 or ESP32
#endif

	bool readHandshake();
	void readLine();
	char *dataptr;
	char databuffer[DATA_BUFFER_LEN];
	char sid[SID_LEN];
	char key[28];
	char *hostname;
	char *nsp;
	int port;

	void findColon(char which);
	void terminateCommand(void);
	bool waitForInput(void);
	void eatHeader(void);
};
