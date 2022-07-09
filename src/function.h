#include <Arduino.h>
#include <SoftwareSerial.h>

// UTILITY
bool blynk_secure_state = false;
bool flag_state_buzz = false;
void logic_function();
uint8_t getFingerprintID();
int getFingerprintIDez();
float lat;
float lng;

// Blynk
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "YourAuthToken";
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";
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
    uint8_t init_pin;
    void setpin_input()
    {
        pinMode(init_pin, OUTPUT);
    }
    void setpin_output()
    {
        pinMode(init_pin, INPUT_PULLUP);
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
SoftwareSerial gps_serial(D4, D6); // RX, TX

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x21, 16, 2);

// FINGERPRINT
#include <Adafruit_Fingerprint.h>
SoftwareSerial finger_serial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&finger_serial);