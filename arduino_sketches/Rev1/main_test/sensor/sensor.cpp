#include <sensor.h>

DHT dht(TEMP_HUM_PIN, DHTTYPE);

sensor::sensor()
{
  pinMode(TEMP_HUM_PIN, INPUT);
  
  pinMode(SOIL_MOIST_PIN_1, INPUT);

  pinMode(SOIL_MOIST_PIN_2, INPUT);

  pinMode(SOIL_MOIST_PIN_3, INPUT);

  pinMode(WATER_LEVEL_PIN, INPUT);

  pinMode(LIGHT_SENSE_PIN_1, INPUT);

  pinMode(LIGHT_SENSE_PIN_2, INPUT);

  pinMode(LIGHT_SENSE_PIN_3, INPUT);
}

void sensor::init()
{
  dht.begin();
}

void sensor::poll()
{
  if (reading_num == 30) {
    reading_num = 0;
  }

  temperature = dht.readTemperature();

  humidity = dht.readHumidity();

  int moist_1 = analogRead(SOIL_MOIST_PIN_1);
  int moist_2 = analogRead(SOIL_MOIST_PIN_2);
  int moist_3 = analogRead(SOIL_MOIST_PIN_3);


  int moist_1_percentage = ((max_moist_1_reading - moist_1) / (max_moist_1_reading - min_moist_1_reading)) * 100;
  int moist_2_percentage = ((max_moist_2_reading - moist_2) / (max_moist_2_reading - min_moist_2_reading)) * 100;
  int moist_3_percentage = ((max_moist_3_reading - moist_3) / (max_moist_3_reading - min_moist_3_reading)) * 100;

  if (abs(moist_1_percentage-moist_2_percentage) > MOIST_DIFF_TOL && abs(moist_1_percentage-moist_3_percentage) > MOIST_DIFF_TOL && abs(moist_2_percentage-moist_3_percentage) > MOIST_DIFF_TOL) {
    soil_moist[reading_num] = 0;
  }
  else if (abs(moist_1_percentage-moist_2_percentage) < MOIST_DIFF_TOL && abs(moist_1_percentage-moist_3_percentage) > MOIST_DIFF_TOL && abs(moist_2_percentage-moist_3_percentage) > MOIST_DIFF_TOL) {
    soil_moist[reading_num] = (moist_1_percentage + moist_2_percentage) / 2;
  }
  else if (abs(moist_1_percentage-moist_2_percentage) > MOIST_DIFF_TOL && abs(moist_1_percentage-moist_3_percentage) < MOIST_DIFF_TOL && abs(moist_2_percentage-moist_3_percentage) > MOIST_DIFF_TOL) {
    soil_moist[reading_num] = (moist_1_percentage + moist_3_percentage) / 2;
  }
  else if (abs(moist_1_percentage-moist_2_percentage) > MOIST_DIFF_TOL && abs(moist_1_percentage-moist_3) > MOIST_DIFF_TOL && abs(moist_2_percentage-moist_3_percentage) < MOIST_DIFF_TOL) {
    soil_moist[reading_num] = (moist_2_percentage + moist_3_percentage) / 2;
  }
  else {
    soil_moist[reading_num] = (moist_1_percentage + moist_2_percentage + moist_3_percentage)  / 3;
  }

  int tmp = 0;

  for (int i = 0; i < NUM_STORED_POINTS; i++) {
    tmp += soil_moist[i];
  }

  soil_moist_avg = tmp / NUM_STORED_POINTS;

  water_present = digitalRead(WATER_LEVEL_PIN);

  int light_1 = analogRead(LIGHT_SENSE_PIN_1);
  int light_2 = analogRead(LIGHT_SENSE_PIN_2);
  int light_3 = analogRead(LIGHT_SENSE_PIN_3);


  if (abs(light_1-light_2) > LIGHT_DIFF_TOL && abs(light_1-light_3) > LIGHT_DIFF_TOL && abs(light_2-light_3) > LIGHT_DIFF_TOL) {
    light[reading_num] = 0;
  }
  else if (abs(light_1-light_2) < LIGHT_DIFF_TOL && abs(light_1-light_3) > LIGHT_DIFF_TOL && abs(light_2-light_3) > LIGHT_DIFF_TOL) {
    light[reading_num] = (light_1 + light_2) / 2;
  }
  else if (abs(light_1-light_2) > LIGHT_DIFF_TOL && abs(light_1-light_3) < LIGHT_DIFF_TOL && abs(light_2-light_3) > LIGHT_DIFF_TOL) {
    light[reading_num] = (light_1 + light_3) / 2;
  }
  else if (abs(light_1-light_2) > LIGHT_DIFF_TOL && abs(light_1-light_3) > LIGHT_DIFF_TOL && abs(light_2-light_3) < LIGHT_DIFF_TOL) {
    light[reading_num] = (light_2 + light_3) / 2;
  }
  else {
    light[reading_num] = (light_1 + light_2 + light_3)  / 3;
  }

  tmp = 0;

  for (int i = 0; i < NUM_STORED_POINTS; i++) {
    tmp += light[i];
  }

  light_avg = tmp / NUM_STORED_POINTS;

  reading_num++;
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
  return soil_moist_avg; 
}	

int sensor::getWaterLevel()
{
  return water_present;
}

int sensor::getLight()
{
  return light_avg;
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
