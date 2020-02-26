/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/

#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClientSecureBearSSL.h>
#else
    #include <Arduino.h>
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

unsigned startTimeStamp;

uICALRelay relay;


void setup_serial() {
    Serial.begin(115200);
    for (int i=0; i<60; i++) {
        relay.statusLedToggle();
        delay(100);
    }
    relay.statusLed(true);
    delay(4000);
    relay.statusLed(false);
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
        relay.statusLedToggle();
        delay(500);
    }
    Serial.println(" Connected");
    relay.statusLed(false);
}


unsigned getUnixTimeStamp() {
    return g_ntpClient.getUnixTime();
}


void setup_ntp() {
    ntpUDP.begin(123);

    Serial.print("Waiting for NTP ");
    while((startTimeStamp = getUnixTimeStamp()) == 0) {
        Serial.print(".");
        relay.statusLedToggle();
        delay(1000);
    }
    Serial.println((String)" Done: " + startTimeStamp);
    relay.statusLed(false);
}

#if defined(ARDUINO_ARCH_ESP8266)

    void updateCalendar(const char* url, std::function<void (Stream&)> setStream) {
        String payload;
        relay.statusLed(true);

        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        //client->setFingerprint(fp);

        //Serial.print("[HTTPS] begin...\n");
        HTTPClient https;
        if (https.begin(*client, url)) {
            int httpCode = https.GET();

            if (httpCode > 0) {
                //Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
                String length = https.getStream().readStringUntil('\n');
                setStream(https.getStream());
            } else {
                Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            }
            https.end();
        } else {
            Serial.printf("[HTTPS] Unable to connect\n");
        }
        relay.statusLed(false);
    }

#else

    void updateCalendar(const char* url, std::function<void (Stream&)> setStream) {
        relay.statusLed(true);
        HTTPClient http;
        http.begin(url);
        http.getStream().flush();
        int httpCode = http.GET();
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        if (httpCode > 0) {
            String length = http.getStream().readStringUntil('\n');
            setStream(http.getStream());
        }
        relay.statusLed(false);
    }

#endif


void setup() {
    relay.begin();

    setup_serial();
    setup_wifi();
    setup_ntp();
}

void relay_updateCalendar(Stream& stm) {
    relay.updateCalendar(stm);
}

void readStream(Stream& stm) {
    size_t len = 81;
    char buf[len];
    
    for (;;) {
        size_t read = stm.readBytesUntil('\r', buf, len-1);
        if (read > 0) {
            buf[read] = 0;
            Serial.print(buf);
        }
        else {
            break;
        }
    }
}

void loop() {
    Serial.println("Loop: BEGIN");
    
    updateCalendar(relay.icalURL, relay_updateCalendar);
    unsigned unixTimeStamp = getUnixTimeStamp();
    relay.wait(relay.updateGates(unixTimeStamp));

    Serial.println((String)"Uptime: " + (getUnixTimeStamp() -   startTimeStamp) + "s");
}
