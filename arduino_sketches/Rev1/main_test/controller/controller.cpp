#include <controller.h>

void controller::init(char* ssid, char* pass)
{
	my_sensor.init();

    my_communicator.setup(ssid, pass);

    rtc.begin();

    rtcSetEpoch();
}

void controller::rtcSetEpoch()
{
	const int GMT = -5; //change this to adapt it to your time zone
    int numberOfTries = 0, maxTries = 6;
    do {
        epoch = WiFi.getTime();
        numberOfTries++;
    }
    while ((epoch == 0) && (numberOfTries < maxTries));

    if (numberOfTries == maxTries) {
        Serial.print("NTP unreachable!!");
        while (1);
    }
    else {
        Serial.print("Epoch received: ");
        Serial.println(epoch);
        rtc.setEpoch(epoch);
        Serial.println("Epoch has been set");
        Serial.println(rtc.getEpoch());
        Serial.println();
    }

    Serial.print("The epoch from rtc is:");
    Serial.println(rtc.getEpoch());

    // Print date...
    Serial.print(rtc.getDay());
    Serial.print("/");
    Serial.print(rtc.getMonth());
    Serial.print("/");
    Serial.print(rtc.getYear());
    Serial.print("\t");

    // Print Time
    Serial.print(rtc.getHours() + GMT);
    Serial.print(":");
    Serial.print(rtc.getMinutes());
    Serial.print(":");
    Serial.print(rtc.getSeconds());

    Serial.println();
}

void controller::checkPump() 
{
        //if the plant needs water and we have the water to do it
    if ((my_sensor.getSoilMoist() > DRY_SOIL_MOISTURE - ((DRY_SOIL_MOISTURE - WET_SOIL_MOISTURE) / 3)) && my_sensor.getWaterLevel() && (pump_thread_active == 0))
    {   
        
        Serial.println("Soil is dry, water is present. Enabling pump.");
        
        my_actuator.enablePump();
        pump_thread_active = 1;
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        pump_start_time = millis();
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    }
    else if (pump_thread_active)
    {   
        my_sensor.poll();
        if (!my_sensor.getWaterLevel() || !(my_sensor.getSoilMoist() > DRY_SOIL_MOISTURE - ((DRY_SOIL_MOISTURE - WET_SOIL_MOISTURE) / 3)))
        {   
            my_actuator.disablePump();
            pump_thread_active = 0;
        }
        else if ( (millis() - pump_start_time) >  ALLOWED_PUMPING_TIME)
        {  
            my_actuator.disablePump();
            pump_thread_active = 0;
        }
    
    }

}

void controller::checkLights()
{
    Serial.println();
    Serial.println();
    Serial.print("the light value is ");
    Serial.print(my_sensor.getLight());
    Serial.println();

    if (my_sensor.getLight() > MIN_LIGHT_THRESHOLD)
    {
        minutes_of_light += 1;
        Serial.println("The light value is above the minimum light threshold.");
    }
    else
    {
        Serial.println("The light value is below the minimum light threshold required.");

    }
    light_readings_recorded += 1;


    if ((rtc.getHours() + GMT) < 7 || (rtc.getHours() + GMT) > 20) // check if it is too early or late to be turning on the lights
    {
        Serial.print("It is only");
        Serial.print(rtc.getHours() + GMT);
        Serial.print("o'clock. I cannot turn the lights on.");

        if (light_thread_active)
        {
            Serial.print("It is too late for lights. It is");
            Serial.print(rtc.getHours() + GMT);
            Serial.print("o'clock. I am turning the lights off.");
            my_actuator.disableLED();
            light_thread_active = 0;
            light_readings_recorded = 0;
        }

        return;
    }

    /* There needs to be some sort of logic in here tracking the total light that has been bee obtained in a day, turn the lights on for the remainder of the day etc. */

    if (minutes_of_light < 15 && (light_thread_active == 0) && light_readings_recorded >= 20)
    {
        Serial.println("We did not get enough light in the previous time range, I am turning the lights on.");
        my_actuator.enableLED();
        light_start_time = millis();
        light_thread_active = 1;
        light_readings_recorded = 0;
        minutes_of_light = 0;
    }
    else if (minutes_of_light > 15 && (light_thread_active == 0) && light_readings_recorded >= 20)
    {
      Serial.println("We got enough light during the previous time frame. Resetting our count.");
      light_readings_recorded = 0;
      minutes_of_light = 0; 
    }
    else if (light_thread_active)
    {
        if ( (millis() - light_start_time) >  ALLOWED_LED_TIME)
        {
            Serial.println("I am turning the lights off, it has been enough time");
            my_actuator.disableLED();
            light_thread_active = 0;
        }

    }
  Serial.println();
  Serial.println();
}

void controller::set_setpoints()
{
	light_hours_min_setpoint = my_communicator.getLightMax();
	light_hours_max_setpoint = my_communicator.getLightMin();
	soil_moisture_setpoint_max = my_communicator.getMoistureMax();
	soil_moisture_setpoint_min = my_communicator.getMoistureMin();
}

