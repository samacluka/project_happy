#ifndef controller_h
#define controller_h

#include <RTCZero.h>

#include "sensor.h"
#include "actuator.h"
#include "communication.h"


class controller
{
  public:
    controller(){};

    void init(char* ssid, char* pass);

    /* Set point functions */
    void set_setpoints();

    int get_light_hours_setpoint();

    int get_soil_moisture_setpoint();
    
    /* Data functions */
    int get_light_hours_data();

    int get_water_applied_data();

    float get_temperature_data();

    float get_humidity_data();

    int get_soil_moisture_data();

    void set_light_hours_data();

    void set_water_applied_data();

    void set_temperature_data();

    void set_humidity_data();

    void set_soil_moisture_data();

    void send_data_to_server();

    void rtcSetEpoch();

    sensor my_sensor;

    actuator my_actuator;

    communication my_communicator;

  private:
	
    /* Set Points that will be updated by get requests to server */
    int light_hours_setpoint;

    int soil_moisture_setpoint;

    /* Control data used to determine actuation. Updated by sensors. Sent to server via put requests */
    int light_hours_data;

    int water_applied_data;

    int temperature_data;

    int humidity_data;

    int soil_moisture_data;

    RTCZero rtc;

    unsigned int epoch;
};

#endif

