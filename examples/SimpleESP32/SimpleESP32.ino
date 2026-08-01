#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>
#include <uICAL.h>

// WiFi credentials
const char* WIFI_SSID = "...";
const char* WIFI_PASS = "...";

// Public holidays in the USA (provided by Google) as an example
const char* ICAL_URL = "https://calendar.google.com/calendar/ical/en.usa.official%23holiday%40group.v.calendar.google.com/public/basic.ics";

// NTP settings
const char* NTP_HOST = "pool.ntp.org";
const long GMT_OFFSET_SEC = 0;      // UTC+0
const int DAYLIGHT_OFFSET_SEC = 0;

void connectWiFi() {
    Serial.printf("Connecting to %s ", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected");
}

time_t waitForTime() {
    Serial.printf("Getting time from %s ", NTP_HOST);
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_HOST);

    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println(" done");
    return now;
}

void halt() {
    while (true) {
        delay(1000);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Running setup...");

    connectWiFi();
    waitForTime();
}

void loop() {
    Serial.printf("Fetching: %s\n", ICAL_URL);

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;
    if (!https.begin(client, ICAL_URL)) {
        Serial.println("Unable to connect");
        halt();
    }

    int httpCode = https.GET();
    if (httpCode <= 0) {
        Serial.printf("GET failed: %s\n", https.errorToString(httpCode).c_str());
        https.end();
        halt();
    }

    uICAL::Calendar_ptr cal = nullptr;
    try {
        uICAL::istream_Stream istm(https.getStream());
        cal = uICAL::Calendar::load(istm);
    }
    catch (uICAL::Error& ex) {
        Serial.printf("%s: Failed loading calendar\n", ex.message.c_str());
        https.end();
        halt();
    }
    https.end();

    time_t now = time(nullptr);

    uICAL::DateTime calBegin(now);
    uICAL::DateTime calEnd(now + 86400);

    uICAL::CalendarIter_ptr calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal, calBegin, calEnd);

    while (calIt->next()) {
        uICAL::CalendarEntry_ptr entry = calIt->current();
        Serial.printf("Event @ %s -> %s : %s\n",
                       entry->start().as_str().c_str(),
                       entry->end().as_str().c_str(),
                       entry->summary().c_str());
    }

    halt();
}
