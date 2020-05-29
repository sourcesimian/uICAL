/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WebServer.h>

#include <EasyNTPClient.h>

/*---------------------------------------------------------------------------*/
#define S(s) #s
const char* wifi_name = S(WIFI_NAME);
const char* wifi_pass = S(WIFI_PASS);
const char* url_to_fetch = S(URL_TO_FETCH);

/*---------------------------------------------------------------------------*/
void http_get(const char* url, const char* hostFingerprint, std::function<void (Stream&)> process_stream) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    if (hostFingerprint) {
        client->setFingerprint(hostFingerprint);
    }
    else {
        client->setInsecure();
    }

    HTTPClient https;
    if (https.begin(*client, url)) {
        int httpCode = https.GET();

        if (httpCode > 0) {
            process_stream(https.getStream());
        } else {
            Serial.println(String("[HTTPS] GET... failed, error: ") + https.errorToString(httpCode));
        }
        https.end();
    } else {
        Serial.println("[HTTPS] Unable to connect");
    }
}

/*---------------------------------------------------------------------------*/
void setup() {
    Serial.begin(115200);
    delay(3000);

}

/*---------------------------------------------------------------------------*/
void process_stream(Stream& stm) {
    String line;
    Serial.println("");
    Serial.println("Content:");

    while(line = stm.readStringUntil('\n')) {
        Serial.println(line);
    }
}

/*---------------------------------------------------------------------------*/
void loop() {
    const char* fingerprint = "01:23:45:67:89:ab:cd:ef:01:23:45:67:89:ab:ed:ef:01:23:45:67";

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_name, wifi_pass);
    Serial.println(String("Waiting for WiFi (") + wifi_name + ") ");

    while (!WiFi.isConnected()) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" Connected!");
    http_get(url_to_fetch, fingerprint, process_stream);

    Serial.println("");
    Serial.println("Waiting ...");
    delay(10000);
}

/*---------------------------------------------------------------------------*/
