#include "uICAL.h"

#include "uICAL/vcomponentstream.h"


#include <iostream>
#include <fstream>
#include <list>

void test_tz1() {
    std::cout << "TEST: tz1" << std::endl;

    std::ifstream input(std::string("test/data/ical2.txt"));
    uICAL::istream_stl ical(input);

    uICAL::VLineStream lines(ical);
    uICAL::VComponentStream components(lines);

    auto vcalendar = components.next();

    auto tzmap = uICAL::TZMap::init(*vcalendar.get());

    {
        auto ds = uICAL::DateStamp("20191105T175555");

        std::list<uICAL::TZ::ptr> tzList;
        
        tzList.push_back(uICAL::TZ::init("NZDT", tzmap));
        tzList.push_back(uICAL::TZ::init("Pacific/Auckland", tzmap));

        tzList.push_back(uICAL::TZ::init("CET", tzmap));
        tzList.push_back(uICAL::TZ::init("Europe/Paris", tzmap));

        tzList.push_back(uICAL::TZ::init("EST", tzmap));
        tzList.push_back(uICAL::TZ::init("America/New_York", tzmap));

        tzList.push_back(uICAL::TZ::init("CST", tzmap));
        tzList.push_back(uICAL::TZ::init("America/Chicago", tzmap));

        tzList.push_back(uICAL::TZ::init("MST", tzmap));
        tzList.push_back(uICAL::TZ::init("America/Phoenix", tzmap));

        tzList.push_back(uICAL::TZ::init("PST", tzmap));
        tzList.push_back(uICAL::TZ::init("America/Los_Angeles", tzmap));

        auto tz = uICAL::TZ::init("Z");
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
