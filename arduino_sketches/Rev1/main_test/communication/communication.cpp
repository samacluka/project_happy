#include <communication.h>


communication::communication()
{
	status  = WL_IDLE_STATUS;

}

void communication::setup(char* ssid_in, char* password_in)
{
	ssid = ssid_in;
	password = password_in;

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

void communication::setServerID(char* serverID) {
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

void communication::sendToServer(float temperature, float humidity, int moisture, int water, int light)
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

	// Check that WiFi is still connected
	if (status != WL_CONNECTED) {
		setup(ssid, password);
	}

	client.stop();

	int return_val = client.connect(server, 80);
	int content_length;
	if (return_val) {
		if(isnan(temperature) || isnan(humidity)) {
			content_length = sprintf(dataString, "%s%s%s%f%s%f%s%d%s%d%s%d", plantIDString, plantID, tempString, 0., humString, 0., moistString, moisture, lightString, light, pumpString, water);
		}
		else {
			content_length = sprintf(dataString, "%s%s%s%f%s%f%s%d%s%d%s%d", plantIDString, plantID, tempString, temperature, humString, humidity, moistString, moisture, lightString, light, pumpString, water);
		}
		sprintf(conlenString, "Content-Length: %d", content_length);
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

void communication::getFromServer() {
	char hostString[50];

	// Check that WiFi is still connected
	if (status != WL_CONNECTED) {
		setup(ssid, password);
	}

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
	moisture_setpoint_max = doc["soilMoisture"]["max"].as<int>();
	moisture_setpoint_min = doc["soilMoisture"]["min"].as<int>();
	light_setpoint_max = doc["lightHours"]["max"].as<int>();
	light_setpoint_min = doc["lightHours"]["min"].as<int>();

        // Disconnect
        client.stop();

    }
    else {
        // if you couldn't make a connection:
        Serial.println("connection to server failed. Failed with error:");
        Serial.println(ret_val);
    }
}

void communication::getPlantId() {
	char hostString[50];

	// Check that WiFi is still connected
	if (status != WL_CONNECTED) {
		setup(ssid, password);
	}

	client.stop();

	sprintf(hostString, "Host: %s", server);

	int ret_val =  client.connect(server, 80);

	if (ret_val) {
		client.println("GET /config/new HTTP/1.1");
		client.println(hostString);
		client.println("Connection: close");
		client.println();

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
        const size_t capacity = JSON_OBJECT_SIZE(1) + 128;
        DynamicJsonDocument doc(capacity);

        // Parse JSON object
        DeserializationError error = deserializeJson(doc, client);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return;
        }


        // Extract values
	plantid = doc["id"].as<char*>();

        // Disconnect
        client.stop();

    }
    else {
        // if you couldn't make a connection:
        Serial.println("connection to server failed. Failed with error:");
        Serial.println(ret_val);
    }
}

int communication::getMoistureMax()
{
	return moisture_setpoint_max;
}

int communication::getMoistureMin()
{
	return moisture_setpoint_min;
}

int communication::getLightMax()
{
	return light_setpoint_max;
}

int communication::getLightMin()
{
	return light_setpoint_min;
}
