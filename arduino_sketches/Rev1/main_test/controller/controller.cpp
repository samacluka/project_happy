#include <controller.h>

void controller::init(char* ssid, char* pass)
{
	my_sensor.init();

    my_communicator.setServerID("encouragemint.herokuapp.com");

    my_communicator.setup(ssid, pass);

	controller_network_ssid = ssid;

	controller_network_password = pass;

    pinMode(WATER_LED_PIN, OUTPUT);

    rtc.begin();

    rtcSetEpoch();

    Serial.println("Set epoch successfully");

    my_communicator.sendToServer("System initialized succesfully.", "success");  

    char epochString[100];

    sprintf(epochString, "RTC set to: %d:%d.", rtc.getHours(),rtc.getMinutes());

    my_communicator.sendToServer(epochString,"success");
}

void controller::rtcSetEpoch()
{
	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("Wifi was not connected when trying to set epoch");
		my_communicator.setup(controller_network_ssid, controller_network_password);
	}

	Serial.println("Acquiring Epoch.");
	// const int GMT = -5; //change this to adapt it to your time zone
    int numberOfTries = 0, maxTries = 100;
    do {	
        // epoch = WiFi.getTime() + GMT * 3600;
        epoch = my_communicator.getTime() + GMT * 3600;
        numberOfTries++;
        if (WiFi.status() != WL_CONNECTED) 
        {
        	Serial.println("Wifi was not connected when trying to set epoch (in the loop)");
        	my_communicator.setup(controller_network_ssid, controller_network_password);
        }
    }
    while ((epoch == 0) && (numberOfTries < maxTries));

    if ((numberOfTries == maxTries) && (epoch == 0))
    {
        Serial.println("NTP unreachable!!");
        rtcSetEpoch();
        while(1);
    }
    else 
    {
        Serial.print("Epoch received: ");
        Serial.println(epoch);
        rtc.setEpoch(epoch);
        Serial.println("Epoch has been set");
    }

    // Print date...
    // Serial.print(rtc.getDay());
    // Serial.print("/");
    // Serial.print(rtc.getMonth());
    // Serial.print("/");
    // Serial.print(rtc.getYear());
    // Serial.print("\t");

    // // Print Time
    // Serial.print(rtc.getHours());
    // Serial.print(":");
    // Serial.print(rtc.getMinutes());
    // Serial.print(":");
    // Serial.print(rtc.getSeconds());
}

void controller::checkPump() 
{
        //if the plant needs water and we have the water to do it
    if ((my_sensor.getSoilMoist() > soil_moisture_max_setpoint - ((soil_moisture_max_setpoint - soil_moisture_min_setpoint) / 3)) && my_sensor.getWaterLevel() && (pump_thread_active == 0))
    {   
        // my_communicator.sendToServer("The plant is being watered.", "success");        
        my_actuator.enablePump();
        pump_thread_active = 1;
        pump_start_time = millis();
    }
    else if (pump_thread_active)
    {   
        my_sensor.poll();
        if (!my_sensor.getWaterLevel() || !(my_sensor.getSoilMoist() > (soil_moisture_max_setpoint - ((soil_moisture_max_setpoint - soil_moisture_min_setpoint) / 3))))
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

    if (((my_sensor.getLight() > MIN_LIGHT_THRESHOLD) || light_thread_active) && rtc.getMinutes() != prev_min)
    {
        minutes_of_light += 1;
        prev_min = rtc.getMinutes();
    }
    else
    {

    }

    if (rtc.getHours() == 0) {
        minutes_of_light = 0;
    } 

    light_hours_data = minutes_of_light/60.;

    Serial.print("light value in checkLights: ");
    Serial.println(light_hours_data);
    Serial.println(minutes_of_light);

    // Serial.println(((latest_on_hour - rtc.getHours()) * 60 - rtc.getMinutes()) < (light_hours_min_setpoint*60 - minutes_of_light));

    if (rtc.getHours() < earliest_on_hour || rtc.getHours() > latest_on_hour || ((latest_on_hour - rtc.getHours()) * 60 - rtc.getMinutes()) > (light_hours_min_setpoint*60 - minutes_of_light)) // check if it is too early or late to be turning on the lights
    {
        Serial.print("It is only");
        Serial.print(rtc.getHours());
        Serial.println("o'clock. I cannot turn the lights on.");

        if (light_thread_active)
        {
            Serial.print("It is too late for lights. It is");
            Serial.print(rtc.getHours());
            Serial.println("o'clock. I am turning the lights off.");
            my_actuator.disableLED();
            light_thread_active = 0;
            light_readings_recorded = 0;
            my_communicator.sendToServer("Plant lights have been turned off.", "success");
        }

        return;
    }

    if (((latest_on_hour - rtc.getHours()) * 60 - rtc.getMinutes()) < (light_hours_min_setpoint*60 - minutes_of_light) && !light_thread_active) {
        my_actuator.enableLED();
        light_start_time = millis();
        light_thread_active = 1;
        my_communicator.sendToServer("Plant lights have been turned on.", "success");
    }
}

void controller::set_setpoints()
{
	light_hours_min_setpoint = my_communicator.getLightMin();
	light_hours_max_setpoint = my_communicator.getLightMax();
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

float controller::get_light_hours_data()
{
    return light_hours_data;
}
