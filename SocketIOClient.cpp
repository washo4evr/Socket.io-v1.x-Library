/*
socket.io-arduino-client: a Socket.IO client for the Arduino
Based on the Kevin Rohling WebSocketClient & Bill Roy Socket.io Lbrary
Copyright 2015 Florent Vidal
Supports Socket.io v1.x
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:
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
#define ESP32
#include <SocketIOClient.h>


String tmpdata = "";	//External variables
String RID = "";
String Rname = "";
String Rcontent = "";



bool SocketIOClient::connect(char thehostname[], int theport) {
	if (!client.connect(thehostname, theport)) return false;
	hostname = thehostname;
	port = theport;
	sendHandshake(hostname);
	return readHandshake();
}

bool SocketIOClient::connectHTTP(char thehostname[], int theport) {
	if (!client.connect(thehostname, theport)) return false;
	hostname = thehostname;
	port = theport;
	return true;
}

bool SocketIOClient::reconnect(char thehostname[], int theport)
{
	if (!client.connect(thehostname, theport)) return false;
	hostname = thehostname;
	port = theport;
	sendHandshake(hostname);
	return readHandshake();
}

bool SocketIOClient::connected() {
	return client.connected();
}

void SocketIOClient::disconnect() {
	client.stop();
}

// find the nth colon starting from dataptr
void SocketIOClient::findColon(char which) {
	while (*dataptr) {
		if (*dataptr == ':') {
			if (--which <= 0) return;
		}
		++dataptr;
	}
}

// terminate command at dataptr at closing double quote
void SocketIOClient::terminateCommand(void) {
	dataptr[strlen(dataptr) - 3] = 0;
}

void SocketIOClient::parser(int index) {
	String rcvdmsg = "";
	int sizemsg = databuffer[index + 1];   // 0-125 byte, index ok        Fix provide by Galilei11. Thanks
	if (databuffer[index + 1]>125)
	{
		sizemsg = databuffer[index + 2];    // 126-255 byte
		index += 1;       // index correction to start
	}
	Serial.print("Message size = ");	//Can be used for debugging
	Serial.println(sizemsg);			//Can be used for debugging
	for (int i = index + 2; i < index + sizemsg + 2; i++)
		rcvdmsg += (char)databuffer[i];
	Serial.print("Received message = ");	//Can be used for debugging
	Serial.println(rcvdmsg);				//Can be used for debugging
	switch (rcvdmsg[0])
	{
	case '2':
		Serial.println("Ping received - Sending Pong");
		heartbeat(1);
		break;

	case '3':
		Serial.println("Pong received - All good");
		break;

	case '4':
		switch (rcvdmsg[1])
		{
		case '0':
			Serial.println("Upgrade to WebSocket confirmed");
			break;
		case '2':
			RID = rcvdmsg.substring(4, rcvdmsg.indexOf("\","));
			Rname = rcvdmsg.substring(rcvdmsg.indexOf("\",") + 4, rcvdmsg.indexOf("\":"));
			Rcontent = rcvdmsg.substring(rcvdmsg.indexOf("\":") + 3, rcvdmsg.indexOf("\"}"));
			//Serial.println("RID = " + RID);
			//Serial.println("Rname = " + Rname);
			//Serial.println("Rcontent = " + Rcontent);
			Serial.println(rcvdmsg);
			break;
		}
	}
}

bool SocketIOClient::monitor() {
	int index = -1;
	int index2 = -1;
	String tmp = "";
	*databuffer = 0;

	if (!client.connected()) {
		if (!client.connect(hostname, port)) return 0;
	}

	if (!client.available())
	{
		return 0;
	}
	char which;

	while (client.available()) {
		readLine();
		tmp = databuffer;
		Serial.println(databuffer);
		dataptr = databuffer;
		index = tmp.indexOf((char)129);	//129 DEC = 0x81 HEX = sent for proper communication
		index2 = tmp.indexOf((char)129, index + 1);
		/*Serial.print("Index = ");			//Can be used for debugging
		Serial.print(index);
		Serial.print(" & Index2 = ");
		Serial.println(index2);*/
		if (index != -1)
		{
			parser(index);
		}
		if (index2 != -1)
		{
			parser(index2);
		}
	}
}

void SocketIOClient::sendHandshake(char hostname[]) {
	client.println(F("GET /socket.io/1/?transport=polling&b64=true HTTP/1.1"));
	client.print(F("Host: "));
	client.println(hostname);
	client.println(F("Origin: Arduino\r\n"));
}

bool SocketIOClient::waitForInput(void) {
	unsigned long now = millis();
	while (!client.available() && ((millis() - now) < 30000UL)) { ; }
	return client.available();
}

void SocketIOClient::eatHeader(void) {
	while (client.available()) {	// consume the header
		readLine();
		if (strlen(databuffer) == 0) break;
	}
}

bool SocketIOClient::readHandshake() {

	if (!waitForInput()) return false;

	// check for happy "HTTP/1.1 200" response
	readLine();
	if (atoi(&databuffer[9]) != 200) {
		while (client.available()) readLine();
		client.stop();
		return false;
	}
	eatHeader();
	readLine();
	String tmp = databuffer;

	int sidindex = tmp.indexOf("sid");
	int sidendindex = tmp.indexOf("\"", sidindex + 6);
	int count = sidendindex - sidindex - 6;

	for (int i = 0; i < count; i++)
	{
		sid[i] = databuffer[i + sidindex + 6];
	}
	Serial.println(" ");
	Serial.print(F("Connected. SID="));
	Serial.println(sid);	// sid:transport:timeout 

	while (client.available()) readLine();
	client.stop();
	delay(1000);

	// reconnect on websocket connection
	if (!client.connect(hostname, port)) {
		Serial.print(F("Websocket failed."));
		return false;
	}
	Serial.println(F("Connecting via Websocket"));

	client.print(F("GET /socket.io/1/websocket/?transport=websocket&b64=true&sid="));
	client.print(sid);
	client.print(F(" HTTP/1.1\r\n"));

	client.print(F("Host: "));
	client.print(hostname);
	client.print("\r\n");
	client.print(F("Sec-WebSocket-Version: 13\r\n"));
	client.print(F("Origin: ArduinoSocketIOClient\r\n"));
	client.print(F("Sec-WebSocket-Extensions: permessage-deflate\r\n"));
	client.print(F("Sec-WebSocket-Key: IAMVERYEXCITEDESP32FTW==\r\n")); // can be anything

	client.print(F("Cookie: io="));
	client.print(sid);
	client.print("\r\n");
	
	client.print(F("Connection: Upgrade\r\n"));

	client.println(F("Upgrade: websocket\r\n"));	// socket.io v2.0.3 supported




	if (!waitForInput()) return false;
	readLine();
	if (atoi(&databuffer[9]) != 101) {	// check for "HTTP/1.1 101 response, means Updrage to Websocket OK
		while (client.available()) readLine();
		client.stop();
		return false;
	}
	readLine();
	readLine();
	readLine();
	for (int i = 0; i < 28; i++)
	{
		key[i] = databuffer[i + 22];	//key contains the Sec-WebSocket-Accept, could be used for verification
	}


	eatHeader();


	/*
	Generating a 32 bits mask requiered for newer version
	Client has to send "52" for the upgrade to websocket
	*/
	randomSeed(analogRead(0));
	String mask = "";
	String masked = "52";
	String message = "52";
	for (int i = 0; i < 4; i++)	//generate a random mask, 4 bytes, ASCII 0 to 9
	{
		char a = random(48, 57);
		mask += a;
	}

	for (int i = 0; i < message.length(); i++)
		masked[i] = message[i] ^ mask[i % 4];	//apply the "mask" to the message ("52")



	client.print((char)0x81);	//has to be sent for proper communication
	client.print((char)130);	//size of the message (2) + 128 because message has to be masked
	client.print(mask);
	client.print(masked);

	monitor();		// treat the response as input
	return true;
}

void SocketIOClient::getREST(String path)
{
	String message = "GET /" + path + "/ HTTP/1.1";
	client.println(message);
	client.print(F("Host: "));
	client.println(hostname);
	client.println(F("Origin: Arduino"));
	client.println(F("Connection: close\r\n"));
}

void SocketIOClient::postREST(String path, String type, String data)
{
	String message = "POST /" + path + "/ HTTP/1.1";
	client.println(message);
	client.print(F("Host: "));
	client.println(hostname);
	client.println(F("Origin: Arduino"));
	client.println(F("Connection: close\r\n"));
	client.print(F("Content-Length: "));
	client.println(data.length());
	client.print(F("Content-Type: "));
	client.println(type);
	client.println("\r\n");
	client.println(data);

}

void SocketIOClient::putREST(String path, String type, String data)
{
	String message = "PUT /" + path + "/ HTTP/1.1";
	client.println(message);
	client.print(F("Host: "));
	client.println(hostname);
	client.println(F("Origin: Arduino"));
	client.println(F("Connection: close\r\n"));
	client.print(F("Content-Length: "));
	client.println(data.length());
	client.print(F("Content-Type: "));
	client.println(type);
	client.println("\r\n");
	client.println(data);
}

void SocketIOClient::deleteREST(String path)
{
	String message = "DELETE /" + path + "/ HTTP/1.1";
	client.println(message);
	client.print(F("Host: "));
	client.println(hostname);
	client.println(F("Origin: Arduino"));
	client.println(F("Connection: close\r\n"));
}

void SocketIOClient::readLine() {
	for (int i = 0; i < DATA_BUFFER_LEN; i++)
		databuffer[i] = ' ';
	dataptr = databuffer;
	while (client.available() && (dataptr < &databuffer[DATA_BUFFER_LEN - 2]))
	{
		char c = client.read();
		Serial.print(c);			//Can be used for debugging
		if (c == 0) Serial.print("");
		else if (c == 255) Serial.println("");
		else if (c == '\r') { ; }
		else if (c == '\n') break;
		else *dataptr++ = c;
	}
	*dataptr = 0;
}

void SocketIOClient::send(String RID, String Rname, String Rcontent) {

	String message = "42[\"" + RID + "\",{\"" + Rname + "\":\"" + Rcontent + "\"}]";
	int header[10];
	header[0] = 0x81;
	int msglength = message.length();
	randomSeed(analogRead(0));
	String mask = "";
	String masked = message;
	for (int i = 0; i < 4; i++)
	{
		char a = random(48, 57);
		mask += a;
	}
	for (int i = 0; i < msglength; i++)
		masked[i] = message[i] ^ mask[i % 4];

	client.print((char)header[0]);	//has to be sent for proper communication
									//Depending on the size of the message
	if (msglength <= 125)
	{
		header[1] = msglength + 128;
		client.print((char)header[1]);	//size of the message + 128 because message has to be masked
	}
	else if (msglength >= 126 && msglength <= 65535)
	{
		header[1] = 126 + 128;
		client.print((char)header[1]);
		header[2] = (msglength >> 8) & 255;
		client.print((char)header[2]);
		header[3] = (msglength)& 255;
		client.print((char)header[3]);
	}
	else
	{
		header[1] = 127 + 128;
		client.print((char)header[1]);
		header[2] = (msglength >> 56) & 255;
		client.print((char)header[2]);
		header[3] = (msglength >> 48) & 255;
		client.print((char)header[4]);
		header[4] = (msglength >> 40) & 255;
		client.print((char)header[4]);
		header[5] = (msglength >> 32) & 255;
		client.print((char)header[5]);
		header[6] = (msglength >> 24) & 255;
		client.print((char)header[6]);
		header[7] = (msglength >> 16) & 255;
		client.print((char)header[7]);
		header[8] = (msglength >> 8) & 255;
		client.print((char)header[8]);
		header[9] = (msglength)& 255;
		client.print((char)header[9]);
	}

	client.print(mask);
	client.print(masked);
}

void SocketIOClient::sendJSON(String RID, String JSON) {
	String message = "42[\"" + RID + "\"," + JSON + "]";
	int header[10];
	header[0] = 0x81;
	int msglength = message.length();
	randomSeed(analogRead(0));
	String mask = "";
	String masked = message;
	for (int i = 0; i < 4; i++)
	{
		char a = random(48, 57);
		mask += a;
	}
	for (int i = 0; i < msglength; i++)
		masked[i] = message[i] ^ mask[i % 4];

	client.print((char)header[0]);	//has to be sent for proper communication
									//Depending on the size of the message
	if (msglength <= 125)
	{
		header[1] = msglength + 128;
		client.print((char)header[1]);	//size of the message + 128 because message has to be masked
	}
	else if (msglength >= 126 && msglength <= 65535)
	{
		header[1] = 126 + 128;
		client.print((char)header[1]);
		header[2] = (msglength >> 8) & 255;
		client.print((char)header[2]);
		header[3] = (msglength)& 255;
		client.print((char)header[3]);
	}
	else
	{
		header[1] = 127 + 128;
		client.print((char)header[1]);
		header[2] = (msglength >> 56) & 255;
		client.print((char)header[2]);
		header[3] = (msglength >> 48) & 255;
		client.print((char)header[4]);
		header[4] = (msglength >> 40) & 255;
		client.print((char)header[4]);
		header[5] = (msglength >> 32) & 255;
		client.print((char)header[5]);
		header[6] = (msglength >> 24) & 255;
		client.print((char)header[6]);
		header[7] = (msglength >> 16) & 255;
		client.print((char)header[7]);
		header[8] = (msglength >> 8) & 255;
		client.print((char)header[8]);
		header[9] = (msglength)& 255;
		client.print((char)header[9]);
	}

	client.print(mask);
	client.print(masked);
}

void SocketIOClient::heartbeat(int select) {
	randomSeed(analogRead(0));
	String mask = "";
	String masked = "";
	String message = "";
	if (select == 0)
	{
		masked = "2";
		message = "2";
	}
	else
	{
		masked = "3";
		message = "3";
	}
	for (int i = 0; i < 4; i++)	//generate a random mask, 4 bytes, ASCII 0 to 9
	{
		char a = random(48, 57);
		mask += a;
	}

	for (int i = 0; i < message.length(); i++)
		masked[i] = message[i] ^ mask[i % 4];	//apply the "mask" to the message ("2" : ping or "3" : pong)



	client.print((char)0x81);	//has to be sent for proper communication
	client.print((char)129);	//size of the message (1) + 128 because message has to be masked
	client.print(mask);
	client.print(masked);
}
