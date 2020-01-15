#include <sensor.h>
#include "DHT.h"

#define DHTTYPE DHT22

DHT dht(TEMP_HUM_PIN, DHTTYPE);

sensor::sensor()
{
  pinMode(TEMP_HUM_PIN, INPUT);
  
  pinMode(SOIL_MOIST_PIN, INPUT);

  pinMode(WATER_LEVEL_PIN, INPUT);

  pinMode(LIGHT_SENSE_PIN, INPUT);
}

sensor::init()
{
  dht.begin();
}

sensor::getTempHum(float &temp, float &humidity)
{
  temp = dht.readTemperature();

  humidity = dht.readHumidity();
}

sensor::getSoilMoist(int &soil_moist)
{
  
}

sensor::getWaterLevel(bool &water_present)
{
  water_present = digitalRead(WATER_LEVEL_PIN);
}

sensor::getHowMuchLight(int &light)
{
  light = analogRead(LIGHT_SENSE_PIN)
}
