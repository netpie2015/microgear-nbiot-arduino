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

3. เราสามารถตามเข้าไปปรับแต่งค่าการทำงานในระดับที่ลึกลงไปอีกได้ในไฟล์ settings.h

```
#define DATA_BUFFER_SIZE                160

#define BC95_USE_EXTERNAL_BUFFER        1
#define BC95_PRINT_DEBUG                0
#define BC95_DEFAULT_SERIAL_TIMEOUT     500
#define BC95_BUFFER_SIZE                DATA_BUFFER_SIZE

#define BC95UDP_USE_EXTERNAL_BUFFER     1
#define BC95UDP_SHARE_GLOBAL_BUFFER     1
#define BC95UDP_SERIAL_READ_CHUNK_SIZE  7
#define BC95UDP_BUFFER_SIZE             DATA_BUFFER_SIZE

#define DNS_CACHE_SLOT                  1
#define DNS_CACHE_SIZE                  24
#define DNS_CACHE_EXPIRE_MS             0
#define DNS_MAX_RETRY                   5
#define DNS_DEFAULT_SERVER              IPAddress(8,8,8,8)

#define NTP_DEFAULT_SERVER              "time.nist.gov"

#define COAP_ENABLE_ACK_CALLBACK        1
#define COAP_CONFIRMABLE                0
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
    BC95.reset();
    
    Serial.begin(9600);
    Serial.println(F("Microgear Arduino NB-IoT Start!"));
    Serial.print(F("IMEI: "));
    Serial.println(BC95.getIMEI());
    Serial.print(F("IMSI: "));
    Serial.println(BC95.getIMSI());

    Serial.print(F("Attach Network..."));
    while (!BC95.attachNetwork()) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(F("\nNB-IOT attached!"));
    Serial.print(F("RSSI: "));
    Serial.println(BC95.getSignalStrength());
    Serial.print(F("IPAddress: "));
    Serial.println(BC95.getIPAddress());

    mg.init(APPID, KEY, SECRET);
    mg.begin(5555);
}

void loop() {
    mSerial.print(F("Sent Signal Strength: "));
    Serial.println(BC95.getSignalStrength());
    mg.publish("/nbiot/rssi", BC95.getSignalStrength());
    mg.loop();
    delay(5000);
}
```
