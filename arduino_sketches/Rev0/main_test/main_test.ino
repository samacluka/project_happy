
/*--------------------------------------- INCLUDES ---------------------------------------*/
#include <sensor.h>
#include <actuator.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>
#include "arduino_secrets.h"
#include <TimedAction.h>
#include <ArduinoJson.h>
/*--------------------------------------- DEFINES ---------------------------------------*/

#define DRY_SOIL_MOISTURE 798
#define WET_SOIL_MOISTURE 395
#define MIN_LIGHT_THRESHOLD 470
#define ALLOWED_PUMPING_TIME 5000

/*--------------------------------------- PROTOS ---------------------------------------*/

void sensorState();
void pump();
void httpPUT();
void httpGET();

/*--------------------------------------- GLOBALS ---------------------------------------*/

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;
int status = WL_IDLE_STATUS;

const int GMT = -5; //change this to adapt it to your time zone

/* Create an rtc object */
RTCZero rtc;

WiFiClient client;
char server[] = "encouragemint.herokuapp.com";
unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 10L * 1000L;

float temperature = 0;
float humidity = 0;
int dryness_of_soil = 0;
int is_water_present = 0;
int light_value = 0;
int pump_thread_active = 0;

long int pump_start_time;

sensor my_sensor(0);
actuator my_actuator(0);

char* tempString = "temperature=";
char* humString = "&humidity=";
char* moistString = "&soilMoisture=";
char* lightString = "&light=";
char* pumpString = "&pumpTime=";
char dataString[127];
char conlenString[30];

TimedAction sensorAction = TimedAction(1 * 1000, sensorState);
TimedAction pumpAction = TimedAction(500, pump);
TimedAction httpPUTAction = TimedAction(30 * 1000, httpPUT);
TimedAction httpGETAction = TimedAction(45 * 1000, httpGET);

/*----------------------------------------- MAIN -----------------------------------------*/

void setup()
{
    Serial.begin(9600);

    WiFiSetup();
    rtcSetEpoch();
    printrtc();
    pinMode(LED_BUILTIN, OUTPUT);

    my_sensor.init();
}

void loop()
{
    sensorAction.check();
    pumpAction.check();
    httpPUTAction.check();
    httpGETAction.check();

    /* Light Logic */
    if (light_value > MIN_LIGHT_THRESHOLD)
    {

    }
}


/* RTC Functions */

void rtcSetEpoch()
{
    unsigned long epoch;
    int numberOfTries = 0, maxTries = 6;
    do {
        epoch = WiFi.getTime();
        numberOfTries++;
    }
    while ((epoch == 0) && (numberOfTries < maxTries));

    if (numberOfTries == maxTries) {
        Serial.print("NTP unreachable!!");
        while (1);
    }
    else {
        Serial.print("Epoch received: ");
        Serial.println(epoch);
        rtc.setEpoch(epoch);

        Serial.println();
    }

    // Print date...
    Serial.print(rtc.getDay());
    Serial.print("/");
    Serial.print(rtc.getMonth());
    Serial.print("/");
    Serial.print(rtc.getYear());
    Serial.print("\t");

    // Print Time
    Serial.print(rtc.getHours() + GMT);
    Serial.print(":");
    Serial.print(rtc.getMinutes());
    Serial.print(":");
    Serial.print(rtc.getSeconds());

    Serial.println();
}


void printrtc()
{

}
/*-------------------------------------- Action --------------------------------------*/

void pump()
{
    //if the plant needs water and we have the water to do it
    if ((dryness_of_soil > DRY_SOIL_MOISTURE - ((DRY_SOIL_MOISTURE - WET_SOIL_MOISTURE) / 3)) && is_water_present && (pump_thread_active == 0))
    {

        Serial.println("Soil is dry, water is present. Enabling pump.");

        my_actuator.enablePump();
        pump_thread_active = 1;
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        pump_start_time = millis();
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    }
    else if (pump_thread_active)
    {
        my_sensor.getWaterLevel(is_water_present);
        my_sensor.getSoilMoist(dryness_of_soil);
        if (!is_water_present || !(dryness_of_soil > DRY_SOIL_MOISTURE - ((DRY_SOIL_MOISTURE - WET_SOIL_MOISTURE) / 3)))
        {
            my_actuator.disablePump();
            pump_thread_active = 0;
        }
        else if ( (millis() - pump_start_time) >  ALLOWED_PUMPING_TIME)
        {
            my_actuator.disablePump();
            pump_thread_active = 0;
        }

    }

}

void sensorState()
{
    my_sensor.getTempHum(temperature, humidity);
    my_sensor.getSoilMoist(dryness_of_soil);
    my_sensor.getWaterLevel(is_water_present);
    my_sensor.getHowMuchLight(light_value);


    //    Serial.print("temperature = ");
    //    Serial.println(temperature, DEC);
    //    Serial.print("humidity = ");
    //    Serial.println(humidity, DEC);
    //    Serial.print("soil moisture = ");
    //    Serial.println(dryness_of_soil, DEC);
    //    Serial.print("water present = ");
    //    Serial.println(is_water_present, DEC);
    //    Serial.print("light_value= ");
    //    Serial.println(light_value, DEC);
}

/*-------------------------------------- WEB COMM. --------------------------------------*/

void WiFiSetup() {
    WiFi.setPins(8, 7, 4, 2);
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        // don't continue:
        while (true);
    }

    // attempt to connect to WiFi network:
    while ( status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        delay(10000);
    }
    // you're connected now, so print out the status:
    printWiFiStatus();
}

void printWiFiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}


void httpPUT() {
    // close any connection before send a new request.
    // This will free the socket on the WiFi shield
    client.stop();

    // if there's a successful connection:
    int return_val = client.connect(server, 80);

    if (return_val) {
        int ret = sprintf(dataString, "%s%f%s%f%s%d%s%d%s%d", tempString, temperature, humString, humidity, moistString, dryness_of_soil, lightString, light_value, pumpString, 1);
        sprintf(conlenString, "Content-Length: %d", ret);

        Serial.println("PUT request sent");
        // send the HTTP PUT request:
        client.println("PUT /controller/setLogs HTTP/1.1");
        client.println("Host: encouragemint.herokuapp.com");
        client.println("User-Agent: ArduinoWiFi/1.1");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.println(conlenString);
        client.println("Connection: close");
        client.println();
        client.println(dataString);

        // note the time that the connection was made:
        lastConnectionTime = millis();
    }
    else {
        // if you couldn't make a connection:
        Serial.println("connection to server failed. Failed with error:");
        Serial.println(return_val);
    }
}

void httpGET() {
    // close any connection before send a new request.
    // This will free the socket on the WiFi shield
    client.stop();

    // if there's a successful connection:
    int return_val = client.connect(server, 80);

    if (return_val) {
        Serial.println("GET request sent");
        // send the HTTP GET request:
        client.println("GET /controller/getSetpoints HTTP/1.1");
        client.println("Host: encouragemint.herokuapp.com");
        //        client.println("User-Agent: ArduinoWiFi/1.1");
        //        client.println("Cache-Control: must-revalidate");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.println("Content-Length: 32");
        client.println("Connection: close");
        client.println();
        client.println("plantid=5e2071c27c213e47b9cb4142");

        // Check HTTP status
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
