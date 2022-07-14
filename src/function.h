#include <Arduino.h>
#include <SoftwareSerial.h>

// UTILITY
bool blynk_secure_state = false;
bool buzz_confirm = false;
bool buzz_wrong = false;
bool door_state = false;
bool door_secure_flag = false;
bool terbuka = LOW;
bool tertutup = HIGH;
void logic_function();
uint8_t getFingerprintID();
int getFingerprintIDez();
float lat;
float lng;
unsigned long times = 0;

// Blynk
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "Admin";
char ssid[] = "Admin";
char pass[] = "";
char server[] = "iot.serangkota.go.id";

WidgetMap myMap(V0);
BLYNK_CONNECTED()
{
    Blynk.syncVirtual(V3);
}
BLYNK_WRITE(V3)
{
    blynk_secure_state = param.asInt();
}

// Pins
class Pin_
{
public:
    int init_pin;
    void setpin_input()
    {
        pinMode(init_pin, INPUT);
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
SoftwareSerial gps_serial(D4, D3); // RX, TX

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x21, 16, 2);

// FINGERPRINT
#include <Adafruit_Fingerprint.h>
SoftwareSerial mySerial(D5, D6);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);