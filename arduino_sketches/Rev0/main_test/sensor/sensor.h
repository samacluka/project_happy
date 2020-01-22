#ifndef sensor_h
#define sensor_h


#define TEMP_HUM_PIN 10
#define SOIL_MOIST_PIN A4
#define WATER_LEVEL_PIN 6 
#define LIGHT_SENSE_PIN A1

class sensor
{
  public:
    sensor(int value); 
    void init();
    void getTempHum(float &temp, float &humidity); // Function to read the temperature & humidity value from the temperature sensor
    void getSoilMoist(int &soil_moist); // Function to read the soil moisture value from the  soil moisture sensor
    void getWaterLevel(int &water_level); // Function to read the water level value from the  water level sensor
    void getHowMuchLight(int &light); // Function to read the current light levels of the plants environemnt

    void static waterISR();
};

#endif

