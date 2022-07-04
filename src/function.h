#include <Arduino.h>
#include <SoftwareSerial.h>

// Blynk
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "YourAuthToken";
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";
char server[] = "iot.serangkota.go.id";

// Pins
class pin_
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
} selenoid, pintu, vibration;

// GPS
#include <TinyGPSPlus.h>
TinyGPSPlus gps;
SoftwareSerial gps_serial(D4, D6); // RX, TX

