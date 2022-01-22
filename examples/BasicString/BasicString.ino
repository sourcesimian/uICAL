#include <uICAL.h>            // Requires: uICAL library

void setup () {
    Serial.begin(115200);
    for (int i = 0; i < 30; i++) {
        Serial.print('.');
        delay(100);
    }
    Serial.println("");
}

String ical(R"(BEGIN:VCALENDAR
VERSION:2.0
CALSCALE:GREGORIAN
METHOD:PUBLISH
X-WR-CALNAME:Home
X-WR-TIMEZONE:America/New_York
BEGIN:VTIMEZONE
TZID:America/New_York
X-LIC-LOCATION:America/New_York
BEGIN:STANDARD
TZOFFSETFROM:-0500
TZOFFSETTO:-0500
TZNAME:EST
DTSTART:19700101T000000
END:STANDARD
END:VTIMEZONE
BEGIN:VEVENT
DTSTART;TZID=America/New_York:20190917T102000
DTEND;TZID=America/New_York:20190917T112203
RRULE:FREQ=WEEKLY;WKST=MO;COUNT=13;INTERVAL=2;BYDAY=SA,TH,TU
DTSTAMP:20191105T175555Z
CREATED:20190819T210234Z
DESCRIPTION:
LAST-MODIFIED:20190914T102649Z
LOCATION:
SEQUENCE:2
STATUS:CONFIRMED
SUMMARY:Irrigation Back
TRANSP:OPAQUE
END:VEVENT
END:VCALENDAR
)");

void loop() {
    Serial.println("# Parsing ICAL:");
    Serial.println(ical);

    uICAL::Calendar_ptr cal = nullptr;
    try {
        uICAL::istream_String istm(ical);  // Use uICAL::istream_Stream to adapt Stream objects
        cal = uICAL::Calendar::load(istm);
    } catch (uICAL::Error e) {
        Serial.println("# Error: " + e.message);
        while(true) {}  // Wait forever
    }
    uICAL::DateTime begin("20190917T000000Z");
    uICAL::DateTime end("20190922T000000Z");

    Serial.println("# From: " + begin.as_str());
    Serial.println("# To: " + end.as_str());

    auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal, begin, end);

    Serial.println("# Events:");
    while(calIt->next()) {
        uICAL::CalendarEntry_ptr entry = calIt->current();
        Serial.println(entry->as_str());
    }

    Serial.println("# Done");
    while(true) {}  // Wait forever
}
