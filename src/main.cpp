#include <Arduino.h>
#include "function.h"
void setup()
{
 
  setting_up();

  // Buzz.init_pin = D7;
  // Selenoid.init_pin = D0;
  // Pintu.init_pin = D1;
  // Vibration.init_pin = D10;

  Buzz.init(OUTPUT);
  Pintu.init(INPUT_PULLUP);
  // Selenoid.init(OUTPUT);
  Vibration.init(INPUT_PULLUP);

  // Selenoid.write(LOW);
  Buzz.write(HIGH);
  delay(1000);
  Buzz.write(LOW);
  // Selenoid.write(HIGH);
}

void loop()
{
  Blynk.run();
  getFingerprintID();
  delay(200);
  logic();
  while (gps_serial.available() > 0)
    gps.encode(gps_serial.read());

  Blynk.virtualWrite(V1, gps.location.lat());
  Blynk.virtualWrite(V2, gps.location.lng());
  myMap.location(1, gps.location.lat(), gps.location.lng(), "Kotak amal");
  
  lcd_blynk.print(0,0, "LAT:");
  lcd_blynk.print(4,0, gps.location.lat());
  lcd_blynk.print(0,1, "LNG:");
  lcd_blynk.print(4,1, gps.location.lng());
  lcd.setCursor(0,0);
  lcd.print("LAT: ");
  lcd.print(gps.location.lat(), 6);
  lcd.setCursor(0,1);
  lcd.print("LNG: ");
  lcd.print(gps.location.lng(), 6);

  if (gps.location.isUpdated())
  {
    Serial.println("no valid ");
    Serial.print("lat = ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("lng = ");
    Serial.println(gps.location.lng(), 6);
  }
  if(gps.location.isValid())
  {
    Serial.println("valid ");
    Serial.print("lat = ");
    Serial.println(gps.location.lat());
    Serial.print("lng = ");
    Serial.println(gps.location.lng());
  }
  if (gps.charsProcessed() < 10);
      // Serial.println(F("WARNING: No GPS data.  Check wiring."));
      
    Serial.print("Getaran = ");
    Serial.println(Vibration.read());
}
