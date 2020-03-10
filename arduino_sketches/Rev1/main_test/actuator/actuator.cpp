#include <actuator.h>
#include "DHT.h"

actuator::actuator()
{
	pinMode(PUMP_PIN, OUTPUT);

	pinMode(LED_PIN, OUTPUT);  
} 

void actuator::enablePump()
{
  digitalWrite(PUMP_PIN, HIGH); 
}

void actuator::disablePump()
{
  digitalWrite(PUMP_PIN, LOW);
}

void actuator::enableLED()
{
  digitalWrite(LED_PIN, HIGH);
}

void actuator::disableLED()
{
   digitalWrite(LED_PIN, LOW);
}
