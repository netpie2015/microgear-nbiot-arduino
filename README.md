# microgear-nbiot-arduino

microgear-nbiot-arduino คือ client library สำหรับบอร์ด Arduino และ NB-IOT shield ในการเชื่อมต่อเข้ากับ NETPIE platform รายละเอียดเกี่ยวกับ NETPIE สามารถศึกษาได้จาก http://netpie.io

## ความเข้ากันได้

Arduino : UNO, MEGA2560

NB-IOT shield/module : True NB-IOT shield, AIS NB-IOT shield, Quectel BC95-B8

## ฟังก์ชั่นการใช้งาน

### include library และประกาศตัวแปร
```C++
#include "BC95Udp.h"
#include "MicrogearNB.h"

BC95UDP client;
Microgear mg(&client);
```

### init และเปิดการทำงานของ microgear
```C++
mg.init(APPID, KEY, SECRET);
mg.begin(LOCAL_PORT);
```

**ส่งข้อมูลเข้า topic**
- mg.publish(char **topic*, char **payload*) 
- mg.publish(char **topic*, int *value*) 

```C++
mg.publish("/home/temp","24.6");
```

**ส่งข้อมูลลง feed**
- mg.writeFeed(char **feedid*, char **payload*) 
- mg.writeFeed(char **feedid*, char **payload*,  char **apikey*) 

วิธีการ authorize การเขียน feed มี 2 แบบ แบบแรกคือระบุ apikey ของ feed หรือแบบที่ 2 ไม่ระบุ apikey แต่ต้องให้สิทธิ์ appid ในการเขียน feed (ตั้งค่าในหน้า feed management บนเว็บ netpie.io)
```C++
mg.writeFeed("myfeed","temp:24.6,humid:62.8");
```

**push ข้อความหาเจ้าของ**
- mg.pushOwner(char **text*) 
```C++
mg.pushOwner("สวัสดีจาก NB-IOT device");
```
**คำแนะนำ**
1. ในฟังก์ชั่น loop() ของ arduino ควรมีการเรียก mg.loop() เป็นระยะๆ เพื่อตรวจสอบข้อมูลที่เข้ามาใน udp socket
```C++
mg.loop();
```
2. สามารถปรับแต่งค่า setting เบื้องต้นได้ในไฟล์ MicrogearNB.h
```C++
#define MICROGEARNB_USE_EXTERNAL_BUFFER     0
#define DATA_BUFFER_SIZE                   128
```
DATA_BUFFER_SIZE คือขนาดของ memory ที่ MicrogearNB จะ allocate ให้สำหรับใช้เป็น buffer สำหรับพักข้อมูลที่เข้าและออกไปยัง socket หากต้องการ reuse memory buffer ในโค้ดโปรแกรม คุณสามารถส่งผ่านตัวแปรภายนอกเข้ามาให้ microgear ใช้เป็น buffer แทนที่จะให้ microgear allocate memory เองได้ ด้วยการเซต
```C++
#define MICROGEARNB_USE_EXTERNAL_BUFFER     1
```
และเรียกใช้ฟังก์ชั่น mg.setExternalBuffer(char *extbuff, size_t extbuffsize)
```C++
char data[100];
mg.setExternalBuffer(data, 100);
```

3. เราสามารถตามเข้าไปปรับแต่งค่าในไฟล์​bc95udp/settings.h ได้อีกด้วย 

```
#define DATA_BUFFER_SIZE                128

#define BC95_USE_EXTERNAL_BUFFER        1
#define BC95_PRINT_DEBUG                0
#define BC95_DEFAULT_SERIAL_TIMEOUT     500
#define BC95_BUFFER_SIZE                DATA_BUFFER_SIZE

#define BC95UDP_USE_EXTERNAL_BUFFER     1
#define BC95UDP_SHARE_GLOBAL_BUFFER     1
#define BC95UDP_SERIAL_READ_CHUNK_SIZE  7
#define BC95UDP_BUFFER_SIZE             DATA_BUFFER_SIZE

#define DNS_MAX_RETRY                   5
#define DNS_DEFAULT_SERVER              IPAddress(8,8,8,8)

#define NTP_DEFAULT_SERVER              "time.nist.gov"

#define COAP_ENABLE_ACK_CALLBACK        0
```

## ตัวอย่างโค้ดบน Arduino UNO

ตัวอย่างการใช้งานกับ NETPIE ในขณะนี้รองรับการ publish, writefeed และส่ง push notification ในโค้ดจะใช้ AltSoftSerial (เป็น library ที่ต้องติดตั้งแยก) ในการทำ software serial เพื่อสื่อสารกับ NB-IOT shield

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

MicrogearNB library มาพร้อมกับ BC95UDP module ซึ่งถูก implement จาก UDP abstract class มาตรฐานของ Arduino จึงสามารถนำไปใช้งานร่วมกับ UDP library อื่นๆของ Arduino ได้ไม่ยาก ตัวอย่างการส่ง raw UDP data ไปยัง DNS server ของ Google ซึ่งมี IP address เป็น 8.8.8.8

```C++
#include <Arduino.h>
#include <AltSoftSerial.h>
#include "BC95Udp.h"

AltSoftSerial bc95serial;

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

MicrogearNB library ยังมาพร้อมกับฟีเจอร์ DNS สำหรับการ resolve domain name อัตโนมัติผ่าน NB-IOT ซึ่งเราสามารถใช้ hostname แทน IP address ใน UDP API ได้เลย เช่น

```C++
udpclient.beginPacket("coap.server.com", 5683);  
```

แต่เพื่อให้เกิดการใช้ memory อย่างมีประสิทธิภาพ ซึ่งสำคัญมากกับบอร์ดที่มี memory น้อยอย่างเช่น UNO จึงขอแนะนำให้ resolve IP ก่อนจึงค่อยนำไปใช้งาน แบบในโค้ดต่อไปนี้

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
    dns.getHostByName("google.com", remoteip);

    Serial.print("The resolved IP address is : ");
    Serial.println(remoteip);
}

void loop() {
  
}

```

นอกจาก DNS และ CoAP แล้ว เรายังสามารถส่งโปรโตโคล NTP sync เวลากับ time server ผ่าน NB-IOT ได้อีกด้วย ตามตัวอย่างนี้

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
