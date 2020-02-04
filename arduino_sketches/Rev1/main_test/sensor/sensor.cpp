#include <sensor.h>

DHT dht(TEMP_HUM_PIN, DHTTYPE);

sensor::sensor(int num)
{
  pinMode(TEMP_HUM_PIN, INPUT);
  
  pinMode(SOIL_MOIST_PIN, INPUT);

  pinMode(WATER_LEVEL_PIN, INPUT);

  pinMode(LIGHT_SENSE_PIN, INPUT);
}

void sensor::init()
{
  dht.begin();
}

void sensor::poll()
{
  temperature = dht.readTemperature();

  humidity = dht.readHumidity();

  soil_moist = analogRead(SOIL_MOIST_PIN);  

  water_present = digitalRead(WATER_LEVEL_PIN);

  light = analogRead(LIGHT_SENSE_PIN);  
}

float sensor::getTemperature()
{
  return temperature;
}

float sensor::getHumidity()
{
  return humidity;
}

int sensor::getSoilMoist()
{
  return soil_moist; 
}	

int sensor::getWaterLevel()
{
  return water_present;
}

int sensor::getLight()
{
  return light;
}

void sensor::setTemperatureZero()
{
  temperature = 0;
  return;
}

void sensor::setHumidityZero()
{
  humidity = 0;
  return;
}