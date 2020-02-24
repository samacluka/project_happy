
/*--------------------------------------- INCLUDES ---------------------------------------*/
#include <sensor.h>
#include <actuator.h>
#include <communication.h>
#include <controller.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>
#include "arduino_secrets.h"
#include <TimedAction.h>
#include <ArduinoJson.h>
#include <math.h>
/*--------------------------------------- DEFINES ---------------------------------------*/

#define DRY_SOIL_MOISTURE 798
#define WET_SOIL_MOISTURE 395
#define MIN_LIGHT_THRESHOLD 470
#define ALLOWED_PUMPING_TIME 5000
#define ALLOWED_LED_TIME 20000

/*--------------------------------------- PROTOS ---------------------------------------*/

void sensorState();
void pump();
void light();
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
unsigned int epoch;

WiFiClient client;
char server[] = "encouragemint.herokuapp.com";
unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 10L * 1000L;

int pump_thread_active = 0;
int light_thread_active = 0;
int minutes_of_light = 0;
int total_minutes_of_light_today = 0;
int light_readings_recorded = 0;

long int pump_start_time;
long int light_start_time;

controller my_controller;

TimedAction sensorAction = TimedAction(1 * 500, sensorState);
TimedAction pumpAction = TimedAction(500, pump);
TimedAction lightAction = TimedAction(1000, light);
TimedAction httpPUTAction = TimedAction(30 * 1000, httpPUT);
TimedAction httpGETAction = TimedAction(45 * 1000, httpGET);

/*----------------------------------------- MAIN -----------------------------------------*/

void setup()
{
    my_controller.init("iPhone", "AbbyRocks");
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
    //if the plant needs water and we have the water to do it
    if ((my_controller.my_sensor.getSoilMoist() > DRY_SOIL_MOISTURE - ((DRY_SOIL_MOISTURE - WET_SOIL_MOISTURE) / 3)) && my_controller.my_sensor.getWaterLevel() && (pump_thread_active == 0))
    {

        Serial.println("Soil is dry, water is present. Enabling pump.");

        my_controller.my_actuator.enablePump();
        pump_thread_active = 1;
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        pump_start_time = millis();
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    }
    else if (pump_thread_active)
    {
        my_controller.my_sensor.poll();
        if (!my_controller.my_sensor.getWaterLevel() || !(my_controller.my_sensor.getSoilMoist() > DRY_SOIL_MOISTURE - ((DRY_SOIL_MOISTURE - WET_SOIL_MOISTURE) / 3)))
        {
            my_controller.my_actuator.disablePump();
            pump_thread_active = 0;
        }
        else if ( (millis() - pump_start_time) >  ALLOWED_PUMPING_TIME)
        {
            my_controller.my_actuator.disablePump();
            pump_thread_active = 0;
        }

    }

}

void light()
{
    Serial.println();
    Serial.println();
    Serial.print("the light value is ");
    Serial.print(my_controller.my_sensor.getLight());
    Serial.println();

    if (my_controller.my_sensor.getLight() > MIN_LIGHT_THRESHOLD)
    {
        minutes_of_light += 1;
        Serial.println("The light value is above the minimum light threshold.");
    }
    else
    {
        Serial.println("The light value is below the minimum light threshold required.");

    }
    light_readings_recorded += 1;


    if ((rtc.getHours() + GMT) < 7 || (rtc.getHours() + GMT) > 20) // check if it is too early or late to be turning on the lights
    {
        Serial.print("It is only");
        Serial.print(rtc.getHours() + GMT);
        Serial.print("o'clock. I cannot turn the lights on.");

        if (light_thread_active)
        {
            Serial.print("It is too late for lights. It is");
            Serial.print(rtc.getHours() + GMT);
            Serial.print("o'clock. I am turning the lights off.");
            my_controller.my_actuator.disableLED();
            light_thread_active = 0;
            light_readings_recorded = 0;
        }

        return;
    }

    /* There needs to be some sort of logic in here tracking the total light that has been bee obtained in a day, turn the lights on for the remainder of the day etc. */

    if (minutes_of_light < 15 && (light_thread_active == 0) && light_readings_recorded >= 20)
    {
        Serial.println("We did not get enough light in the previous time range, I am turning the lights on.");
        my_controller.my_actuator.enableLED();
        light_start_time = millis();
        light_thread_active = 1;
        light_readings_recorded = 0;
        minutes_of_light = 0;
    }
    else if (minutes_of_light > 15 && (light_thread_active == 0) && light_readings_recorded >= 20)
    {
      Serial.println("We got enough light during the previous time frame. Resetting our count.");
      light_readings_recorded = 0;
      minutes_of_light = 0; 
    }
    else if (light_thread_active)
    {
        if ( (millis() - light_start_time) >  ALLOWED_LED_TIME)
        {
            Serial.println("I am turning the lights off, it has been enough time");
            my_controller.my_actuator.disableLED();
            light_thread_active = 0;
        }

    }
  Serial.println();
  Serial.println();
}


void sensorState()
{
    my_controller.my_sensor.poll();

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
}
