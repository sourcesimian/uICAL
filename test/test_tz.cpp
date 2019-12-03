#include "uICAL/cppstl.h"
#include "uICAL/tz.h"
#include "uICAL/tzbyid.h"
#include "uICAL/tzidmap.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/datestamp.h"
#include "uICAL/datetime.h"

#include <iostream>
#include <fstream>

void test_tz1() {
    std::ifstream input(std::string("test/data/calendar2.dat"));

    auto vcalendar = uICAL::VComponent::parse(input);

    auto tzidmap = uICAL::TZIdMap::init(*vcalendar.get());

    {
        auto ds = uICAL::DateStamp("20191105T175555");
        auto tz0 = uICAL::TZbyId::init(tzidmap, "Pacific/Auckland");
        auto tz1 = uICAL::TZbyId::init(tzidmap, "Europe/Paris");
        auto tz2 = uICAL::TZbyId::init(tzidmap, "America/New_York");
        auto tz3 = uICAL::TZbyId::init(tzidmap, "America/Chicago");
        auto tz4 = uICAL::TZbyId::init(tzidmap, "America/Phoenix");
        auto tz5 = uICAL::TZbyId::init(tzidmap, "America/Los_Angeles");
        auto dt = uICAL::DateTime(ds, tz1);

        std::cout << ds << std::endl;
        std::cout << dt.datestamp() << std::endl;
        std::cout << "--" << std::endl;
        std::cout << dt.datestamp(tz0) << " " << tz0 << std::endl;
        std::cout << dt.datestamp(tz1) << " " << tz1 << std::endl;
        std::cout << dt.datestamp(tz2) << " " << tz2 << std::endl;
        std::cout << dt.datestamp(tz3) << " " << tz3 << std::endl;
        std::cout << dt.datestamp(tz4) << " " << tz4 << std::endl;
        std::cout << dt.datestamp(tz5) << " " << tz5 << std::endl;
    }


    std::cout << "DONE" <<std::endl;
}

void test_tz() {
    test_tz1();
}
