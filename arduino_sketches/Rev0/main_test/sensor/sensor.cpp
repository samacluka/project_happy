#include <sensor.h>
#include "DHT.h"

#define DHTTYPE DHT22

DHT dht(TEMP_HUM_PIN, DHTTYPE);

sensor::sensor(int num)
{
  pinMode(TEMP_HUM_PIN, INPUT);
  
  pinMode(SOIL_MOIST_PIN, INPUT);

  pinMode(WATER_LEVEL_PIN, INPUT);

  pinMode(LIGHT_SENSE_PIN, INPUT);

  // attachInterrupt(digitalPinToInterrupt(WATER_LEVEL_PIN), waterISR, LOW);
}

void sensor::init()
{
  dht.begin();
}

void sensor::getTempHum(float &temp, float &humidity)
{
  temp = dht.readTemperature();

  humidity = dht.readHumidity();
}

void sensor::getSoilMoist(int &soil_moist)
{
  soil_moist = analogRead(SOIL_MOIST_PIN);
}	

void sensor::getWaterLevel(int &water_present)
{
  water_present = digitalRead(WATER_LEVEL_PIN);
}

void sensor::getHowMuchLight(int &light)
{
  light = analogRead(LIGHT_SENSE_PIN);
}

// void sensor::waterISR()
// {
//   delay(10000);
// 	Serial.print("HELLO I AM THE WATER ISR FEAR ME!");
// }