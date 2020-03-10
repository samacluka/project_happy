#ifndef actuator_h
#define actuator_h

#define PUMP_PIN 11
#define LED_PIN 12

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

