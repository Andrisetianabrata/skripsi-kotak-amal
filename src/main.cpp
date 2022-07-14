#include <Arduino.h>
#include "function.h"
void setup()
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println(Buzz.init_pin);

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
 
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
  delay(50);
  
  if(buzz_confirm == true)
  {
    door_state = true;
    Buzz.write(HIGH);
    delay(1000);
    Buzz.write(LOW);
    buzz_confirm = false;
  }

  if(buzz_wrong == true)
  {
    Buzz.write(HIGH);
    delay(250);
    Buzz.write(LOW);
    delay(250);
    Buzz.write(HIGH);
    delay(250);
    Buzz.write(LOW);
    delay(250);
    buzz_wrong = false;
  }

  if (door_secure_flag)
  {
    Selenoid.write(LOW);
  }

  if (Pintu.state() == tertutup && door_secure_flag)
  {
    Selenoid.write(HIGH);
    door_secure_flag = false;
  }

  if(Pintu.state() == terbuka && !door_secure_flag)
  {
    // Blynk.notify("Kotak amal terbuka dengan paksa");
  }
}
