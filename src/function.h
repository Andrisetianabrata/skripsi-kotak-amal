#include <Arduino.h>
#include <SoftwareSerial.h>

// UTILITY
bool blynk_secure_state = false;
bool flag_state_buzz = false;

// Blynk
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "YourAuthToken";
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";
char server[] = "iot.serangkota.go.id";

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