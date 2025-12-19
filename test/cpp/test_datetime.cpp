#include "../catch.hpp"

#include <cstdio>
#include <iostream>
#include <map>

#include "uICAL.h"

#include "uICAL/datetime.h"

TEST_CASE("DateTime::str", "[uICAL][DateTime]") {
    REQUIRE_THROWS_WITH(uICAL::DateTime("20200605T06493"), "ValueError: Bad datetime: \"20200605T06493\"");
}

TEST_CASE("DateTime::date-only format", "[uICAL][DateTime]") {
    // RFC 5545 allows DATE values without time component for all-day events:
    //   DTSTART;VALUE=DATE:20251217
    // Date-only values are "floating" per RFC 5545 - they represent the same
    // calendar date regardless of timezone, so no 'Z' suffix.
    uICAL::DateTime dt("20251217");
    REQUIRE(dt.as_str() == "20251217T000000");

    // Verify date components
    uICAL::DateStamp ds = dt.datestamp();
    REQUIRE(ds.year == 2025);
    REQUIRE(ds.month == 12);
    REQUIRE(ds.day == 17);
    REQUIRE(ds.hour == 0);
    REQUIRE(ds.minute == 0);
    REQUIRE(ds.second == 0);
}
