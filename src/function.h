#include <Arduino.h>
#include <SoftwareSerial.h>

// UTILITY
bool blynk_secure_state = false;
bool flag_door = 0;
bool buzz_confirm = false;
bool buzz_wrong = false;
bool door_state = false;
bool door_secure_flag = false;
bool terbuka = !LOW;
bool tertutup = !HIGH;
// unsigned long times = 0;

// Blynk
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "hy1i86C-X8p-a_rwzWusm8uDqbJ9KN1T";
char ssid[] = "PONDOK AZZUMAR";
char pass[] = "88021249";
char server[] = "iot.serangkota.go.id";

WidgetMap myMap(V0);
WidgetLED led(V3);
WidgetLCD lcd_blynk(V4);

// Pins
class Pin_
{
public:
  int init_pin;
  void setpin_input()
  {
    pinMode(init_pin, INPUT_PULLUP);
  }
  void setpin_output()
  {
    pinMode(init_pin, OUTPUT);
  }
  bool state()
  {
    return digitalRead(init_pin);
  }
  void write(bool state)
  {
    digitalWrite(init_pin, state);
  }
} Selenoid, Pintu, Vibration, Buzz;

// GPS
#include <TinyGPSPlus.h>
TinyGPSPlus gps;
SoftwareSerial gps_serial(D3, D2); // RX, TX

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x21, 16, 2);

// FINGERPRINT
#include <Adafruit_Fingerprint.h>
SoftwareSerial mySerial(D5, D6);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setting_up()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  gps_serial.begin(9600);
  Serial.println(Buzz.init_pin);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Init Finger print");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword())
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor terdeteksi");
    Serial.println("Found fingerprint sensor!");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor tidak");
    lcd.setCursor(0, 1);
    lcd.print("Terdeteksi");
    Serial.println("Did not find fingerprint sensor :(");
    while (1)
    {
      delay(1);
    }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0)
  {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else
  {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains ");
    Serial.print(finger.templateCount);
    Serial.println(" templates");
  }
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connetcing to");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  Blynk.begin(auth, ssid, pass, server, 8080);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("connected to");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  lcd.clear();
}

void logic()
{
  if (buzz_confirm == true)
  {
    door_state = true;
    Buzz.write(HIGH);
    delay(1000);
    Buzz.write(LOW);
    buzz_confirm = false;
  }

  if (buzz_wrong == true)
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
    if (Pintu.state() == tertutup)
    {
      Selenoid.write(LOW);
      flag_door = 1;
      Serial.println("Pintu terbuka");
      led.on();
    }
  }
  else
  {
    if (Pintu.state() == tertutup)
    {
      Serial.println("Pintu tertutup");
      Selenoid.write(HIGH);
      flag_door = 0;
      led.off();
    }
  }

  if (Pintu.state() == terbuka)
  {
    door_secure_flag = false;
  }

  if (Pintu.state() == terbuka && !door_secure_flag && flag_door == 0)
  {
    Blynk.notify("Kotak amal terbuka dengan paksa");
    Serial.println("pintu terbuka paksa");
  }
}

uint8_t getFingerprintID()
{
  uint8_t p = finger.getImage();
  switch (p)
  {
  case FINGERPRINT_OK:
    Serial.println("Image taken");
    break;
  case FINGERPRINT_NOFINGER:
    // Serial.println("No finger detected");
    return p;
  case FINGERPRINT_PACKETRECIEVEERR:
    Serial.println("Communication error");
    return p;
  case FINGERPRINT_IMAGEFAIL:
    Serial.println("Imaging error");
    return p;
  default:
    Serial.println("Unknown error");
    return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p)
  {
  case FINGERPRINT_OK:
    Serial.println("Image converted");
    break;
  case FINGERPRINT_IMAGEMESS:
    Serial.println("Image too messy");
    return p;
  case FINGERPRINT_PACKETRECIEVEERR:
    Serial.println("Communication error");
    return p;
  case FINGERPRINT_FEATUREFAIL:
    Serial.println("Could not find fingerprint features");
    return p;
  case FINGERPRINT_INVALIDIMAGE:
    Serial.println("Could not find fingerprint features");
    return p;
  default:
    Serial.println("Unknown error");
    return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK)
  {
    // Match
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Jari Terdeteksi");
    lcd.setCursor(0,1);
    lcd.print("Pintu terbuka");
    Serial.println("Found a print match!");
    buzz_confirm = true;
    door_secure_flag = true;
    delay(2000);
    lcd.clear();
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Communication error");
    return p;
  }
  else if (p == FINGERPRINT_NOTFOUND)
  {
    // Not match
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Jari Terdeteksi");
    lcd.setCursor(0,1);
    lcd.print("Tidak Terdaftar");
    Serial.println("Did not find a match");
    Blynk.notify("Sidik jari tidak terdaftar mencoba membuka kotak");
    buzz_wrong = true;
    delay(2000);
    lcd.clear();
    return p;
  }
  else
  {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);

  return finger.fingerID;
}
