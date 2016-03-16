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

//Uncomment the correct line

#ifdef W5100
#include <Ethernet.h>					//For W5100
#endif

#ifdef ENC28J60
#include <UIPEthernet.h>				//For ENC28J60
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>				//For ESP8266
#endif

#ifndef W5100 || ENC28J60 || ESP8266	//If no interface is defined
#error "Please specify an interface such as W5100, ENC28J60, or ESP8266"
#error "above your includes like so : #define <interface> "
#endif

//#include "SPI.h"
// Length of static data buffers
#define DATA_BUFFER_LEN 120
#define SID_LEN 24

class SocketIOClient {
public:
	typedef void(*DataArrivedDelegate)(SocketIOClient client, char *data);
	bool connect(char hostname[], int port = 80);
	bool connectHTTP(char hostname[], int port = 80);
	bool connected();
	void disconnect();
	bool reconnect(char hostname[], int port = 80);
	bool monitor();
	void setDataArrivedDelegate(DataArrivedDelegate dataArrivedDelegate);
	void send(String RID, String Rname, String Rcontent);
	void sendJSON(String RID, String JSON);
	void heartbeat(int select);
	void getREST(String request);
private:
	void parser(int index);
	void sendHandshake(char hostname[]);
	//EthernetClient client;				//For ENC28J60 or W5100
	WiFiClient client;						//For ESP8266
	DataArrivedDelegate dataArrivedDelegate;
	bool readHandshake();
	void readLine();
	char *dataptr;
	char databuffer[DATA_BUFFER_LEN];
	char sid[SID_LEN];
	char key[28];
	char *hostname;
	int port;

	void findColon(char which);
	void terminateCommand(void);
	bool waitForInput(void);
	void eatHeader(void);
};
