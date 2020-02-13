#include <communication.h>


communication::communication() 
{
	status  = WL_IDLE_STATUS;
}

void communication::setup(char* ssid, char* password)
{
	WiFi.setPins(8,7,4,2);
	if (WiFi.status() == WL_NO_SHIELD) {
		Serial.println("WiFi shield not present");
		while(true);
	}

	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		status  = WiFi.begin(ssid, password);

		delay(1000);
	}

	printWiFiStatus();
}

void setServerID(char* serverID) {
	server = serverID;
}

void communication::printWiFiStatus() 
{
	Serial.println("SSID: ");
	Serial.println(WiFi.SSID());
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI): ");
	Serial.print(rssi);
	Serial.println(" dBm");
}

void sendToServer(float temperature, float humidity, int moisture, int water, int light)
{
	char* plantIDString = "plantid=";
	char* tempString = "&temperature=";
	char* humString = "&humidity=";
	char* moistString = "&soilMoisture=";
	char* lightString = "&light=";
	char* pumpString = "&pumpTime=";
	char* plantID = "5e2071c27c213e47b9cb4142";
	char dataString[127];
	char conlenString[30];
	char hostString[50];

	client.stop();

	int return_val = client.connect(server, 80);

	if (return_val) {
		if(isnan(my_sensor.getTemperature()) || isnan(my_sensor.getHumidity())) {
			int ret = sprintf(dataString, "%s%s%s%f%s%f%s%d%s%d%s%d", plantIDString, plantID, tempString, 0., humString, 0., moistString, moisture, lightString, light, pumpString, water);
		}
		else {
			int ret = sprintf(dataString, "%s%s%s%f%s%f%s%d%s%d%s%d", plantIDString, plantID, tempString, temperature, humString, humidity, moistString, moisture, lightString, light, pumpString, water);
		}
		sprintf(conlenString, "Content-Length: %d", ret);
		sprintf(hostString, "Host: %s", server);

		client.println("PUT /controller/setLogs HTTP/1.1");
		client.println(hostString);
		client.println("User-Agent: ArduinoWiFi/1.1");
		client.println("Content-Type: application/x-www-form-urlencoded");
		client.println(conlenString);
		client.println("Connection: close");
		client.println();
		client.println(dataString);

		char status[32] = {0};
        client.readBytesUntil('\r', status, sizeof(status));
        // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
        if (strcmp(status + 9, "200 OK") != 0) {
            Serial.print(F("Unexpected response: "));
            Serial.println(status);
            return;
        }
	}
	else {
		Serial.println("connection to server failed. Failed with error:");
        Serial.println(return_val);
	}
}

void getFromServer() {
	char hostString[50];

	client.stop();

	sprintf(hostString, "Host: %s", server);

	int ret_val =  client.connect(server, 80);

	if (ret_val) {
		client.println("GET /controller/getSetpoints HTTP/1.1");
		client.println(hostString);
		client.println("Content-Type: application/x-www-form-urlencoded");
		client.println("Content-Length: 32");
		client.println("Connection: close");
		client.println();
        client.println("plantid=5e2071c27c213e47b9cb4142");

        char status[32] = {0};
        client.readBytesUntil('\r', status, sizeof(status));
        // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
        if (strcmp(status + 9, "200 OK") != 0) {
            Serial.print(F("Unexpected response: "));
            Serial.println(status);
            return;
        }

        // Skip HTTP headers
        char endOfHeaders[] = "\r\n\r\n";
        if (!client.find(endOfHeaders)) {
            Serial.println(F("Invalid response"));
            return;
        }

        // Allocate the JSON document
        // Use arduinojson.org/v6/assistant to compute the capacity.
        const size_t capacity = 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(6) + 512;
        DynamicJsonDocument doc(capacity);

        // Parse JSON object
        DeserializationError error = deserializeJson(doc, client);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return;
        }


        // Extract values
        Serial.println(F("Response:"));
        Serial.println(doc["Name"].as<char*>());
        Serial.println(doc["Type"].as<char*>());
        Serial.println(doc["soilMoisture"]["max"].as<float>(), 2);
        Serial.println(doc["soilMoisture"]["min"].as<float>(), 2);
        Serial.println(doc["lightThreshold"]["max"].as<float>(), 2);
        Serial.println(doc["lightThreshold"]["min"].as<float>(), 2);

        // Disconnect
        client.stop();

        // note the time that the connection was made:
        lastConnectionTime = millis();
    }
    else {
        // if you couldn't make a connection:
        Serial.println("connection to server failed. Failed with error:");
        Serial.println(return_val);
    }
}










