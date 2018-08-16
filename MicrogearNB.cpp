/*
NETPIE NB-IOT library for Arduino.
Author: Chavee Issariyapat
E-mail: chavee@nexpie.com

This software is released under the MIT License.
*/

#include "BC95.h"
#include "BC95Udp.h"
#include "Dns.h"
#include "MicrogearNB.h";

Microgear::Microgear(UDP *_udp) {
    udp = _udp;
    ((BC95UDP *)udp)->setExternalBuffer(buffer, DATA_BUFFER_SIZE);
}

#if MICROGEARNB_USE_EXTERNAL_BUFFER == 1
void Microgear::setExternalBuffer(char *sbuff, size_t sbuffsize) {
    buffer = sbuff;
    buffersize = sbuffsize;
    ((BC95UDP *)udp)->setExternalBuffer(buffer, buffersize);
}
#endif

void Microgear::init(char *_appid, char *_key, char *_secret) {
    appid = _appid;
    key = _key;
    secret = _secret;
}

void Microgear::begin() {
    begin(5555);
}

void Microgear::begin(uint16_t port) {
    DNSClient *dns = new DNSClient();
    dns->begin();
    dns->getHostByName(GWHOST, gwaddr);
    delete dns;

    udp->begin(port);
    coap = new Coap(*udp);
}

void Microgear::loop() {
    coap->loop();
}

void Microgear::coapSend(char *buffer, char* payload) {
    coap->put(gwaddr, GWPORT, buffer, payload);
}

void Microgear::publish(char *topic, int value) {
    itoa(value, buffer+buffersize-8, 10);
    publish(topic, buffer+buffersize-8);
}

void Microgear::publish(char *topic, char *payload) {
    strcpy_P(buffer, PSTR("topic/"));
    strcat(buffer, appid);
    strcat(buffer, topic);
    strcat_P(buffer, PSTR("?auth="));
    strcat(buffer, key);
    strcat(buffer, ":");
    strcat(buffer, secret);
    coapSend(buffer, payload);
}

void Microgear::chat(char *alias, char *payload) {
    strcpy_P(buffer, PSTR("microgear/"));
    strcat(buffer, appid);
    strcat(buffer, "/");
    strcat(buffer, alias);
    strcat_P(buffer, PSTR("?auth="));
    strcat(buffer, key);
    strcat(buffer, ":");
    strcat(buffer, secret);
    coapSend(buffer, payload);
}

void Microgear::writeFeed(char *feedid, char *payload) {
    char *p;

    strcpy_P(buffer, PSTR("feed/"));
    strcat(buffer, feedid);
    strcat_P(buffer, PSTR("?auth="));
    strcat(buffer, key);
    strcat(buffer, ":");
    strcat(buffer, secret);

    p = buffer + strlen(buffer) + 1;
    strcpy_P(p, PSTR("data="));
    strcat(p, payload);
    coapSend(buffer, p);
}

void Microgear::writeFeed(char *feedid, char *payload, char* apikey) {
    char *p;

    strcpy_P(buffer, PSTR("feed/"));
    strcat(buffer, feedid);
    strcat_P(buffer, PSTR("?apikey="));
    strcat(buffer, apikey);

    p = buffer + strlen(buffer) + 1;
    strcpy_P(p, PSTR("data="));
    strcat(p, payload);

    coapSend(buffer, payload);
}

void Microgear::pushOwner(char *text) {
    strcpy_P(buffer, PSTR("push/owner?auth="));
    strcat(buffer, key);
    strcat(buffer, ":");
    strcat(buffer, secret);
    coapSend(buffer, text);
}
