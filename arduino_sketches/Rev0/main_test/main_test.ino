/*--------------------------------------- INCLUDES ---------------------------------------*/
#include <sensor.h>
#include <actuator.h>
//#include <SPI.h>
//#include <WiFi101.h>

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
volatile boolean water_ISR_flag = true;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

/*----------------------------------------- MAIN -----------------------------------------*/

//void waterISR()
//{
//  if (lastDebounceTime == 0)
//  {
//    lastDebounceTime = millis();
//    if (water_ISR_flag)
//    {
//      water_ISR_flag = false;
//    }
//    else
//    {
//      water_ISR_flag = true;
//    }
//  }
//
//  if ((millis() - lastDebounceTime) > debounceDelay)
//  {
//    if (water_ISR_flag)
//    {
//      water_ISR_flag = false;
//    }
//    else
//    {
//      water_ISR_flag = true;
//    }
//    lastDebounceTime = millis();
//  }
//
//
//  return;
//  // digitalWrite(LED_BUILTIN,HIGH);
//}

void setup()
{
  //    WiFiSetup();
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  //  attachInterrupt(digitalPinToInterrupt(WATER_LEVEL_PIN), waterISR, RISING);
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

  //    digitalWrite(LED_BUILTIN, HIGH);

  my_sensor.init();

  my_sensor.getTempHum(temperature, humidity);
  my_sensor.getSoilMoist(dryness_of_soil);
  my_sensor.getWaterLevel(is_water_present);
  my_sensor.getHowMuchLight(light_value);

  //    Serial.print("temperature = ");
  //    Serial.println(temperature, DEC);
  //    Serial.print("humidity = ");
  //    Serial.println(humidity, DEC);
  Serial.print("soil moisture = ");
  Serial.println(dryness_of_soil, DEC);
  Serial.print("water present = ");
  Serial.println(is_water_present, DEC);
  //    Serial.print("light_value= ");
  //    Serial.println(light_value, DEC);
  //  Serial.print("water ISR flag = ");
  //  Serial.println(water_ISR_flag);

  // Logic for turning on pump if the moisture detector is low
  if (dryness_of_soil > DRY_SOIL_MOISTURE - ((DRY_SOIL_MOISTURE - WET_SOIL_MOISTURE) / 3) && is_water_present)
  {
    Serial.println("Soil is dry, water is present. Enabling pump.");

    my_actuator.enablePump();
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

    do
    {
      Serial.println("I am pumping.");
      delay(1000);
      my_sensor.getWaterLevel(is_water_present);
      my_sensor.getSoilMoist(dryness_of_soil);

    } while (is_water_present && (dryness_of_soil > DRY_SOIL_MOISTURE - ((DRY_SOIL_MOISTURE - WET_SOIL_MOISTURE) / 3)));

    water_ISR_flag = false;
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    my_actuator.disablePump();
    delay(1000);
  }

  //if(water_ISR_flag)  {
  //    digitalWrite(LED_BUILTIN,LOW);
  //
  //   water_ISR_flag =  0;
  //}

  delay(2000);
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
