#include "uICAL.h"

#include <iostream>
#include <fstream>
#include <list>

void test_tz1() {
    std::cout << "TEST: tz1" << std::endl;

    std::ifstream input(std::string("test/data/ical2.txt"));
    uICAL::istream_stl ical(input);

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    auto vcalendar = uICAL::Calendar::load(ical, tzmap);
    {
        auto ds = uICAL::DateStamp("20191105T175555");

        std::list<uICAL::TZ_ptr> tzList;

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("NZDT", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("Pacific/Auckland", tzmap));

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("CET", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("Europe/Paris", tzmap));

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("EST", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("America/New_York", tzmap));

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("CST", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("America/Chicago", tzmap));

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("MST", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("America/Phoenix", tzmap));

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("PST", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("America/Los_Angeles", tzmap));

        auto tz = uICAL::new_ptr<uICAL::TZ>("Z");
        auto dt = uICAL::DateTime(ds, tz);

        std::cout << ds.as_str() << std::endl;
        std::cout << dt.datestamp().as_str() << std::endl;
        std::cout << "--" << std::endl;

        for (auto tz : tzList) {
            std::cout << dt.datestamp(tz).as_str() << " " << tz->as_str() << std::endl;
        }
    }
}

void test_tz() {
    test_tz1();
}
