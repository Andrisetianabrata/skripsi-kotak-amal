#include <Arduino.h>
#include "function.h"
void setup()
{
  setting_up();
  Buzz.init_pin = D7;
  Selenoid.init_pin = D8;
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
  delay(500);
  logic();
  // Serial.println(Pintu.state());
}
