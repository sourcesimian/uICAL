#ifndef uical_recurrenceiter_h
#define uical_recurrenceiter_h

#include "uICAL/counter.h"
#include "uICAL/rruleparser.h"

namespace uICAL {
    class RRule {
        public:
            using ptr = std::shared_ptr<RRule>;

            static RRule::ptr init(const std::string rrule, const DateTime dtstart);

            void begin(const DateTime begin);
            void end(const DateTime end);
            void exclude(const DateTime exclude);

            bool next();
            DateTime now() const;

            void str(std::ostream& out) const;

        protected:
            RRule(const RRuleParser::ptr parser, const DateTime dtstart);

            using counters_t = std::vector<Counter::ptr>;
            using sync_f = std::function<void(counters_t::iterator it)>;

            bool init();
            void setupCounters(DateStamp base);
            bool initCounters(DateStamp base, DateStamp from);
            bool nextDate();
            bool nextExclude();
            bool nextNow();
            void setCurrentNow();
            bool resetCascade(counters_t::iterator it, sync_f sync);
            bool expired(DateTime current) const;
        
            const RRuleParser::ptr p;
            const DateTime dtstart;

            DateTime until;
            DateTime range_begin;
            DateTime range_end;
            std::vector<DateTime> excludes;

            counters_t counters;
            int count;

            DateTime current_now;
    };
}
#endif
