#ifndef Sensor_h
#define Sensor_h


#define TEMP_HUM_PIN 5
#define SOIL_MOIST_PIN 10
#define WATER_LEVEL_PIN 6 
#define LIGHT_SENSE_PIN A1

class Sensor
{
  public:
  sensor(); 
  void getTempHum(float &temp, float &humidity); // Function to read the temperature & humidity value from the temperature sensor
  void getSoilMoist(int &soil_moist); // Function to read the soil moisture value from the  soil moisture sensor
  void getWaterLevel(bool &water_level); // Function to read the water level value from the  water level sensor
  void getHowMuchLight(int &light); // Function to read the current light levels of the plants environemnt
}
