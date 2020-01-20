/*--------------------------------------- INCLUDES ---------------------------------------*/
#include <sensor.h>
#include <actuator.h>
#include <RTCZero.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>
#include "arduino_secrets.h"

/*--------------------------------------- DEFINES ---------------------------------------*/

#define DRY_SOIL_MOISTURE 798
#define WET_SOIL_MOISTURE 395

/*--------------------------------------- GLOBALS ---------------------------------------*/

//char ssid[] = SECRET_SSID;
//char pass[] = SECRET_PASS;
//int keyIndex = 0;
//int status = WL_IDLE_STATUS;
//WiFiClient client;
//char server[] = "projecthappy.herokuapp.com";
//unsigned long lastConnectionTime = 0;
//const unsigned long postingInterval = 10L * 1000L;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                           // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

const int GMT = -5; //change this to adapt it to your time zone

/* Create an rtc object */
RTCZero rtc;

/*----------------------------------------- MAIN -----------------------------------------*/

void setup()
{
  //    WiFiSetup();
  WiFi.setPins(8, 7, 4, 2);
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  rtc.begin(); // initialize RTC

  // check if the WiFi module works
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

void loop()
{
  float temperature;
  float humidity;
  int dryness_of_soil;
  int is_water_present;
  int light_value;

  sensor my_sensor(0);
  actuator my_actuator(0);

  Serial.print("Unix time = ");
  Serial.println(rtc.getEpoch());


  // Print date...
  Serial.print(rtc.getDay());
  Serial.print("/");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  Serial.print(rtc.getYear());
  Serial.print("\t");


  Serial.print(rtc.getHours()+ GMT);
  Serial.print(":");
  Serial.print(rtc.getMinutes());
  Serial.print(":");
  Serial.print(rtc.getSeconds());

  Serial.println();


  //    digitalWrite(LED_BUILTIN, HIGH);

  my_sensor.init();

  my_sensor.getHowMuchLight(light_value);

  Serial.print("light_value= ");
  Serial.println(light_value, DEC);

  delay(500);
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

/*-------------------------------------- WEB COMM. --------------------------------------*/
/*
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
*/
