#include <Arduino.h>
#include "function.h"

void setup()
{
    Selenoid.init_pin = D2;
    Buzz.init_pin = D0;
    Vibration.init_pin = D3;
    Pintu.init_pin = D7;
    Selenoid.setpin_output();
    Pintu.setpin_input();
    Vibration.setpin_input();
    Buzz.setpin_output();
    finger.begin(57600);

    if (finger.verifyPassword())
    {
        Serial.println("Found fingerprint sensor!");
    }
    else
    {
        Serial.println("Did not find fingerprint sensor :(");
        while (1)
        {
            delay(1);
        }
    }
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


    Buzz.write(HIGH);
    delay(1000);
    Buzz.write(LOW);
    delay(1000);
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connecting to Blynk");
    Serial.begin(9600);
    gps_serial.begin(9600);
    Blynk.begin(auth, ssid, pass, server, 8080);
    lcd.print("Connected to Blynk");
    lcd.clear();
}

void loop()
{
    Blynk.run();
    logic_function();
    Buzz.write(flag_state_buzz);
    getFingerprintID();
    myMap.location(0, lat, lng, "Kotak");
    lcd.setCursor(0, 0);
    lcd.print("lat :");
    lcd.print(lat, 6);
    lcd.setCursor(0, 1);
    lcd.print("lng :");
    lcd.print(lng, 6);
}

void logic_function()
{
    // jika sensor magnet menutup (true)
    if (Pintu.state())
    {
        // kunci selenoid
        Selenoid.write(HIGH);
    }

    // fungsi tombol pengaman Blynk dan alarm
    if (blynk_secure_state)
    {
        if (Vibration.state() && !flag_state_buzz)
        {
            // Blynk.notify("Ada yang mencoba memindahkan kotak amal");
            flag_state_buzz = true;
        }else if (!Pintu.state()){ // jika saat pengaman blyn dinyalakan dan sensor magnet terbuka
            // Blynk.notify("Ada yang membuka kotak amal secara paksa");
        }
    }
    // nyalakan alarm / buzzer selama 5 detik
    if (flag_state_buzz)
    {
        static unsigned long time = millis();
        if (millis() - time > 5000)
        {
            flag_state_buzz = false;
        }
    }

    // fungsi gps
    while (gps_serial.available() > 0)
        gps.encode(gps_serial.read());
    if (gps.location.isValid())
    {
        lat = gps.location.lat();
        lng = gps.location.lng();
        Serial.print("Satellite : ");
        Serial.println(gps.satellites.value());
        Serial.print("Latitude  : ");
        Serial.println(lat, 6);
        Serial.print("Longitude : ");
        Serial.println(lng, 6);
    }
    if (gps.charsProcessed() < 10)
        Serial.println(F("WARNING: No GPS data.  Check wiring."));
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
        Serial.println("No finger detected");
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
        Serial.println("Found a print match!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sidik jari cocok");
        lcd.setCursor(0, 0);
        lcd.print("dengan id ");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_NOTFOUND)
    {
        Serial.println("Did not find a match");
        // Blynk.notify("Sidik jari tak di kenal mencoba membuka kotak amal");
        lcd.print("Sidik jari tidak");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.setCursor(0, 1);
        lcd.print("cocok");
        lcd.clear();
        flag_state_buzz = true;
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
    lcd.print(finger.fingerID);

    // Blynk.notify("Kotak amal berhasil teruka");  
    lcd.clear();
    return finger.fingerID;
}

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
