#ifndef actuator_h
#define actuator_h

#define PUMP_PIN A2
#define LED_PIN A3

class actuator
{
  public:
    actuator();
    void enablePump();
    void disablePump();
    void enableLED();
    void disableLED();
};

#endif

