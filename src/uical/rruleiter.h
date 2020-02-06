#ifndef uical_rruleiter_h
#define uical_rruleiter_h

#include "uICAL/counter.h"
#include "uICAL/rrule.h"

namespace uICAL {
    class RRuleIter {
        public:
            using ptr = std::shared_ptr<RRuleIter>;

            static RRuleIter::ptr init(const RRule::ptr rr);
            RRuleIter(const RRule::ptr rr);

            void begin(const DateTime begin);
            void end(const DateTime end);

            bool next();
            DateTime now() const;

            void debug(std::ostream& out) const;
            void str(std::ostream& out) const;

        protected:
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
        
            const RRule::ptr rr;

            DateTime until;
            DateTime range_begin;
            DateTime range_end;
            //std::vector<DateTime> excludes;

            counters_t counters;
            int count;

            DateTime current_now;
    };

    std::ostream & operator << (std::ostream &out, const RRuleIter::ptr &e);
}
#endif
