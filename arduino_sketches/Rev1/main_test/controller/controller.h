#ifndef controller_h
#define controller_h

#include <RTCZero.h>

#include "sensor.h"
#include "actuator.h"
#include "communication.h"

#define DRY_SOIL_MOISTURE 798
#define WET_SOIL_MOISTURE 395
#define MIN_LIGHT_THRESHOLD 470
#define ALLOWED_PUMPING_TIME 5000
#define ALLOWED_LED_TIME 20000

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

    void checkPump();

    void checkLights();

    sensor my_sensor;

    actuator my_actuator;

    communication my_communicator;

  private:
	
    /* Set Points that will be updated by get requests to server */
    int light_hours_setpoint;

    int soil_moisture_setpoint_max;

    int soil_moisture_setpoint_min;

    /* Control data used to determine actuation. Updated by sensors. Sent to server via put requests */
    int light_hours_data;

    int water_applied_data;

    int temperature_data;

    int humidity_data;

    int soil_moisture_data;

    RTCZero rtc;

    unsigned int epoch;

    long int pump_start_time;
    
    long int light_start_time;

    int pump_thread_active = 0;

    int light_thread_active = 0;

    int minutes_of_light = 0;

    int total_minutes_of_light_today = 0;

    int light_readings_recorded = 0;

    const int GMT = -5;
};

#endif

