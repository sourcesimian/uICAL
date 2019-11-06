#include "uICAL/cppstl.h"
#include "uICAL/calendar.h"

#include <iostream>
#include <fstream>

void test_cal1() {
    std::ifstream input(std::string("test/data/calendar1.dat"));

    auto cal = uICAL::Calendar::init(input);

    std::cout << "DONE" <<std::endl;
}

void test_calendar() {
    test_cal1();
}