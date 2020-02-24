#include <actuator.h>
#include "DHT.h"

actuator::actuator()
{
	pinMode(PUMP_PIN, OUTPUT);

	pinMode(LED_PIN, OUTPUT);  
} 

void actuator::enablePump()
{
  analogWrite(PUMP_PIN, 255); 
}

void actuator::disablePump()
{
  analogWrite(PUMP_PIN, 0);
}

void actuator::enableLED()
{
  analogWrite(LED_PIN, 255);
}

void actuator::disableLED()
{
    analogWrite(LED_PIN, 0);
}