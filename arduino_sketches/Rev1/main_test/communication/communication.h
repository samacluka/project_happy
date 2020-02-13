#ifndef communication_h
#define communication_h

class communication
{
private:
	WiFiClient client;
	char* server;
	int status;

public:
	communication();
	~communication();
	void setup(char* ssid, char* password);
	void setServerID(char*  serverID);
	void printWiFiStatus();
	void sendToServer();
	void getFromServer();
};