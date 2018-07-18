#include <Arduino.h>

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)      // Arduino UNO
    #include <AltSoftSerial.h>
    AltSoftSerial bc95serial;
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)   // Arduino MEGA2560
    #define bc95serial Serial1
#endif

#include "BC95Udp.h"
#include "MicrogearNB.h"

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
    Serial.println(F("NB-IOT attached.."));
    Serial.println(BC95.getSignalStrength());
    Serial.println(BC95.getIPAddress());

    mg.init("deerdev","T6Vwv04nxpXbWr4","3b3ROsQhw68lKeV5VT47V1Eoi");
    mg.begin(5555);
}

void loop() {


    int chk = DHT.read22(DHT22_PIN);
    if (chk == DHTLIB_OK) {
        dtostrf(DHT.temperature,0,1,payload);
        strcat(payload,",");
        dtostrf(DHT.humidity,0,1,payload+strlen(payload));
        Serial.println(payload);
        mg.publish("/nbsensor2",payload);

//if (DHT.humidity > 65) mg.pushOwner("Hello from NB-IOT.");

         if (millis() - lastfeed > 30000) {
             strcpy(payload,"data=temp:");
             dtostrf(DHT.temperature,0,1,payload+strlen(payload));
             strcat(payload,",humid:");
             dtostrf(DHT.humidity,0,1,payload+strlen(payload));
             mg.writeFeed("nbiot",payload);
             Serial.println(payload);
             lastfeed =  millis();
         }
    }
    mg.loop();
    delay(5000);
}
