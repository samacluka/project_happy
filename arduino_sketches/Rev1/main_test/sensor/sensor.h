#ifndef sensor_h
#define sensor_h

#include "DHT.h"

#define DHTTYPE DHT22
#define TEMP_HUM_PIN 10
#define SOIL_MOIST_PIN A4
#define WATER_LEVEL_PIN 6 
#define LIGHT_SENSE_PIN A1

class sensor
{
  public:
    sensor(int value); 
    void init();
    void poll();
    float getTemperature(); // Function to read the temperature & humidity value from the temperature sensor
    float getHumidity();
    int getSoilMoist(); // Function to read the soil moisture value from the  soil moisture sensor
    int getWaterLevel(); // Function to read the water level value from the  water level sensor
    int getLight(); // Function to read the current light levels of the plants environemnt

    /* These two set functions used only in the httpPut call if temp or humidity is NaN  */
    void setTemperatureZero();
    void setHumidityZero(); 
  private:
	
    float temperature;

  	float humidity;

  	int soil_moist;  

  	int water_present;

 	int light;  
};

#endif

