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
