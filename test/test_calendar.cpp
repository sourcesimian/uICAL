#include "uICAL/cppstl.h"
#include "uICAL/util.h"

#include "uICAL/calendar.h"

#include <iostream>
#include <fstream>

void test_cal1() {
    std::ifstream input(std::string("test/data/calendar1.dat"));

    auto cal = uICAL::Calendar::init(input);

    auto calIt = uICAL::CalendarIter::init(cal,
        uICAL::DateTime("20191231T100000"),
        uICAL::DateTime("20200123T100000"));

    while (calIt->next()) {
        auto entry = calIt->current();
        std::cout << entry << std::endl;
    }

    std::cout << "DONE" << std::endl;
}

void test_calendar() {
    test_cal1();
}
