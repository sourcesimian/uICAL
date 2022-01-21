#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include <WiFiUDP.h>
#include <EasyNTPClient.h>

#include <uICAL.h>

const char* NTP_HOST = "pool.ntp.org";

const int NTP_OFFSET = 0;  // UTC+0
const int NTP_PORT = 123;

WiFiUDP g_ntpUDP;
EasyNTPClient g_ntpClient(g_ntpUDP, NTP_HOST, NTP_OFFSET);

String wifi_ssid = "...";
String wifi_pass = "...";
String ical_url = "...";

void setup() {
    Serial.begin(115200);

    Serial.print("Connecting to %s .", wifi_ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());

    while (WiFi.isConnected() == false) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");

    g_ntpUDP.begin(NTP_PORT);


    if (g_ntpClient.getUnixTime() == 0) {

}

void stop() {
    Serial.println("END", ical_url);
    while(true) {
        delay(1000);
    }
}


void loop() {


    Serial.println("Fetching: %s", ical_url);

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;
    if (! https.begin(*client, ical_url)) {
        stop();
    }
    int httpCode = https.GET();

    if (httpCode <= 0) {
        stop();
    }

    String length = https.getStream().readStringUntil('\n');

    uICAL::Calendar_ptr cal = nullptr;
    try {
        uICAL::istream_Stream istm(https.getStream());
        cal = uICAL::Calendar::load(istm);
    }
    catch (uICAL::Error ex) {
        Serial.println("%s: %s", ex.message.c_str(), "! Failed loading calendar");
        stop();
    }

    unsigned now = g_ntpClient.getUnixTime();

    uICAL::DateTime calBegin(now);
    uICAL::DateTime calEnd(now + 86400);

    uICAL::CalendarIter_ptr calIt = uICAL::new_ptr<uICAL::CalendarIter>(this->cal, calBegin, calEnd);

    while (calIt->next()) {
        uICAL::CalendarEntry_ptr entry = calIt->current();
        Serial.println("Event @ %s -> %s : %s",
                        entry->start().as_str().c_str(),
                        entry->end().as_str().c_str(),
                        entry->summary().c_str());

    }

    stop();
}


