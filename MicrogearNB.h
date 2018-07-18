/*
NETPIE Microgear NB-IOT library for Arduino.
Author: Chavee Issariyapat
E-mail: chavee@nexpie.com

This software is released under the MIT License.
*/

#ifndef MICROGEARNB_h
#define MICROGEARNB_h

#define MICROGEARNB_USE_EXTERNAL_BUFFER     0
#define DATA_BUFFER_SIZE                   128
#define GWHOST                             "coap.netpie.io"
#define GWPORT                             45683

#include "CoAP.h";

class Microgear {
public:
    BC95UDP *udp;
    Coap *coap;
    Microgear(UDP*);
    void init(char *appid, char *key, char *secret);
    void begin();
    void begin(uint16_t);
    void Microgear::publish(char *topic, int value);
    void publish(char *topic, char *payload);
    void chat(char *alias, char *payload);
    void writeFeed(char *topic, char *payload);
    void writeFeed(char *topic, char *payload, char *apikey);
    void pushOwner(char *text);
    void loop();

    #if MICROGEARNB_USE_EXTERNAL_BUFFER == 1
    char *buffer;
    size_t buffersize = 0;
    void setExternalBuffer(char *extbuff, size_t extbuffsize);
    #else
    char buffer[DATA_BUFFER_SIZE];
    #define buffersize DATA_BUFFER_SIZE
    #endif
private:
    char *appid;
    char *key;
    char *secret;
    IPAddress gwaddr;
    void coapSend(char *buffer, char* payload);
};

#endif
