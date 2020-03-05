#ifndef communication_h
#define communication_h

#include <WiFi101.h>
#include <ArduinoJson.h>
#include <math.h>

#define WATERING_MESSAGE 0
#define LIGHTS_ON_MESSAGE 1
#define LIGHTS_OFF_MESSAGE 2
#define WATER_EMPTY_MESSAGE 3

#define PRIMARY 0
#define SECONDARY 1
#define SUCCESS 2
#define DANGER 3
#define WARNING 4
#define INFO 5
#define LIGHT 6
#define DARK 7

class communication
{
private:
	WiFiClient client;
	char* server;
	int status;
	char* ssid;
	char* password;
	const char* plantid;
	int moisture_setpoint_max;
	int moisture_setpoint_min;
	int light_setpoint_max;
	int light_setpoint_min;
    const char* message[] = {"Plant is being watered.", "Plant lights have been turned on.", "Plant lights have been turned off.", "Water reservoir is empty."};
    const char* message_type[] = {"primary","secondary","success","danger","warning","info","light","dark"};

public:
	communication();
	void setup(char* ssid, char* password);
	void setServerID(char*  serverID);
	void printWiFiStatus();
	void sendToServer(float temperature, float humidity, int moisture, int water, int light);
    void sendToServer(char* message, char* type);
	void getFromServer();
	void getPlantId();
	int getMoistureMax();
	int getMoistureMin();
	int getLightMax();
	int getLightMin();
};
#endif
