
#include "uICAL.h"

#include <fstream>

void test_cal1() {
    std::ifstream input(std::string("test/data/ical1.txt"));
    uICAL::istream_stl ical(input);

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    auto cal = uICAL::Calendar::load(ical, tzmap);
    std::cout << tzmap->as_str();

    auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal,
        uICAL::DateTime("20191016T102000Z"),
        uICAL::DateTime("20191017T103000EST", tzmap));

    while (calIt->next()) {
        auto entry = calIt->current();
        std::cout << entry->as_str() << std::endl;
    }

    std::cout << "DONE" << std::endl;
}

void test_calendar() {
    test_cal1();
}
