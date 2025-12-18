#include "../catch.hpp"

#include "uICAL.h"

#include "uICAL/vevent.h"

#include <fstream>
#include <sstream>

// Mock istream that simulates the OLD buggy Arduino readuntil behavior
// with a fixed 80-byte buffer that truncates long lines
class istream_mock_buggy : public uICAL::istream {
public:
    istream_mock_buggy(std::istream& istm) : istm(istm) {}

    char peek() const override {
        return istm.peek();
    }

    char get() override {
        return istm.get();
    }

    // Simulate the OLD buggy Arduino behavior: 80-byte buffer, no looping
    bool readuntil(uICAL::string& st, char delim, size_t maxLen = 0) override {
        (void)maxLen;  // Buggy version ignores maxLen
        const size_t len = 81;  // Old buggy buffer size
        char buf[len];

        // Read up to len-1 chars or until delimiter
        size_t i = 0;
        while (i < len - 1) {
            int c = istm.get();
            if (c == EOF || c == delim) {
                break;
            }
            buf[i++] = (char)c;
        }

        if (i > 0) {
            buf[i] = 0;
            st = buf;
            return true;
        }
        return false;
    }

protected:
    std::istream& istm;
};

// Mock istream that simulates the FIXED Arduino readuntil behavior
// with original buffer size but looping, capped at MAX_LINE_LENGTH
class istream_mock_fixed : public uICAL::istream {
public:
    istream_mock_fixed(std::istream& istm) : istm(istm) {}

    char peek() const override {
        return istm.peek();
    }

    char get() override {
        return istm.get();
    }

    // Simulate the FIXED Arduino behavior: original buffer, loop with optional max
    bool readuntil(uICAL::string& st, char delim, size_t maxLen = 0) override {
        static const size_t CHUNK_SIZE = 81;       // Original buffer size
        char buf[CHUNK_SIZE];
        st = "";

        while (maxLen == 0 || st.length() < maxLen) {
            size_t i = 0;
            while (i < CHUNK_SIZE - 1) {
                int c = istm.get();
                if (c == EOF || c == delim) {
                    buf[i] = 0;
                    st += buf;
                    return !st.empty() || i > 0;
                }
                buf[i++] = (char)c;
            }
            buf[i] = 0;
            st += buf;
            // Buffer was full, continue reading until delimiter or max
        }
        // Hit max length - consume remaining chars until delimiter
        while (true) {
            int c = istm.get();
            if (c == EOF || c == delim) break;
        }
        return true;
    }

protected:
    std::istream& istm;
};

TEST_CASE("Calendar::basic", "[uICAL][Calendar]") {
    std::ifstream input(std::string("test/data/ical1.txt"));
    uICAL::istream_stl ical(input);

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    auto cal = uICAL::Calendar::load(ical, tzmap);
    REQUIRE(cal->as_str() == "CALENDAR\n");

    auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal,
        uICAL::DateTime("20191016T102000Z"),
        uICAL::DateTime("20191017T103000EST", tzmap));

    auto next = [=](){
        if(calIt->next()) {
            return calIt->current()->as_str();
        }
        else {
            return uICAL::string("END");
        }
    };

    REQUIRE(next() ==
        "Calendar EVENT: Irrigation Front\n"
        " - start: 20191017T100000EST\n"
        " - span: PT20M\n"
    );

    REQUIRE(next() ==
        "Calendar EVENT: Irrigation Back\n"
        " - start: 20191017T102000EST\n"
        " - span: PT5M\n"
    );

    REQUIRE(next() ==
        "Calendar EVENT: Irrigation Beds\n"
        " - start: 20191017T103000EST\n"
        " - span: PT10M\n"
    );

    REQUIRE(next() == "END");
}


TEST_CASE("Calendar::finite", "[uICAL][Calendar]") {
    std::ifstream input(std::string("test/data/ical1.txt"));
    uICAL::istream_stl ical(input);

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    auto cal = uICAL::Calendar::load(ical, tzmap, [](const uICAL::VEvent& event) {
        if (event.summary == "Irrigation Back") return true;
        return false;
    });

    auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal,
        uICAL::DateTime("20191016T102000Z"),
        uICAL::DateTime("20191017T103000Z"));
}

TEST_CASE("Calendar::folding", "[uICAL][Calendar]") {
    std::ifstream input(std::string("test/data/ical_folding.txt"));
    uICAL::istream_stl ical(input);

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    auto cal = uICAL::Calendar::load(ical, tzmap);
    REQUIRE(cal->as_str() == "CALENDAR\n");

    auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(
        cal,
        uICAL::DateTime("20191016T102000Z"),
        uICAL::DateTime("20191017T103000EST", tzmap));

    auto next = [=]() {
        if(calIt->next()) {
            return calIt->current()->as_str();
        } else {
            return uICAL::string("END");
        }
    };

    REQUIRE(next() ==
            "Calendar EVENT: This is a short line that is folded.\n"
            " - start: 20191017T102000EST\n"
            " - span: PT5M\n");

    REQUIRE(next() ==
            "Calendar EVENT: This is a long line which needs to wrap as it's "
            "longer than 75 characters. "
            "https://icalendar.org/iCalendar-RFC-5545/3-1-content-lines.html "
            "states that lines longer than 75 characters should be folded.\n"
            " - start: 20191017T103000EST\n"
            " - span: PT10M\n");

    REQUIRE(next() == "END");
}

TEST_CASE("Calendar::folding_escaped_multiline", "[uICAL][Calendar]") {
    // Test parsing of multi-line DESCRIPTION with:
    // - Escaped characters: \n (newline), \, (comma), \; (semicolon)
    // - UTF-8 characters (€)
    // - Line folding that breaks mid-word/number
    std::ifstream input(std::string("test/data/ical_folding.txt"));
    uICAL::istream_stl ical(input);

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    auto cal = uICAL::Calendar::load(ical, tzmap);
    REQUIRE(cal->as_str() == "CALENDAR\n");

    // Query date range that includes the 2022 ski trip event
    auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(
        cal,
        uICAL::DateTime("20220120T000000Z"),
        uICAL::DateTime("20220131T000000Z"));

    auto next = [=]() {
        if(calIt->next()) {
            return calIt->current()->as_str();
        } else {
            return uICAL::string("END");
        }
    };

    // Verify the event with escaped characters and UTF-8 parses correctly
    auto result = next();
    REQUIRE(result.find("Ski Trip with escaped chars") != uICAL::string::npos);

    REQUIRE(next() == "END");
}

TEST_CASE("Arduino::readuntil_buffer_bug", "[uICAL][Arduino]") {
    // This test demonstrates the bug in the old Arduino readuntil implementation
    // which had a fixed 80-byte buffer that truncated long ICS lines.
    //
    // The DESCRIPTION line in the test data is ~250 chars and spans multiple
    // folded lines. With the old buggy implementation, it would be truncated
    // at 80 bytes, causing the parser to see malformed lines.

    SECTION("Buggy 80-byte buffer causes parse error") {
        std::ifstream input(std::string("test/data/ical_folding.txt"));
        istream_mock_buggy ical(input);

        uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();

        // The buggy implementation truncates long lines, causing parse errors
        // when VLineStream tries to unfold continuation lines
        REQUIRE_THROWS_AS(
            uICAL::Calendar::load(ical, tzmap),
            uICAL::ParseError
        );
    }

    SECTION("Fixed looping implementation works correctly") {
        std::ifstream input(std::string("test/data/ical_folding.txt"));
        istream_mock_fixed ical(input);

        uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();

        // The fixed implementation loops to read complete lines
        auto cal = uICAL::Calendar::load(ical, tzmap);
        REQUIRE(cal->as_str() == "CALENDAR\n");

        // Verify we can find the event with the long DESCRIPTION
        auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(
            cal,
            uICAL::DateTime("20220120T000000Z"),
            uICAL::DateTime("20220131T000000Z"));

        REQUIRE(calIt->next());
        auto result = calIt->current()->as_str();
        REQUIRE(result.find("Ski Trip with escaped chars") != uICAL::string::npos);
    }
}
