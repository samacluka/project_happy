#ifndef controller_h
#define controller_h

#include <RTCZero.h>

#include "sensor.h"
#include "actuator.h"
#include "communication.h"

#define MIN_LIGHT_THRESHOLD 470
#define ALLOWED_PUMPING_TIME 15000
#define ALLOWED_LED_TIME 20000
#define MAX_PUMP_PER_DAY 600000

#define WATER_LED_PIN 5

class controller
{
  public:
    controller(){};

    void init(char* ssid, char* pass);

    /* Set point functions */
    void set_setpoints();

    void rtcSetEpoch();

    void checkPump();

    void checkLights();

    void waterIndicatorOn();

    void waterIndicatorOff();

    float get_light_hours_data();

    int get_water_level_message_status();

    void set_water_level_message_status(int status);

    sensor my_sensor;

    actuator my_actuator;

    communication my_communicator;

    RTCZero rtc;

  private:
	
    /* Set Points that will be updated by get requests to server */
    int light_hours_min_setpoint;

    int light_hours_max_setpoint;

    int soil_moisture_min_setpoint;

    int soil_moisture_max_setpoint;

    int prev_min;

    /* Control data used to determine actuation. Updated by sensors. Sent to server via put requests */
    float light_hours_data;

    int water_applied_data;

    int temperature_data;

    int humidity_data;

    int soil_moisture_data;

    unsigned long int epoch;

    long int pump_start_time;
    
    long int light_start_time;

    int pump_thread_active = 0;

    int light_thread_active = 0;

    int minutes_of_light = 0;

    int seconds_of_pumping = 0;

    int time_since_last_pump;

    int total_minutes_of_light_today = 0;

    int light_readings_recorded = 0;

    const int GMT = -4;

    const int latest_on_hour = 20;

    const int earliest_on_hour = 7;

    char* controller_network_ssid;

    char* controller_network_password;

    int water_level_message_sent = 0;
};

#endif

