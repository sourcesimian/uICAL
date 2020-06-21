/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_byweekdaycounter_h
#define uical_byweekdaycounter_h

#include "uICAL/counter.h"

namespace uICAL {
    class RRule;

    class ByWeekDayCounter : protected CounterT<RRule::Day_pair> {
        public:
            static Counter_ptr init(const values_t& values, const RRule_ptr& r);
            static Counter_ptr init(const value_t value, const RRule_ptr& r);

            virtual bool reset(const DateStamp& start);
            virtual DateStamp value() const;
            virtual bool next();

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;

        protected:
            ByWeekDayCounter(const values_t& values, const RRule_ptr& p);
            virtual ~ByWeekDayCounter() = default;

            virtual const string name() const { return "ByWeekDay"; }
            virtual void wrap();

            const RRule_ptr& r;

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
