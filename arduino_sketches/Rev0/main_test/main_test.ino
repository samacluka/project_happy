#include <sensor.h>

void setup() 
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
}

void loop() 
{
  sensor my_sensor(0);
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
  delay(500);    
}
