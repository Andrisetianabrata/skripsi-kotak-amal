#include <Arduino.h>
#include "function.h"
void setup()
{
  setting_up();
  Buzz.init_pin = D7;
  Selenoid.init_pin = D0;
  Pintu.init_pin = D1;
  Buzz.setpin_output();
  Pintu.setpin_input();
  Selenoid.setpin_output();

  Selenoid.write(LOW);
  Buzz.write(HIGH);
  delay(1000);
  Buzz.write(LOW);
  Selenoid.write(HIGH);
}

void loop()
{
  getFingerprintID();
  delay(50);
  logic();
  while (gps_serial.available() > 0)
    gps.encode(gps_serial.read());

  if(gps.location.isValid())
  {
    lat = gps.location.lat();
    lng = gps.location.lng();
  }
  Serial.printf("lat = %f\nlng = %f\n", lat, lng);
}
