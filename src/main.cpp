#include <Arduino.h>
#include "function.h"

void logic_function();

void setup()
{
    selenoid.init_pin = D2;
    vibration.init_pin = D3;
    pintu.init_pin = D7;
    selenoid.setpin_output();
    pintu.setpin_input();
    vibration.setpin_input();
    Serial.begin(9600);
    gps_serial.begin(9600);
    Blynk.begin(auth, ssid, pass, server, 8080);
}

void loop()
{
    Blynk.run();
    logic_function();
}

void logic_function()
{
    if (pintu.state() == 1)
    {
        selenoid.write(HIGH);
    }

    if (vibration.state() == HIGH)
    {
        // Blynk.notify("Ada yang mencoba memindahkan kotak amal");
    }

    while (gps_serial.available() > 0)
        gps.encode(gps_serial.read());

    if (gps.location.isValid())
    {
        Serial.print("Satellite : ");
        Serial.println(gps.satellites.value());
        Serial.print("Latitude  : ");
        Serial.println(gps.location.lat(), 6);
        Serial.print("Longitude : ");
        Serial.println(gps.location.lng(), 6);
    }

    if (gps.charsProcessed() < 10)
        Serial.println(F("WARNING: No GPS data.  Check wiring."));
}