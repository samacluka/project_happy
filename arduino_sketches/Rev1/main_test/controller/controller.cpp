#include <controller.h>

void controller::init(char* ssid, char* pass)
{
	my_sensor.init();

    my_communicator.setup(ssid, pass);

    pinMode(WATER_LED_PIN, OUTPUT);

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
    if ((my_sensor.getSoilMoist() > soil_moisture_max_setpoint - ((soil_moisture_max_setpoint - soil_moisture_min_setpoint) / 3)) && my_sensor.getWaterLevel() && (pump_thread_active == 0))
    {   
        
        Serial.println("Soil is dry, water is present. Enabling pump.");
        my_communicator.sendToServer(message[WATERING_MESSAGE], message_type[SUCCESS]);        
        my_actuator.enablePump();
        pump_thread_active = 1;
        pump_start_time = millis();
    }
    else if (pump_thread_active)
    {   
        my_sensor.poll();
        if (!my_sensor.getWaterLevel() || !(my_sensor.getSoilMoist() > soil_moisture_max_setpoint - ((soil_moisture_max_setpoint - soil_moisture_max_setpoint) / 3)))
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
    if (my_sensor.getLight() > MIN_LIGHT_THRESHOLD)
    {
        minutes_of_light += 1;
        Serial.println("The light value is above the minimum light threshold.");
    }
    else
    {
        Serial.println("The light value is below the minimum light threshold required.");

    }

    if ((rtc.getHours() + GMT) == 0) {
        minutes_of_light = 0;
    } 

    light_hours_data = minutes_of_light/60;

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
            my_communicator.sendToServer(message[LIGHTS_OFF], type[SUCCESS]);
        }

        return;
    }

    if (((20 - (rtc.getHours() + GMT)) * 60 - rtc.getMinutes()) < (light_hours_min_setpoint - minutes_of_light) && !light_thread_active) {
        my_actuator.enableLED();
        light_start_time = millis();
        light_thread_active = 1;
        my_communicator.sendToServer(message[LIGHTS_ON_MESSAGE], type[SUCCESS]);
    }
}

void controller::set_setpoints()
{
	light_hours_min_setpoint = my_communicator.getLightMax();
	light_hours_max_setpoint = my_communicator.getLightMin();
	soil_moisture_max_setpoint = my_communicator.getMoistureMax();
	soil_moisture_min_setpoint = my_communicator.getMoistureMin();
}

void controller::waterIndicatorOn()
{
	digitalWrite(WATER_LED_PIN, 1);
}

void controller::waterIndicatorOff()
{
	digitalWrite(WATER_LED_PIN, 0);
}
