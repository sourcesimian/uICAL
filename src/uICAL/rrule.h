/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_rrule_h
#define uical_rrule_h

#include "uICAL/base.h"
#include "uICAL/types.h"
#include "uICAL/datetime.h"
#include "uICAL/datestamp.h"
#include "uICAL/tz.h"

namespace uICAL {
    class RRule : public Base {
        public:
            using ptr = std::shared_ptr<RRule>;

            static RRule::ptr init(const string& rrule, const DateTime& dtstart);
            RRule(const string& rrule, const DateTime& dtstart);

            void exclude(const DateTime& exclude);

            void str(ostream& out) const;

            enum class Freq {
                NONE, SECONDLY, MINUTELY, HOURLY, DAILY, WEEKLY, MONTHLY, YEARLY,
            };

            friend class RRuleIter;
            friend class ByWeekDayCounter;

            using Day_pair = std::pair<int, DateTime::Day>;
            using Day_vector = std::vector<Day_pair>;

        protected:
            void parseRRule(const string& rrule);

            Day_vector parseByDay(const string& name) const;
            int parseInt(const string& name) const;
            std::vector<string> parseArray(const string& value) const;
            DateTime::Day parseDay(const string& name) const ;
            DateTime parseDate(const string& name) const;

            const char* dayAsString(DateTime::Day day) const;
            const char* frequencyAsString(Freq freq) const;
            string intAsString(int value) const;

            bool excluded(const DateTime& now) const;

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
    };

    ostream& operator << (ostream& out, const RRule::Day_pair& dp);
}
#endif
