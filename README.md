# microgear-nbiot-arduino

microgear-nbiot-arduino คือ client library สำหรับบอร์ด Arduino และ NB-IOT shield ในการเชื่อมต่อเข้ากับ NETPIE platform รายละเอียดเกี่ยวกับ NETPIE สามารถศึกษาได้จาก http://netpie.io

## ความเข้ากันได้

Arduino : UNO, MEGA2560

NB-IOT shield/module : True NB-IOT shield, AIS NB-IOT shield, Quectel BC95-B8

## ตัวอย่างการใช้งาน

ตัวอย่างการใช้งานกับ NETPIE ในขณะนี้รองรับการ publish, writefeed และส่ง push notification

```C++
#include <Arduino.h>
#include "AltSoftSerial.h"
#include "BC95Udp.h"
#include "MicrogearNB.h"

#define APPID    "{YOUR_APPID}"
#define KEY      "{YOUR_KEY}"
#define SECRET   "{YOUR_SECRET}"

AltSoftSerial bc95serial;
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
```

library ชุดนี้ยังรองรับการรับส่งข้อมูลผ่านโปรโตคอล UDP ในมาตรฐานของ Arduino UDP class ตัวอย่างการส่ง UDP packet สำหรับ DNS query ไปยัง DNS server 8.8.8.8

```C++
#include <Arduino.h>
#include <AltSoftSerial.h>
#include "BC95Udp.h"

AltSoftSerial bc95serial;

// 8.8.8.8 is the Google's public DNS server.
#define SERVER_IP     IPAddress(8, 8, 8, 8)
#define SERVER_PORT   53

// This binary string represents a UDP paylaod of the DNS query for the domain name nexpie.com
uint8_t udpdata[] = "\xC0\x5B\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x06\x6E\x65\x78\x70\x69\x65\x03\x63\x6F\x6D\x00\x00\x01\x00\x01";

BC95UDP udpclient;
uint8_t buff[64];

void printHEX(uint8_t *buff, size_t len) {
    for (int i=0; i<len; i++) {
        if (buff[i]<16) Serial.print(" 0");
        else Serial.print(" ");
        Serial.print(buff[i], HEX);
    }
}

void setup() {
    bc95serial.begin(9600);
    BC95.begin(bc95serial);
    BC95.reset();

    Serial.begin(9600);
    while (!BC95.attachNetwork()) {
        Serial.println("...");
        delay(1000);
    }

    udpclient.begin(8053);
    udpclient.beginPacket(SERVER_IP, SERVER_PORT);    
    udpclient.write(udpdata, 28);
    udpclient.endPacket();

    while (udpclient.parsePacket() == 0) {
        delay(500);
    }

    size_t len = udpclient.read(buff, 64);

    Serial.println(F("\n\nReceive UDP payload : "));
    printHEX(buff, len);
}

void loop() {
  
}

```

library ชุดนี้มาพร้อม DNS class สำหรับการ resolve domain name ผ่าน NB-IOT

```C++
#include <Arduino.h>
#include <AltSoftSerial.h>
#include "Dns.h"

AltSoftSerial bc95serial;

DNSClient dns;
IPAddress remoteip;

void setup() {
    bc95serial.begin(9600);
    BC95.begin(bc95serial);
    BC95.reset();

    Serial.begin(9600);
    Serial.println(F("Starting..."));

    while (!BC95.attachNetwork()) {
        Serial.println("...");
        delay(1000);
    }
    Serial.println(F("NB-IOT attached.."));

    dns.begin();
    dns.getHostByName("gogole.com", remoteip);

    Serial.print("The resolved IP address is : ");
    Serial.println(remoteip);
}

void loop() {
  
}

```

และยังสามารถ NTP sync เวลากับ time server ผ่าน NB-IOT ได้อีกด้วย

```C++
#include <Arduino.h>
#include <AltSoftSerial.h>
#include "NTPClient.h"

AltSoftSerial bc95serial;

BC95UDP udp;
NTPClient ntpclient(udp);

void setup() {
    bc95serial.begin(9600);
    BC95.begin(bc95serial);
    BC95.reset();

    Serial.begin(9600);
    Serial.println(F("Starting..."));

    while (!BC95.attachNetwork()) {
        Serial.println("...");
        delay(1000);
    }
    Serial.println(F("NB-IOT attached.."));

    ntpclient.begin();
    ntpclient.update();
    Serial.print("The current GMT time is : ");
    Serial.println(ntpclient.getFormattedTime());
}

void loop() {
  
}

```
