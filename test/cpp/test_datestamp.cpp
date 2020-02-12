#include <cstdio>
#include <iostream>
#include <map>

#include "uICAL.h"

uICAL::DateStamp ds(const std::string& value) {
    using namespace uICAL;
    DateTime dt(value);
    return dt.datestamp();
}

void cmp_datestamp(const std::string test, uICAL::DateStamp res, uICAL::DateStamp exp) {
    using namespace uICAL;

    if (res != exp) {
        std::cout << "FAIL: " << test << std::endl;
        std::cout << " res: " << res << std::endl;
        std::cout << " exp: " << exp << std::endl;
    }
}

void test_datestamp_1() {
    std::cout << "TEST: datestamp 1" << std::endl;

    using namespace uICAL;

    DateStamp dt1, exp;

    // Leap year - Feburary rollover
    dt1 = ds("19840228T090000");
    dt1.incDay(1);
    cmp_datestamp("Leap year - Feburary rollover 1", dt1, ds("19840229T090000"));
    dt1.incDay(1);
    cmp_datestamp("Leap year - Feburary rollover 2", dt1, ds("19840301T090000"));

    // Non leap year - Feburary rollover
    dt1 = ds("19850228T090000");
    dt1.incDay(1);
    cmp_datestamp("Non leap year - Feburary rollover", dt1, ds("19850301T090000"));


    // September roll over
    dt1 = ds("19970929T090000");
    dt1.incDay(1);
    cmp_datestamp("September roll over 1", dt1, ds("19970930T090000"));
    dt1.incDay(1);
    cmp_datestamp("September roll over 2", dt1, ds("19971001T090000"));
}

void test_datestamp_weekNo() {
    using namespace uICAL;

    std::cout << "TEST: datestamp weekNo" << std::endl;

    auto t = [](const std::string datetime, DateTime::Day dayOfWeek, unsigned weekNo) {
        DateStamp dt = ds(datetime);
        DateTime::Day d = dt.dayOfWeek();
        unsigned w = dt.weekNo();
        if (dayOfWeek != DateTime::Day::NONE && d != dayOfWeek)
            std::cout << dt << " = " << (unsigned)d << " " << "{{" << (unsigned)dayOfWeek << "}}" << std::endl;
        else if (w != weekNo)
            std::cout << dt << " = " << w << " " << "[" << weekNo << "]" << std::endl;
        else
            std::cout << dt << " = " << w << std::endl;

    };

    // Starts on Mon
    t("19960101T090000", DateTime::Day::MON, 1);
    t("19960107T090000", DateTime::Day::SUN, 1);
    t("19960108T090000", DateTime::Day::MON, 2);

    // Starts on Tues
    t("19910101T090000", DateTime::Day::TUE, 1);
    t("19910106T090000", DateTime::Day::SUN, 1);
    t("19910107T090000", DateTime::Day::MON, 2);

    // Starts on Weds
    t("19970101T090000", DateTime::Day::WED, 1);
    t("19970105T090000", DateTime::Day::SUN, 1);
    t("19970106T090000", DateTime::Day::MON, 2);

    // Starts on Thurs
    t("19980101T090000", DateTime::Day::THU, 1);
    t("19980104T090000", DateTime::Day::SUN, 1);
    t("19980105T090000", DateTime::Day::MON, 2);
    t("19980111T090000", DateTime::Day::SUN, 2);
    t("19980112T090000", DateTime::Day::MON, 3);

    t("19980118T090000", DateTime::Day::SUN, 3);
    t("19980119T090000", DateTime::Day::MON, 4);
    t("19980125T090000", DateTime::Day::SUN, 4);
    t("19980126T090000", DateTime::Day::MON, 5);
    t("19980201T090000", DateTime::Day::SUN, 5);
    t("19980202T090000", DateTime::Day::MON, 6);
    t("19980208T090000", DateTime::Day::SUN, 6);
    t("19980209T090000", DateTime::Day::MON, 7);
    t("19980215T090000", DateTime::Day::SUN, 7);
    t("19980216T090000", DateTime::Day::MON, 8);
    t("19980222T090000", DateTime::Day::SUN, 8);
    t("19980223T090000", DateTime::Day::MON, 9);

    // Starts on Fri
    t("19990101T090000", DateTime::Day::FRI, 53);
    t("19990103T090000", DateTime::Day::SUN, 53);
    t("19990104T090000", DateTime::Day::MON, 1);
    t("19990110T090000", DateTime::Day::SUN, 1);
    t("19990111T090000", DateTime::Day::MON, 2);

    // Starts on Sat
    t("20000101T090000", DateTime::Day::SAT, 52);
    t("20000102T090000", DateTime::Day::SUN, 52);
    t("20000103T090000", DateTime::Day::MON, 1);
    t("20000109T090000", DateTime::Day::SUN, 1);
    t("20000110T090000", DateTime::Day::MON, 2);

    // Starts on Sun
    t("20060101T090000", DateTime::Day::SUN, 52);
    t("20060102T090000", DateTime::Day::MON, 1);
    t("20060108T090000", DateTime::Day::SUN, 1);
    t("20060109T090000", DateTime::Day::MON, 2);
}


void test_datestamp() {
    test_datestamp_1();
    test_datestamp_weekNo();
}
