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
	char* ssid;
	char* password;
	char* plantid;
	int moisture_setpoint_max;
	int moisture_setpoint_min;
	int light_setpoint_max;
	int light_setpoint_min;

public:
	communication();
	void setup(char* ssid, char* password);
	void setServerID(char*  serverID);
	void printWiFiStatus();
	void sendToServer(float temperature, float humidity, int moisture, int water, int light);
	void getFromServer();
	void getPlantId();
	int getMoistureMax();
	int getMoistureMin();
	int getLightMax();
	int getLightMin();
};
#endif
