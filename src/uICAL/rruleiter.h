/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_rruleiter_h
#define uical_rruleiter_h

#include "uICAL/base.h"

namespace uICAL {
    class Counter;
    class RRule;

    class RRuleIter : public Base {
        public:
            RRuleIter(const RRule_ptr rr, const DateTime& begin, const DateTime& end);

            bool next();
            DateTime now() const;

            void str(ostream& out) const;

        protected:
            using counters_t = std::vector<Counter_ptr>;
            using sync_f = std::function<void(counters_t::iterator it)>;

            bool start();
            bool expired(const DateTime& current) const;
            bool nextExclude();
            bool nextNow();
            void setCurrentNow();

            void setupCounters(const DateStamp& base);

            const RRule_ptr rr;

            DateTime range_begin;
            DateTime range_end;

            Cascade_ptr cascade;
            int count;

            DateTime current_now;
    };
}
#endif
