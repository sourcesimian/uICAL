/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_byweekdaycounter_h
#define uical_byweekdaycounter_h

#include "uICAL/counter.h"
#include "uICAL/rrule.h"

namespace uICAL {
    class ByWeekDayCounter : protected CounterT<RRule::Day_pair> {
        public:
            static Counter::ptr init(const values_t& values, const RRule::ptr& r);
            static Counter::ptr init(const value_t value, const RRule::ptr& r);

            virtual bool reset(const DateStamp& start);
            virtual DateStamp value() const;
            virtual bool next();

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;

        protected:
            ByWeekDayCounter(const values_t& values, const RRule::ptr& p);

            virtual const string name() const { return "ByWeekDay"; }
            virtual void wrap();

            const RRule::ptr& p;

            RRule::Day_vector byIndexedDay;
            RRule::Day_vector::const_iterator itIndex;
            std::vector<DateTime::Day> byDayOfWeek;

            DateTime::Day firstDayOfSpan;
            unsigned span;
            unsigned cursor;

            unsigned span_offset;
            static const unsigned no_offset;
    };
}
#endif
