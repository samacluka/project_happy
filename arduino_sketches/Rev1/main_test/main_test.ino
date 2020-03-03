
/*--------------------------------------- INCLUDES ---------------------------------------*/
#include <controller.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>
#include "arduino_secrets.h"
#include <TimedAction.h>
#include <ArduinoJson.h>
#include <math.h>

/*--------------------------------------- PROTOS ---------------------------------------*/

void sensorState();
void pump();
void light();
void httpPUT();
void httpGET();

/*--------------------------------------- GLOBALS ---------------------------------------*/

char* ssid = SECRET_SSID;
char* pass = SECRET_PASS;

controller my_controller;

TimedAction sensorAction = TimedAction(1 * 500, sensorState);
TimedAction pumpAction = TimedAction(500, pump);
TimedAction lightAction = TimedAction(1000, light);
TimedAction httpPUTAction = TimedAction(30 * 1000, httpPUT);
TimedAction httpGETAction = TimedAction(45 * 1000, httpGET);

/*----------------------------------------- MAIN -----------------------------------------*/

void setup()
{
    my_controller.init(SECRET_SSID, SECRET_PASS);
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    sensorAction.check();
    pumpAction.check();
    lightAction.check();
    httpPUTAction.check();
    httpGETAction.check();
}


/*-------------------------------------- Action --------------------------------------*/

void pump()
{
    my_controller.checkPump();
}

void light()
{
    my_controller.checkLights();
}


void sensorState()
{
    my_controller.my_sensor.poll();

    if (my_controller.my_sensor.getWaterLevel() == 0)
    {
        my_controller.waterIndicatorOn();
    }
    else
    {
        my_controller.waterIndicatorOff();
    }
    
        Serial.print("temperature = ");
        Serial.println(my_controller.my_sensor.getTemperature(), DEC);
        Serial.print("humidity = ");
        Serial.println(my_controller.my_sensor.getHumidity(), DEC);
        Serial.print("soil moisture = ");
        Serial.println(my_controller.my_sensor.getSoilMoist(), DEC);
        Serial.print("water present = ");
        Serial.println(my_controller.my_sensor.getWaterLevel(), DEC);
        Serial.print("light_value= ");
        Serial.println(my_controller.my_sensor.getLight(), DEC);
}

void httpPUT() {
    my_controller.my_communicator.sendToServer(my_controller.my_sensor.getTemperature(), my_controller.my_sensor.getHumidity(), my_controller.my_sensor.getSoilMoist(), my_controller.my_sensor.getWaterLevel(), my_controller.my_sensor.getLight());
}

void httpGET() {
  my_controller.my_communicator.getFromServer();
  my_controller.set_setpoints();
}
