#include "../catch.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <map>

#include "uICAL.h"

using test_f = std::function<void(std::string, std::string, std::string, std::vector<std::string>, std::vector<std::string>)>;

static void run_tests(const std::string dat_file, test_f test)
{
    std::ifstream input(dat_file);

    std::string dtstart;
    std::string rrule;
    std::vector<std::string> excludes;
    std::vector<std::string> expected;

    int i = 1;
    for (std::string line; std::getline(input, line); )
    {
        if (line.find('#') == 0) {
            // ignore comments
        }
        else if (line.find("DTSTART") == 0) {
            dtstart = line;
        }
        else if (line.find("RRULE") == 0) {
            rrule = line;
        }
        else if (line.find("EXDATE") == 0) {
            excludes.push_back(line);
        }
        else if (line.find(" - ") == 0) {
            expected.push_back(line.substr(3));
        }
        else if (dtstart.size() &&
                 rrule.size() &&
                 expected.size() &&
                 (line.find(" ") == 0 || line.length() == 0)) {
            try {
                test(dtstart, rrule, expected[0], excludes, expected);
            }
            catch (uICAL::Error e) {
                WARN("uICAL::Error " << e.message.c_str());
            } catch (const std::exception& e) {
                WARN("std::exception " << e.what());
            } catch (const std::string& e) {
                WARN("std::string " << e.c_str());
            } catch (...) {
                WARN("Exception ... caught");
            }
            dtstart.clear();
            rrule.clear();
            expected.clear();
        }
        else if (line.find(" ") == 0 || line.length() == 0) {
            dtstart.clear();
            rrule.clear();
            expected.clear();
        }
        else {
            FAIL("! bad line (" << i <<"): \"" << line << "\"");
            CHECK(false);
        }
        i ++;
    }

}

void test_basic(std::string dtstart, std::string rrule, std::string begin, std::vector<std::string> excludes, std::vector<std::string> expected) {
    uICAL::VLine_ptr ldtstart = uICAL::new_ptr<uICAL::VLine>(dtstart);
    uICAL::VLine_ptr lrrule = uICAL::new_ptr<uICAL::VLine>(rrule);

    uICAL::DateTime start = uICAL::DateTime(ldtstart->value);

    std::string res;
    try {
        uICAL::DateTime rrBegin;
        if (!begin.empty()) {
            rrBegin = uICAL::DateTime(begin);
        }

        uICAL::RRule_ptr rr = uICAL::new_ptr<uICAL::RRule>(lrrule->value, start);
        for (std::string exclude : excludes) {
            uICAL::VLine_ptr excl = uICAL::new_ptr<uICAL::VLine>(exclude);
            rr->exclude(uICAL::DateTime(excl->value));
        }

        uICAL::RRuleIter_ptr occ = uICAL::new_ptr<uICAL::RRuleIter>(rr, rrBegin, uICAL::DateTime());

        std::vector<std::string> results;
        for (auto eit = expected.begin(); eit != expected.end(); ++eit) {
            if (!occ->next()) {
                break;
            }
            std::ostringstream res;
            uICAL::DateTime now = occ->now();
            res << now.as_str();
            results.push_back(res.str());
        }

        std::ostringstream exp, res;

        bool fail = false;
        auto eit = expected.begin();
        auto rit = results.begin();
        for (; eit != expected.end(); ++eit, ++rit) {
            if (rit == results.end()) {
                fail = true;
            }
            if ((*eit) == (*rit)) {
                exp << (*eit) << " ";
                res << "               " << " ";
            }
            else {
                if ((*eit) == "...") {
                    break;
                }
                std::string diff;
                for (unsigned i = 0; i < (*eit).length(); ++i) {
                    diff.push_back((*eit)[i] == (*rit)[i] ? ' ' : (*rit)[i]);
                }
                exp << (*eit) << " ";
                res << diff << " ";
                fail = true;
            }
        }
        if (fail) {
            WARN("FAIL: " << start.as_str() << " " << rrule << "\n" <<
                 "  ccc: " << occ->as_str() << "\n" <<
                 "  exp: " << exp.str() << "\n" <<
                 "  res: " << res.str()
            );

            REQUIRE(false);
        }
        else {
            INFO("PASS: " << start.as_str() << " " << rrule);
            REQUIRE(true);
        }
    }
    catch (uICAL::Error e) {
        WARN("EXEP: " << start.as_str() << " " << rrule << "\n" <<
             "  msg: " << e.message
        );
    }
}

TEST_CASE("RRule::rrule_txt", "[uICAL][RRule]") {
    run_tests("test/data/rrule.txt", test_basic);
}

TEST_CASE("RRule::test2", "[uICAL][RRule]") {
    uICAL::string rrule("FREQ=DAILY;COUNT=4");
    uICAL::string dtstart("19970902T090000");
    uICAL::string begin("19970902T090000");
    uICAL::string end("29970902T090000");

    auto rr = uICAL::RRuleIter(
        uICAL::new_ptr<uICAL::RRule>(rrule, uICAL::DateTime(dtstart, uICAL::TZ::unaware())),
        uICAL::DateTime(), uICAL::DateTime());

    REQUIRE_THROWS_WITH(rr.now(), "RecurrenceError: Not yet initialised, call next() first");

    REQUIRE(rr.next() == true);
    REQUIRE(rr.now().as_str() == "19970902T090000");

    REQUIRE(rr.next() == true);
    REQUIRE(rr.now().as_str() == "19970903T090000");

    REQUIRE(rr.next() == true);
    REQUIRE(rr.now().as_str() == "19970904T090000");

    REQUIRE(rr.next() == true);
    REQUIRE(rr.now().as_str() == "19970905T090000");

    REQUIRE(rr.next() == false);

    REQUIRE_THROWS_WITH(rr.now(), "RecurrenceError: No more occurrences");
}

TEST_CASE("RRULE::str", "[uICAL][RRule]") {
    uICAL::string rule("FREQ=DAILY;COUNT=4");
    uICAL::string dtstart("19970902T090000");

    uICAL::RRule_ptr rrule = uICAL::new_ptr<uICAL::RRule>(rule, dtstart);

    REQUIRE(rrule->as_str() == "RRULE:FREQ=DAILY;INTERVAL=1;COUNT=4;WKST=MO");
}

TEST_CASE("RRULE::empty", "[uICAL][RRule]") {
    uICAL::string rule("");
    uICAL::string dtstart("19970902T090000");

    auto rrule = uICAL::RRule(rule, dtstart);

    REQUIRE(rrule.as_str() == "RRULE:FREQ=DAILY;INTERVAL=1;COUNT=1;WKST=MO");
}

TEST_CASE("RRULE::bad_freq", "[uICAL][RRule]") {
    uICAL::string rule("FREQ=FORTNIGHTLY");
    uICAL::string dtstart("19970902T090000");

    REQUIRE_THROWS_WITH(uICAL::RRule(rule, dtstart), "ParseError: Unknown RRULE:FREQ type: FORTNIGHTLY");
}

// TEST_CASE("RRULE::bad_indexs", "[uICAL][RRule]") {
//     uICAL::string dtstart("19970902T090000");

//     auto rrule = uICAL::RRule("", dtstart);

//     REQUIRE_THROWS_WITH(rrule.parseDay("MY"), "ParseError: Unknown day name: MY");
//     REQUIRE_THROWS_WITH(rrule.dayAsString(uICAL::DateTime::Day::NONE), "ParseError: Unknown day index: ...");
//     REQUIRE_THROWS_WITH(rrule.frequencyAsString(uICAL::RRule::Freq::NONE), "ParseError: Unknown dafrequencyy index: ...");
// }

TEST_CASE("RRULE::negative_range", "[uICAL][RRule]") {
    uICAL::string dtstart("19970902T090000");
    uICAL::string begin("19970902T090000");
    uICAL::string end("19970901T090000");

    REQUIRE_THROWS_WITH(
        uICAL::RRuleIter(uICAL::new_ptr<uICAL::RRule>("", uICAL::DateTime(dtstart)),
                         uICAL::DateTime(begin),
                         uICAL::DateTime(end)),
        "ValueError: Begin and end describe a negative range"
    );
}

TEST_CASE("RRULE::negative_end", "[uICAL][RRule]") {
    uICAL::string dtstart("19970902T090000");
    uICAL::string begin("19970802T090000");
    uICAL::string end("19970901T090000");

    auto rruleIt = uICAL::RRuleIter(uICAL::new_ptr<uICAL::RRule>("", uICAL::DateTime(dtstart)),
                                    uICAL::DateTime(begin),
                                    uICAL::DateTime(end));

    REQUIRE(rruleIt.next() == false);
}

// TEST_CASE("RRULE::until_before_begin", "[uICAL][RRule]") {
//     uICAL::string dtstart("19970902T090000");
//     uICAL::string begin("19971102T090000");
//     uICAL::string end("19971130T090000");

//     auto rruleIt = uICAL::RRuleIter(uICAL::new_ptr<uICAL::RRule>("FREQ=DAILY;WKST=MO;UNTIL=19970930T090000",
//                                                                  uICAL::DateTime(dtstart)),
//                                     uICAL::DateTime(begin),
//                                     uICAL::DateTime(end));
//     REQUIRE(rruleIt.next() == false);
// }

TEST_CASE("RRULE::done_before_begin", "[uICAL][RRule]") {
    uICAL::string dtstart("19970902T090000");
    uICAL::string begin("19971102T090000");
    uICAL::string end("19971130T090000");

    auto rruleIt = uICAL::RRuleIter(uICAL::new_ptr<uICAL::RRule>("FREQ=DAILY;WKST=MO;COUNT=10",
                                                                 uICAL::DateTime(dtstart)),
                                    uICAL::DateTime(begin),
                                    uICAL::DateTime(end));
    REQUIRE(rruleIt.next() == false);
}