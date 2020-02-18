/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_rrule_h
#define uical_rrule_h

#include "uICAL/types.h"
#include "uICAL/datetime.h"
#include "uICAL/datestamp.h"
#include "uICAL/tz.h"

namespace uICAL {
    class RRule {
        public:
            using ptr = std::shared_ptr<RRule>;

            static RRule::ptr init(const std::string rrule, const DateTime dtstart);

            void exclude(const DateTime exclude);

            void str(std::ostream& out) const;
            std::string str() const;

            enum class Freq {
                NONE, SECONDLY, MINUTELY, HOURLY, DAILY, WEEKLY, MONTHLY, YEARLY,
            };

            friend class RRuleIter;
            friend class ByWeekDayCounter;

            using Day_pair = std::pair<int, DateTime::Day>;
            using Day_vector = std::vector<Day_pair>;

        protected:
            RRule(const std::string rrule, const DateTime dtstart);

            void parseRRule(const std::string rrule);

            Day_vector parseByDay(const std::string name) const;
            int parseInt(const std::string name) const;
            std::vector<std::string> parseArray(const std::string value) const;
            DateTime::Day parseDay(const std::string name) const ;
            DateTime parseDate(const std::string name) const;

            const char* dayAsString(DateTime::Day day) const;
            const char* frequencyAsString(Freq freq) const;
            std::string intAsString(int value) const;

            const DateTime dtstart;

            Freq freq;
            int interval;
            int count;
            DateTime::Day wkst;
            DateTime until;
            std::vector<unsigned> bySecond;
            std::vector<unsigned> byMinute;
            std::vector<unsigned> byHour;
            std::vector<Day_pair> byDay;
            std::vector<int> byMonthDay;
            std::vector<unsigned> byMonth;
            std::vector<int> byYearDay;
            std::vector<unsigned> byWeekNo;
            std::vector<int> bySetPos;

            std::vector<DateTime> excludes;
            bool excluded(const DateTime now) const;
    };

    std::ostream& operator << (std::ostream& out, const RRule::ptr& e);
    std::ostream& operator << (std::ostream& out, const RRule& e);

    std::ostream& operator << (std::ostream& out, const RRule::Day_pair& dp);
}
#endif
