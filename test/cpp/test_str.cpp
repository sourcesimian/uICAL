#include "../catch.hpp"

#include "uICAL.h"

#include "uICAL/vevent.h"
#include "uICAL/vlinestream.h"
#include "uICAL/vline.h"
#include "uICAL/vobjectstream.h"
#include "uICAL/vobject.h"

#include <iostream>
#include <fstream>
#include <list>

static const char* vtimezone1 =
    "BEGIN:VTIMEZONE\n"
    "TZID:America/New_York\n"
    "X-LIC-LOCATION:America/New_York\n"
    "BEGIN:STANDARD\n"
    "TZOFFSETFROM:-0500\n"
    "TZOFFSETTO:-0500\n"
    "TZNAME:EST\n"
    "DTSTART:19700101T000000\n"
    "END:STANDARD\n"
    "END:VTIMEZONE\n";

static const char* event1 =
    "BEGIN:VEVENT\n"
    "DTSTART;TZID=America/New_York:20190917T103000\n"
    "DTEND;TZID=America/New_York:20190917T104000\n"
    "RRULE:FREQ=MONTHLY;WKST=MO;UNTIL=20201231T215959Z;BYMONTHDAY=17\n"
    "DTSTAMP:20191105T175555Z\n"
    "CREATED:20190819T210313Z\n"
    "DESCRIPTION:\n"
    "LAST-MODIFIED:20190914T102748Z\n"
    "LOCATION:\n"
    "SEQUENCE:3\n"
    "STATUS:CONFIRMED\n"
    "SUMMARY:Irrigation Beds\n"
    "TRANSP:OPAQUE\n"
    "END:VEVENT\n";


TEST_CASE("STR::line", "[uICAL][STR]") {
    const char* rrule = "RRULE:FREQ=DAILY;INTERVAL=1;COUNT=4;WKST=MO";
    auto line = uICAL::new_ptr<uICAL::VLine>(rrule);

    REQUIRE(line->as_str() == rrule);
}

TEST_CASE("STR::obj", "[uICAL][STR]") {

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    {
        std::stringstream input(vtimezone1);
        uICAL::istream_stl ical(input);

        uICAL::VLineStream lines(ical);
        uICAL::VObjectStream stm(lines);
        uICAL::VObject_ptr obj = stm.nextObject(true);

        REQUIRE(obj->as_str() == vtimezone1);
        tzmap->add(obj);
    }

    REQUIRE(tzmap->as_str() == "America/New_York : EST : -0500\n");

    {
        std::stringstream input(event1);
        uICAL::istream_stl ical(input);

        uICAL::VLineStream lines(ical);
        uICAL::VObjectStream stm(lines);
        uICAL::VObject_ptr obj = stm.nextObject(true);

        REQUIRE(obj->as_str() == event1);

        uICAL::VEvent_ptr event = uICAL::new_ptr<uICAL::VEvent>(obj, tzmap);

        REQUIRE(event->as_str() ==
            "VEVENT: Irrigation Beds\n"
            " - start: 20190917T103000EST\n"
            " - end: 20190917T104000EST\n"
            " - rrule: RRULE:FREQ=MONTHLY;INTERVAL=1;WKST=MO;UNTIL=20201231T215959Z;BYMONTHDAY=17\n"
        );
    }
}
