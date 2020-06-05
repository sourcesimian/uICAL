#include "../catch.hpp"

#include "uICAL.h"

#include "uICAL/vline.h"
#include "uICAL/vobjectstream.h"
#include "uICAL/vobject.h"

TEST_CASE("THROW::vline", "[uICAL][THROW]") {
    REQUIRE_THROWS_WITH(uICAL::VLine(""), "ParseError: VLINE is empty");
    REQUIRE_THROWS_WITH(uICAL::VLine("DTSTART"), "ParseError: VLINE does not have a ':' \"DTSTART\"");

    // REQUIRE_THROWS_WITH(uICAL::VLine("FOO:FISH=fish;CHIPS=chips;SPAM"), "ParesError: Bad param: \"SPAM\"");

}

TEST_CASE("THROW::vobject", "[uICAL][THROW]") {
    {
        std::stringstream input("BEGIN:VEVENT");
        uICAL::istream_stl ical(input);

        uICAL::VLineStream lines(ical);
        uICAL::VObjectStream stm(lines);
        REQUIRE_THROWS_WITH(stm.nextObject(true), "ParseError: Parse error, unexpected end of ICAL");
    }

    {
        std::stringstream input("BEGIN:FOO\nBAR:one\nEND:BAR\n");
        uICAL::istream_stl ical(input);

        uICAL::VLineStream lines(ical);
        uICAL::VObjectStream stm(lines);
        REQUIRE_THROWS_WITH(stm.nextObject(true), "ParseError: Mismatch for: \"FOO\": END:BAR");
    }

    {
        std::stringstream input("FOO:BAR");
        uICAL::istream_stl ical(input);

        uICAL::VLineStream lines(ical);
        uICAL::VObjectStream stm(lines);
        REQUIRE_THROWS_WITH(stm.nextObject(true), "ParseError: Parse error, expected BEGIN: FOO:BAR");
    }

    {
        std::stringstream input("");
        uICAL::istream_stl ical(input);

        uICAL::VLineStream lines(ical);
        uICAL::VObjectStream stm(lines);
        REQUIRE_THROWS_WITH(stm.nextObject(true), "ParseError: Parse error, empty stream");
    }

}
