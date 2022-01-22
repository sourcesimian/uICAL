#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include <WiFiUDP.h>
#include <EasyNTPClient.h>

#include <uICAL.h>

const char* WIFI_SSID = "...";
const char* WIFI_PASS = "...";

const char* ICAL_URL = "...";

const char* NTP_HOST = "pool.ntp.org";
const int NTP_OFFSET = 0;  // UTC+0
const int NTP_PORT = 123;

WiFiUDP g_ntpUDP;
EasyNTPClient g_ntpClient(g_ntpUDP, NTP_HOST, NTP_OFFSET);


void setup() {
    // * Setup Serial port
    Serial.begin(115200);
    delay(1000);
    Serial.println("Hello");

    // * Connect to WiFi
    Serial.print("Connecting to %s .", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.isConnected() == false) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");

    // * Get current time
    g_ntpUDP.begin(NTP_PORT);

    Serial.print("Getting time from %s .", NTP_HOST);
    while (g_ntpClient.getUnixTime() == 0) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
}


void stop() {
    Serial.println("END", ICAL_URL);
    while(true) {
        delay(1000);
    }
}


void loop() {
    Serial.println("Fetching: %s", ICAL_URL);

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;
    if (! https.begin(*client, ICAL_URL)) {
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
    uICAL::DateTime calEnd(now + (86400 * 7));

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


