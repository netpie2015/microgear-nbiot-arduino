#include <Arduino.h>

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)      // Arduino UNO
    #include <AltSoftSerial.h>
    AltSoftSerial bc95serial;
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)   // Arduino MEGA2560
    #define bc95serial Serial1
#endif

#include "BC95Udp.h"
#include "MicrogearNB.h"

#define APPID    "{YOUR_APPID}"
#define KEY      "{YOUR_KEY}"
#define SECRET   "{YOUR_SECRET}"

BC95UDP client;
Microgear mg(&client);

char payload[32];

void setup() {
    bc95serial.begin(9600);
    BC95.begin(bc95serial);

    Serial.begin(9600);
    Serial.println(F("Starting..."));

    BC95.reset();
    Serial.println(BC95.getIMEI());
    Serial.println(BC95.getIMSI());

    while (!BC95.attachNetwork()) {
        Serial.println("...");
        delay(1000);
    }
    Serial.println(F("NB-IOT attached\n RSSI:"));
    Serial.println(BC95.getSignalStrength());
    Serial.println(BC95.getIPAddress());

    mg.init(APPID, KEY, SECRET);
    mg.begin(5555);
}

void loop() {
    mg.publish("/nbsensor2/rssi", BC95.getSignalStrength());
    mg.loop();
    delay(5000);
}