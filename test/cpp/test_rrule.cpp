#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <map>

#include "uICAL.h"

std::string string_diff(const std::string a, const std::string b) {
    std::ostringstream out;
    int len = std::min(a.length(), b.length());
    for (int i = 0; i < len; ++i) {
        if (a.c_str()[i] == b.c_str()[i]) {
            out << " ";
        }
        else {
            out << "^";
        }
    }
    return out.str();
}

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
            test(dtstart, rrule, expected[0], excludes, expected);

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
            std::cerr << "! bad line (" << i <<"): \"" << line << "\"" << std::endl; 
        }
        i ++;
    }

}

void test_basic(std::string dtstart, std::string rrule, std::string begin, std::vector<std::string> excludes, std::vector<std::string> expected) {
    // if (std::string::npos == rrule.find("RRULE:FREQ=MONTHLY;BYDAY=FR;BYMONTHDAY=13"))
    //     return;
    
    uICAL::VLine::ptr ldtstart = uICAL::VLine::init(dtstart);
    uICAL::VLine::ptr lrrule = uICAL::VLine::init(rrule);
    
    uICAL::DateTime start = uICAL::DateTime(ldtstart->value);

    std::string res;
    try {
        //uICAL::Recurrence::ptr rec = uICAL::Recurrence::init(lrrule->value);

        uICAL::DateTime rrBegin;
        if (!begin.empty()) {
            rrBegin = uICAL::DateTime(begin);
        }

        uICAL::RRule::ptr rr = uICAL::RRule::init(lrrule->value, start);
        for (std::string exclude : excludes) {
            uICAL::VLine::ptr excl = uICAL::VLine::init(exclude);
            rr->exclude(uICAL::DateTime(excl->value));
        }

        uICAL::RRuleIter::ptr occ = uICAL::RRuleIter::init(rr, rrBegin, uICAL::DateTime());

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
            std::cout << "FAIL: " << start.as_str() << " " << rrule << std::endl;
            std::cout << "  ccc: " << occ->as_str() << std::endl;
            std::cout << "  exp: " << exp.str() << std::endl;
            std::cout << "  res: " << res.str() << std::endl;
        }
        else {
            std::cout << "PASS: " << start.as_str() << " " << rrule << std::endl;
        }
    }
    catch (uICAL::Error e) {
        std::cout << "EXEP: " << start.as_str() << " " << rrule << std::endl;
        std::cout << "  msg: " << e.message << std::endl;
    }
}


void test_2() {
    uICAL::string rrule("FREQ=DAILY;COUNT=4");
    uICAL::string dtstart("19970902T090000");
    uICAL::string begin("19970902T090000");
    uICAL::string end("29970902T090000");

    auto rr = uICAL::RRuleIter::init(
        uICAL::RRule::init(rrule, uICAL::DateTime(dtstart, uICAL::TZ::unaware())),
        uICAL::DateTime(), uICAL::DateTime());
    while (rr->next())
    {
        std::cout << rr->now().as_str() << std::endl;
    }

}


void test_rrule() {
    run_tests("test/data/rrule.txt", test_basic);
    test_2();
}
