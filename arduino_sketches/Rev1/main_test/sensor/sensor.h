#ifndef sensor_h
#define sensor_h

#include "DHT.h"

#define DHTTYPE DHT22
#define TEMP_HUM_PIN 10
#define SOIL_MOIST_PIN_1 A4
#define SOIL_MOIST_PIN_2 A4     //WHICH PIN?
#define SOIL_MOIST_PIN_3 A4     //WHICH PIN?
#define WATER_LEVEL_PIN 6 
#define LIGHT_SENSE_PIN_1 A1
#define LIGHT_SENSE_PIN_2 A1     //WHICH PIN?
#define LIGHT_SENSE_PIN_3 A1     //WHICH PIN?
#define MOIST_DIFF_TOL 30 // max difference allowed between sensor readings
#define LIGHT_DIFF_TOL 90
#define NUM_STORED_POINTS 30

class sensor
{
  public:
    sensor();
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

  	int soil_moist_avg;  

    int soil_moist[NUM_STORED_POINTS] = {0};

  	int water_present;

 	  int light_avg;  

    int light[NUM_STORED_POINTS] = {0};

    int reading_num;
};

#endif

