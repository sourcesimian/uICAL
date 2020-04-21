#include "../catch.hpp"

#include "uICAL.h"

#include <fstream>

TEST_CASE("Calendar::basic", "[uICAL][Calendar]") {
    std::ifstream input(std::string("test/data/ical1.txt"));
    uICAL::istream_stl ical(input);

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    auto cal = uICAL::Calendar::load(ical, tzmap);
    std::cout << tzmap->as_str();

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

    std::cout << next() << std::endl;
    std::cout << next() << std::endl;
    std::cout << next() << std::endl;
    REQUIRE(next() == "END");
}
