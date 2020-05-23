/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/

#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClientSecureBearSSL.h>
    #include <ESP8266WebServer.h>
#else
    #include <Arduino.h>
    #include <WiFi.h>
    #include <HTTPClient.h>
    #include <WebServer.h>
#endif

#include <WiFiUDP.h>
#include <EasyNTPClient.h>

#include "uICALRelay.h"

struct WiFi_config_t {
    const char* ssid;
    const char* pass;
};

#include "uICALRelay_config.h"

#define NTP_HOST "pool.ntp.org"

WiFiUDP ntpUDP;
EasyNTPClient g_ntpClient(ntpUDP, NTP_HOST, 0); // UTC
unsigned startTimeStamp;

void updateCalendar(const char* url, const char* hostFingerprint, std::function<void (Stream&)> processStream);
unsigned getUnixTimeStamp();
uICALRelay relay(uICALRelay_config, updateCalendar, getUnixTimeStamp);


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
    WiFi.begin((char *)WiFi_config.ssid, (char *)WiFi_config.pass);
    Serial.print(String("Waiting for WiFi (") + WiFi_config.ssid + ") ");
    while (!wifiIsUp()) {
        Serial.print(".");
        relay.statusLedToggle();
        delay(500);
    }
    Serial.print(" Connected [");
    Serial.print(WiFi.localIP());
    Serial.println("]");
    relay.statusLed(false);
}


void setup_wifi_ap() {
    IPAddress local_ip, gateway, subnet;

    local_ip.fromString("10.0.0.7");
    gateway.fromString("10.0.0.1");
    subnet.fromString("255.255.255.0");

    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP("uICAL_Relay", NULL, 6, 0, 0);

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
    ESP8266WebServer config_server(80);
#else
    WebServer config_server(80);
#endif


void setup_config_api() {
    config_server.on("/", HTTP_GET, [&]() {
            config_server.send(200, "text/html",
                "Welcome to REST Web Server");
        });
}


#if defined(ARDUINO_ARCH_ESP8266)
    void updateCalendar(const char* url, const char* hostFingerprint, std::function<void (Stream&)> processStream) {
        String payload;

        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        if (hostFingerprint) {
            client->setFingerprint(hostFingerprint);
        }

        HTTPClient https;
        if (https.begin(*client, url)) {
            int httpCode = https.GET();

            if (httpCode > 0) {
                String length = https.getStream().readStringUntil('\n');
                processStream(https.getStream());
            } else {
                Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            }
            https.end();
        } else {
            Serial.printf("[HTTPS] Unable to connect\n");
        }
    }

#else

    void updateCalendar(const char* url, const char* hostFingerprint, std::function<void (Stream&)> processStream) {
        relay.statusLed(true);
        HTTPClient https;
        https.begin(url);
        https.getStream().flush();
        int httpCode = https.GET();
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        if (httpCode > 0) {
            String length = https.getStream().readStringUntil('\n');
            processStream(https.getStream());
        } else {
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
        relay.statusLed(false);
    }
#endif


unsigned getUnixTimeStamp() {
    return g_ntpClient.getUnixTime();
}


void setup() {
    relay.begin();

    setup_serial();
    setup_wifi();
    setup_ntp();

    setup_config_api();
}


void loop() {
    try {
        relay.handleRelays();
    }
    catch(uICAL::Error e) {
        Serial.println((String)"EXCEPTION: " + e.message);
        delay(1000);
    }

    config_server.handleClient();
}
