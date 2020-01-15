#include <sensor.h>

sensor sensor();

void setup() 
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() 
{
  digitalWrite(LED_BUILTIN, HIGH); 
  // put your main code here, to run repeatedly:
  sensor.init();
  float temperature;
  float humidity;
  sensor.getTempHum(temperature, humidity);

  Serial.print("temperature = ");
  Serial.println(temperature, DEC);
  Serial.print("humidity = ");
  Serial.println(humidity, DEC);
  
  bool is_water_present;
  sensor.getSoilMost(is_water_present);
  Serial.print("water present = ");
  Serial.println(is_water_present, DEC);

  int light_value;
  sensor.getHowMuchLight(light_value);

  Serial.print("light_value= ");
  Serial.println(light_value, DEC);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);    
}
