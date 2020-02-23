/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
#else
    #include <WiFi.h>
    #include <HTTPClient.h>
#endif

#include <WiFiUDP.h>
#include <EasyNTPClient.h>

#include "uICALRelay.h"

extern const char* g_wifi_ssid;
extern const char* g_wifi_pass;

#define NTP_HOST "pool.ntp.org"

WiFiUDP ntpUDP;
EasyNTPClient g_ntpClient(ntpUDP, NTP_HOST, 0); // UTC

unsigned getUnixTimeStamp();
String httpGet(const char* url);

uICALRelay relay(getUnixTimeStamp, httpGet);


void setup_serial() {
    Serial.begin(9600);
    delay(4000);
    Serial.println("~~~");
}


bool wifiIsUp() {
    return WiFi.status() == WL_CONNECTED;
}


void setup_wifi() {
    WiFi.begin((char *)g_wifi_ssid, (char *)g_wifi_pass);
    Serial.print(String("Waiting for WiFi (") + g_wifi_ssid + ") ");
    while (!wifiIsUp()) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" Connected");
}


unsigned getUnixTimeStamp() {
    return g_ntpClient.getUnixTime();
}


void setup_ntp() {
    ntpUDP.begin(123);

    Serial.print("Waiting for NTP ");
    while(getUnixTimeStamp() == 0) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" Done");
}


String httpGet(const char* url) {
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode < 0) {
        return String();
    }
    return http.getString();
}


void setup(){
    setup_serial();
    setup_wifi();
    setup_ntp();
}


void loop() {
    relay.wait(relay.loop());
}