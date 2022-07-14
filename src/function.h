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
// void logic_function();
// uint8_t getFingerprintID();
// int getFingerprintIDez();
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
SoftwareSerial gps_serial(D4, D3); // RX, TX

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x21, 16, 2);

// FINGERPRINT
#include <Adafruit_Fingerprint.h>
SoftwareSerial mySerial(D5, D6);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

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
		Serial.println("Found a print match!");
		buzz_confirm = true;
		door_secure_flag = true;
		times = millis();
	}
	else if (p == FINGERPRINT_PACKETRECIEVEERR)
	{
		Serial.println("Communication error");
		return p;
	}
	else if (p == FINGERPRINT_NOTFOUND)
	{
		// Not match
		Serial.println("Did not find a match");
		buzz_wrong = true;
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

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez()
{
	uint8_t p = finger.getImage();
	if (p != FINGERPRINT_OK)
		return -1;

	p = finger.image2Tz();
	if (p != FINGERPRINT_OK)
		return -1;

	p = finger.fingerFastSearch();
	if (p != FINGERPRINT_OK)
		return -1;

	// found a match!
	Serial.print("Found ID #");
	Serial.print(finger.fingerID);
	Serial.print(" with confidence of ");
	Serial.println(finger.confidence);
	return finger.fingerID;
}

void setting_up()
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
		Selenoid.write(LOW);
		Serial.println("Pintu terbuka");
	}else{
		Serial.println("pintu tertutup");
	}

	if (Pintu.state() == tertutup && door_secure_flag)
	{
		Selenoid.write(HIGH);
		door_secure_flag = false;
	} 

	if (Pintu.state() == terbuka && !door_secure_flag)
	{
		// Blynk.notify("Kotak amal terbuka dengan paksa");
		Serial.println("pintu terbuka paksa");
	}
}