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

#include <uICALRelay.h>
#include <ConfigWiFiAP.h>

#include "ButtonMonitor.h"
#include "LedFlash.h"

#if defined(DEBUG)
    #define LOG(...) Serial.println(__VA_ARGS__)
#else
    #define LOG(...) while(false){}
#endif

/*---------------------------------------------------------------------------*/
struct device_gate_pin_t {
    const char* id;
    const uint8_t pin;
    const bool active_high;
};

struct device_config_t {
    uint8_t statusLedPin;
    uint8_t pushButtonPin;
    device_gate_pin_t* gatePins;
};

#include "device_config.h"

/*---------------------------------------------------------------------------*/
enum loop_mode_t {
    WIFI_INIT,
    WIFI_SETUP,
    NTP_INIT,
    NTP_SETUP,
    RUN_INIT,
    RUN,
    CONFIG_INIT,
    CONFIG,
};

loop_mode_t g_loopMode;

WiFiUDP g_ntpUDP;
EasyNTPClient g_ntpClient(g_ntpUDP, ntp_host, 0); // UTC

ConfigWiFiAP g_config(config_ap, config_ap_items, SPIFFS, WiFi);

void update_calendar(String& url, String& hostFingerprint, std::function<void (Stream&)> processStream);
unsigned get_unix_timestamp();
void set_gate(const char* id, bool state);

uICALRelay g_relay(update_calendar, get_unix_timestamp, set_gate);
ButtonMonitor g_button(device_config.pushButtonPin);
LedFlash g_led(device_config.statusLedPin);
int g_configMillis;


/*---------------------------------------------------------------------------*/
unsigned get_unix_timestamp() {
    return g_ntpClient.getUnixTime();
}

/*---------------------------------------------------------------------------*/
#if defined(ARDUINO_ARCH_ESP8266)
    void update_calendar(String& url, String& hostFingerprint, std::function<void (Stream&)> processStream) {
        bool success = false;

        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        if (!hostFingerprint.isEmpty()) {
            client->setFingerprint(hostFingerprint.c_str());
        }
        else {
            client->setInsecure();
        }

        HTTPClient https;
        if (https.begin(*client, url)) {
            int httpCode = https.GET();

            if (httpCode > 0) {
                String length = https.getStream().readStringUntil('\n');
                processStream(https.getStream());
                success = true;
            } else {
                LOG(String("[HTTPS] GET... failed, error: ") + https.errorToString(httpCode));
            }
            https.end();
        } else {
            LOG("[HTTPS] Unable to connect");
        }

        if (success) {
            g_led.state(false);
        }
        else {
            g_led.flash(50, 9950);
        }
    }

#else

    void update_calendar(String& url, String& hostFingerprint, std::function<void (Stream&)> processStream) {
        bool success = false;

        HTTPClient https;
        https.begin(url);
        https.getStream().flush();
        int httpCode = https.GET();
        LOG(String("[HTTPS] GET... code: ") + httpCode);
        if (httpCode > 0) {
            String length = https.getStream().readStringUntil('\n');
            processStream(https.getStream());
            success = true;
        } else {
            LOG(String("[HTTPS] GET... failed, error: ") + https.errorToString(httpCode);
        }
        https.end();

        if (success) {
            g_led.state(false);
        }
        else {
            g_led.flash(50, 9950);
        }
    }
#endif

/*---------------------------------------------------------------------------*/
void set_gate(const char* id, bool state) {
    for (int i=0; i<100; i++) {
        if (!device_config.gatePins[i].id) break;

        if (id == device_config.gatePins[i].id) {
            digitalWrite(device_config.gatePins[i].pin,
                         device_config.gatePins[i].active_high == state ? HIGH : LOW);
            break;
        }
    }
}

/*---------------------------------------------------------------------------*/
void setup_io_pins() {
    for (int i=0; i<100; i++) {
        if (!device_config.gatePins[i].id) break;

        digitalWrite(device_config.gatePins[i].pin,
                     !device_config.gatePins[i].active_high ? HIGH : LOW);
        pinMode(device_config.gatePins[i].pin, OUTPUT);
    }
}

/*---------------------------------------------------------------------------*/
void config_relay() {
    g_relay.config(g_config.getConfig("icalurl"),
                   g_config.getConfig("poll").toInt(),
                   g_config.getConfig("fingerprint"));
    
    for (int i=0; i<100; i++) {
        if (!device_config.gatePins[i].id) break;
        
        String name = g_config.getConfig(device_config.gatePins[i].id);
        g_relay.configGate(device_config.gatePins[i].id, name);
    }
}

/*---------------------------------------------------------------------------*/
void setup() {
    setup_io_pins();

    Serial.begin(115200);

    SPIFFS.begin();

    g_loopMode = WIFI_INIT;
}

/*---------------------------------------------------------------------------*/
void loop() {
    int button = g_button.getPressed();
    g_led.handle();

    if (button < -10000 && g_loopMode != CONFIG && g_loopMode != CONFIG_INIT) {
        g_led.state(true);
        g_loopMode = CONFIG_INIT;
    }
    else if (button > 3000 && button < 7000) {
        LOG("Toggling gate0");
        digitalWrite(device_config.gatePins[0].pin, !digitalRead(device_config.gatePins[0].pin));
    }

    switch (g_loopMode) {
        case WIFI_INIT:
            WiFi.disconnect();
            {
                String ssid = g_config.getConfig("wifissid");
                if (ssid.isEmpty()) {
                    g_loopMode = CONFIG_INIT;
                    break;
                }
                LOG(String("WIFI_SETUP mode (") + ssid + ")");
                WiFi.mode(WIFI_STA);
                WiFi.begin(ssid.c_str(), g_config.getConfig("wifipass").c_str());
            }
            g_led.flash(300, 1700);
            g_loopMode = WIFI_SETUP;
            break;

        case WIFI_SETUP:
            if (!WiFi.isConnected()) {
                break;
            }
            LOG(String("WIFI Connected [") + WiFi.localIP().toString() + "]");
            g_led.state(false);
            g_loopMode = NTP_INIT;
            break;

        case NTP_INIT:
            LOG(String("NTP_SETUP mode (") + ntp_host + ")");
            g_ntpUDP.begin(123);
            g_led.flash(300, 2700);
            g_loopMode = NTP_SETUP;
            break;

        case NTP_SETUP:
            if (!WiFi.isConnected()) {
                g_loopMode = WIFI_INIT;
                break;
            }
            if (g_ntpClient.getUnixTime() == 0) {
                break;
            }
            g_led.state(false);
            g_loopMode = RUN_INIT;
            break;

        case RUN_INIT:
            LOG("RUN mode");
            config_relay();
            g_led.state(false);
            g_loopMode = RUN;
            break;

        case RUN:
            if (button > 100 && button < 3000) {
                LOG("Forcing update");
                g_led.state(true);
                g_relay.forceUpdate();
            }
            if (!WiFi.isConnected()) {
                g_loopMode = WIFI_INIT;
                break;
            }
            try {
                g_relay.handleRelays();
            }
            catch(uICAL::Error e) {
                LOG((String)"EXCEPTION: " + e.message);
                delay(1000);
            }
            break;

        case CONFIG_INIT:
            LOG("Entering CONFIG mode");
            if(!g_config.start()) {
                LOG("Failed to enter CONFIG mode");
                g_loopMode = WIFI_INIT;
                break;
            }
            g_led.flash(200, 100);
            g_configMillis = millis();
            g_loopMode = CONFIG;
            break;

        case CONFIG:
            if ((button > 100 && button < 3000) || (millis() - g_configMillis) > 300000) {
                g_config.stop();
                g_loopMode = WIFI_INIT;
                break;
            }
            g_config.handle();
            break;
    }
}

/*---------------------------------------------------------------------------*/
