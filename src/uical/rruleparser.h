#ifndef uical_recurrence_h
#define uical_recurrence_h

#include "uICAL/datetime.h"
#include "uICAL/datestamp.h"
#include "uICAL/timezone.h"

namespace uICAL {
    class RRuleParser {
        public:
            using ptr = std::shared_ptr<RRuleParser>;

            static RRuleParser::ptr init(const std::string rrule);

            void str(std::ostream& out) const;
            std::string str() const;

            enum class Freq {
                NONE, SECONDLY, MINUTELY, HOURLY, DAILY, WEEKLY, MONTHLY, YEARLY,
            };

            friend class RRule;
            friend class ByWeekDayCounter;

            using Day_pair = std::pair<int, DateTime::Day>;
            using Day_vector = std::vector<Day_pair>;

        protected:
            RRuleParser(const std::string rrule);

            void parseRrule(const std::string rrule);

            Day_vector parseByDay(const std::string name) const;
            int parseInt(const std::string name) const;
            std::vector<std::string> parseArray(const std::string value) const;
            DateTime::Day parseDay(const std::string name) const ;
            DateTime parseDate(const std::string name) const;

            const char* dayAsString(DateTime::Day day) const;
            const char* frequencyAsString(Freq freq) const;
            std::string intAsString(int value) const;

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
    };

    std::ostream & operator << (std::ostream &out, const RRuleParser::ptr &e);
    std::ostream & operator << (std::ostream &out, const RRuleParser &e);

    std::ostream & operator << (std::ostream &out, const RRuleParser::Day_pair &dp);
}
#endif
