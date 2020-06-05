#include "../catch.hpp"

#include "uICAL.h"

#include "uICAL/vevent.h"

#include <fstream>

TEST_CASE("Calendar::basic", "[uICAL][Calendar]") {
    std::ifstream input(std::string("test/data/ical1.txt"));
    uICAL::istream_stl ical(input);

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    auto cal = uICAL::Calendar::load(ical, tzmap);
    REQUIRE(cal->as_str() == "CALENDAR\n");

    auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal,
        uICAL::DateTime("20191016T102000Z"),
        uICAL::DateTime("20191017T103000EST", tzmap));

    auto next = [=](){
        if(calIt->next()) {
            return calIt->current()->as_str();
        }
        else {
            return uICAL::string("END");
        }
    };

    REQUIRE(next() == 
        "Calendar EVENT: Irrigation Front\n"
        " - start: 20191017T100000EST\n"
        " - span: PT20M\n"
    );

    REQUIRE(next() ==
        "Calendar EVENT: Irrigation Back\n"
        " - start: 20191017T102000EST\n"
        " - span: PT5M\n"
    );

    REQUIRE(next() ==
        "Calendar EVENT: Irrigation Beds\n"
        " - start: 20191017T103000EST\n"
        " - span: PT10M\n"
    );

    REQUIRE(next() == "END");
}


TEST_CASE("Calendar::finite", "[uICAL][Calendar]") {
    std::ifstream input(std::string("test/data/ical1.txt"));
    uICAL::istream_stl ical(input);

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    auto cal = uICAL::Calendar::load(ical, tzmap, [](const uICAL::VEvent& event) {
        if (event.summary == "Irrigation Back") return true;
        return false;
    });

    auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal,
        uICAL::DateTime("20191016T102000Z"),
        uICAL::DateTime("20191017T103000Z"));

}
