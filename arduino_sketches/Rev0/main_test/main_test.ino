#include <TimedAction.h>

/*--------------------------------------- INCLUDES ---------------------------------------*/
#include <sensor.h>
#include <actuator.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>
#include "arduino_secrets.h"

/*--------------------------------------- DEFINES ---------------------------------------*/

#define DRY_SOIL_MOISTURE 798
#define WET_SOIL_MOISTURE 395

#define MIN_LIGHT_THRESHOLD 470

#define ALLOWED_PUMPING_TIME 5000

/*--------------------------------------- PROTOS ---------------------------------------*/

void pump();

/*--------------------------------------- GLOBALS ---------------------------------------*/

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;
int status = WL_IDLE_STATUS;

const int GMT = -5; //change this to adapt it to your time zone

/* Create an rtc object */
RTCZero rtc;

//WiFiClient client;
//char server[] = "projecthappy.herokuapp.com";
//unsigned long lastConnectionTime = 0;
//const unsigned long postingInterval = 10L * 1000L;

float temperature = 0;
float humidity = 0;
int dryness_of_soil = 0;
int is_water_present = 0;
int light_value = 0;
int pump_thread_active = 0;

sensor my_sensor(0);
actuator my_actuator(0);

TimedAction pumpAction = TimedAction(500, pump);

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
    /* Sense System State */

    my_sensor.getTempHum(temperature, humidity);
    my_sensor.getSoilMoist(dryness_of_soil);
    my_sensor.getWaterLevel(is_water_present);
    my_sensor.getHowMuchLight(light_value);

    pumpAction.check();

    Serial.print("temperature = ");
    Serial.println(temperature, DEC);
    Serial.print("humidity = ");
    Serial.println(humidity, DEC);
    Serial.print("soil moisture = ");
    Serial.println(dryness_of_soil, DEC);
    Serial.print("water present = ");
    Serial.println(is_water_present, DEC);
    Serial.print("light_value= ");
    Serial.println(light_value, DEC);


    /* Decide upon Actuation */

    /* Pump Logic */



    /* Light Logic */
    if (light_value > MIN_LIGHT_THRESHOLD)
    {

    }


    delay(5000);
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
}


void printrtc()
{
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
/*-------------------------------------- Action --------------------------------------*/

void pump()
{
    static long int pump_start_time;
    
    
    //if the plant needs water and we have the water to do it
    if (dryness_of_soil > DRY_SOIL_MOISTURE - ((DRY_SOIL_MOISTURE - WET_SOIL_MOISTURE) / 3) && is_water_present && !pump_thread_active)
    {
        Serial.println("Soil is dry, water is present. Enabling pump.");

        my_actuator.enablePump();
        pump_thread_active = 1;
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        
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

/*
    void httpSend() {
    // close any connection before send a new request.
    // This will free the socket on the WiFi shield
    client.stop();

    // if there's a successful connection:
    int return_val = client.connect(server,80);

    if (return_val) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("POST /postRoute HTTP/1.1");
    client.println("Host: projecthappy.herokuapp.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Content-Type: application/x-www-form-urlencoded");
    //    client.println("Content-Type: text-plain");
    client.println("Content-Length: 21");
    client.println("Connection: close");
    client.println();
    client.println("field1=0&field2=hello");


    //client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
    }
    else {
    // if you couldn't make a connection:
    Serial.println("connection to server failed. Failed with error:");
    Serial.println(return_val);
    }
    }

    void httpReceive() {
    // close any connection before send a new request.
    // This will free the socket on the WiFi shield
    client.stop();

    // if there's a successful connection:
    int return_val = client.connect(server,80);

    if (return_val) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /test HTTP/1.1");
    client.println("Host: projecthappy.herokuapp.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Cache-Control: must-revalidate");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
    }
    else {
    // if you couldn't make a connection:
    Serial.println("connection to server failed. Failed with error:");
    Serial.println(return_val);
    }
    }

*/
