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
#include <DNSServer.h>
#include <FS.h>

#include "uICALRelay.h"
#include "ButtonMonitor.h"
#include "LedFlash.h"
#include "ConfigWiFiAP.h"

#define _TRACE_ Serial.println(String("") + "main" + ": (" + __LINE__ + ")")

/*---------------------------------------------------------------------------*/
struct uICALRelay_gatePin_t {
    const char* id;
    const uint8_t pin;
};

struct uICALRelay_config_t {
    uint8_t statusLedPin;
    uint8_t pushButtonPin;
    uICALRelay_gatePin_t* gatePins;
};


#include "uICALRelay_config.h"

enum loop_mode_t {
    INIT_TEST,
    TEST,
    INIT_RUN,
    RUN,
    INIT_CONFIG,
    CONFIG,
};


/*---------------------------------------------------------------------------*/
const char * NTP_HOST = "pool.ntp.org";

/*---------------------------------------------------------------------------*/
loop_mode_t loop_mode;

WiFiUDP g_ntpUDP;
EasyNTPClient g_ntpClient(g_ntpUDP, NTP_HOST, 0); // UTC

ConfigWiFiAP g_config(config_ap, config_ap_items, SPIFFS, WiFi);

void update_calendar(String& url, String& hostFingerprint, std::function<void (Stream&)> processStream);
unsigned get_unix_timestamp();
void set_gate(const char* id, bool state);

uICALRelay g_relay(update_calendar, get_unix_timestamp, set_gate);
ButtonMonitor g_button(uICALRelay_config.pushButtonPin);
LedFlash g_led(uICALRelay_config.statusLedPin);

void setup_io_pins() {
    // Setup gate pins
    for (int i=0; i<100; i++) {
        if (!uICALRelay_config.gatePins[i].id) {
            break;
        }
        digitalWrite(uICALRelay_config.gatePins[i].pin, LOW);
        pinMode(uICALRelay_config.gatePins[i].pin, OUTPUT);
    }
}

/*---------------------------------------------------------------------------*/
void set_gate(const char* id, bool state) {
    for (int i=0; i<100; i++) {
        if (!uICALRelay_config.gatePins[i].id) {
            break;
        }
        if (id == uICALRelay_config.gatePins[i].id) {
            digitalWrite(uICALRelay_config.gatePins[i].pin, !state);
            break;
        }
    }
}

/*---------------------------------------------------------------------------*/
void setup_serial() {
    Serial.begin(115200);
    for (int i=0; i<60; i++) {
        g_led.toggle();
        delay(100);
    }
    g_led.state(true);
    delay(4000);
    g_led.state(false);
    Serial.println("~~~");
}

void setup_wifi() {
    WiFi.disconnect();

    String ssid = g_config.getConfig("wifissid");
    WiFi.begin(ssid.c_str(), g_config.getConfig("wifipass").c_str());
    Serial.print(String("Waiting for WiFi (") + ssid + ") ");
    while (!WiFi.isConnected()) {
        Serial.print(".");
        g_led.toggle();
        delay(300);
    }
    Serial.print(" Connected [");
    Serial.print(WiFi.localIP());
    Serial.println("]");
    g_led.state(false);
}

unsigned get_unix_timestamp() {
    return g_ntpClient.getUnixTime();
}

void setup_ntp() {
    unsigned startTimeStamp;

    g_ntpUDP.begin(123);

    Serial.print("Waiting for NTP ");
    while ((startTimeStamp = get_unix_timestamp()) == 0) {
        Serial.print(".");
        g_led.toggle();
        delay(1000);
    }
    Serial.println((String)" Done: " + startTimeStamp);
    g_led.state(false);
}



#if defined(ARDUINO_ARCH_ESP8266)
    void update_calendar(String& url, String& hostFingerprint, std::function<void (Stream&)> processStream) {
        g_led.state(true);

        String payload;
        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        if (!hostFingerprint.isEmpty()) {
            client->setFingerprint(hostFingerprint.c_str());
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

        g_led.state(false);
    }

#else

    void update_calendar(String& url, String& hostFingerprint, std::function<void (Stream&)> processStream) {
        g_led.state(true);

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

        g_led.state(false);
    }
#endif

void config_relay() {
    String url = g_config.getConfig("icalurl");
    int poll = g_config.getConfig("poll").toInt();
    String fingerprint = g_config.getConfig("fingerprint");
    g_relay.config(url, poll, fingerprint);
    
    for (int i=0; i<100; i++) {
        if (!uICALRelay_config.gatePins[i].id) {
            break;
        }
        
        String name = g_config.getConfig(uICALRelay_config.gatePins[i].id);
        g_relay.configGate(uICALRelay_config.gatePins[i].id, name);
    }
}

void setup() {
    setup_io_pins();
    setup_serial();
    SPIFFS.begin();

    loop_mode = INIT_RUN;

    Serial.println("loop");
}


void loop() {
    int button = g_button.getPressed();
    g_led.handle();

    switch (loop_mode) {
        case INIT_TEST:
            g_led.flash(2000);
            loop_mode = TEST;
            break;

        case TEST:
            break;

        case INIT_RUN:
            Serial.println("Setup run mode");
            setup_wifi();
            setup_ntp();
            config_relay();
            g_led.state(false);
            loop_mode = RUN;
            break;

        case RUN:
            if (button < -10000) {
                loop_mode = INIT_CONFIG;
                break;
            }
            if (button > 500 && button < 3000) {
                g_relay.forceUpdate();
            }
            try {
                g_relay.handleRelays();
            }
            catch(uICAL::Error e) {
                Serial.println((String)"EXCEPTION: " + e.message);
                delay(1000);
            }
            break;

        case INIT_CONFIG:
            Serial.println("Setup config mode");
            g_config.begin();
            g_led.flash(300, 2700);
            loop_mode = CONFIG;
            break;

        case CONFIG:
            if (button > 500 && button < 3000) {
                loop_mode = INIT_RUN;
                break;
            }
            g_config.handle();
            break;
    }
}
