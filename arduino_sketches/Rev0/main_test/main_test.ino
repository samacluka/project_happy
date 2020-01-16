#include <sensor.h>
#include <actuator.h>

#define DRY_SOIL_MOISTURE 798
#define WET_SOIL_MOISTURE 395

void setup() 
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
}

void loop() 
{
  sensor my_sensor(0);

  actuator my_actuator(0);
  
  digitalWrite(LED_BUILTIN, HIGH); 
  // put your main code here, to run repeatedly:
  my_sensor.init();
  float temperature;
  float humidity;
  my_sensor.getTempHum(temperature, humidity);

  Serial.print("temperature = ");
  Serial.println(temperature, DEC);
  Serial.print("humidity = ");
  Serial.println(humidity, DEC);

  int dryness_of_soil;
  my_sensor.getSoilMoist(dryness_of_soil);
  Serial.print("soil moisture = ");
  Serial.println(dryness_of_soil, DEC);

// Logic for turning on pump if the moisture detector is low
  //if (dryness_of_soil > DRY_SOIL_MOISTURE - ((DRY_SOIL_MOISTURE - WET_SOIL_MOISTURE) / 3))
  //{
      //my_actuator.enablePump();
      //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      //delay(15000); 
      //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
     // my_actuator.disablePump();
   //   delay(1000);
 // }
  
  int is_water_present;
  my_sensor.getWaterLevel(is_water_present);
  Serial.print("water present = ");
  Serial.println(is_water_present, DEC);

  int light_value;
  my_sensor.getHowMuchLight(light_value);

  Serial.print("light_value= ");
  Serial.println(light_value, DEC);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);    
}
