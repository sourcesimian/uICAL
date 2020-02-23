
#include "uICAL.h"

#include <fstream>

void test_cal1() {
    std::ifstream finput(std::string("test/data/ical1.txt"));
    uICAL::istream_stl input(finput);

    auto cal = uICAL::Calendar::init(input);
    std::cout << cal->tzmap->as_str();

    auto calIt = uICAL::CalendarIter::init(cal,
        uICAL::DateTime("20191016T102000Z"),
        uICAL::DateTime("20191017T103000EST", cal->tzmap));

    while (calIt->next()) {
        auto entry = calIt->current();
        std::cout << entry->as_str() << std::endl;
    }

    std::cout << "DONE" << std::endl;
}

void test_calendar() {
    test_cal1();
}
