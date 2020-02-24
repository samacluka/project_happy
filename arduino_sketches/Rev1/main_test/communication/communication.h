#ifndef communication_h
#define communication_h

#include <WiFi101.h>
#include <ArduinoJson.h>
#include <math.h>

class communication
{
private:
	WiFiClient client;
	char* server;
	int status;

public:
	communication();
	void setup(char* ssid, char* password);
	void setServerID(char*  serverID);
	void printWiFiStatus();
	void sendToServer(float temperature, float humidity, int moisture, int water, int light);
	void getFromServer();
};
#endif