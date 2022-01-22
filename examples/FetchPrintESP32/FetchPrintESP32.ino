#include <WiFi.h>
#include <HTTPClient.h>

#include <WiFiUDP.h>
#include <EasyNTPClient.h>    // Requires: EasyNTPClient library

#include <uICAL.h>            // Requires: uICAL library

const char* NTP_HOST = "pool.ntp.org";

const int NTP_OFFSET = 0;  // UTC+0
const int NTP_PORT = 123;

WiFiUDP g_ntpUDP;
EasyNTPClient g_ntpClient(g_ntpUDP, NTP_HOST, NTP_OFFSET);

String wifi_ssid = "Rincewind";
String wifi_pass = "TYGERBERG";
String ical_url = "https://calendar.google.com/calendar/ical/fbu3tbt6va7n7007pgfqr3h068%40group.calendar.google.com/private-f9d1237a62840f0a0cd722a00eb07f2e/basic.ics";
String root_ca = "";
String event_sumary = "Cat Blanket";

void setup() {
    Serial.begin(115200);

    Serial.print("# Connecting to " + wifi_ssid + " .");
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());

    while (WiFi.isConnected() == false) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");

    g_ntpUDP.begin(NTP_PORT);

    while (g_ntpClient.getUnixTime() == 0) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
}


void stop() {
    Serial.println("# Done");
    while(true) {
        delay(1000);
    }
}


void loop() {
    Serial.println("# Fetching: " + ical_url);

//    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
//    client->setInsecure();

    HTTPClient https;
    if (! https.begin(ical_url, root_ca.c_str())) {
        stop();
    }
    int httpCode = https.GET();

    if (httpCode <= 0) {
        stop();
    }

//    Serial.println(https.getStream().readString());
    String length = https.getStream().readStringUntil('\n');

    uICAL::Calendar_ptr cal = nullptr;
    try {
        uICAL::istream_Stream istm(https.getStream());
    Serial.println("5");
        cal = uICAL::Calendar::load(istm, [=](const uICAL::VEvent& event){
            return event.summary == event_sumary;
        });
    Serial.println("6");
    }
    catch (uICAL::Error ex) {
        Serial.println("# Error: " + ex.message + " ! Failed loading calendar");
        stop();
    }
    https.end();
    Serial.println("7");

    unsigned now = g_ntpClient.getUnixTime();

    uICAL::DateTime calBegin(now);
    uICAL::DateTime calEnd(now + (86400 * 7));

    uICAL::CalendarIter_ptr calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal, calBegin, calEnd);

    while (calIt->next()) {
        uICAL::CalendarEntry_ptr entry = calIt->current();
        Serial.println("Event @ " + entry->start().as_str() +" -> " + entry->end().as_str() + " : " + entry->summary());
    }

    stop();
}
