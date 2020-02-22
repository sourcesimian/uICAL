/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_bycounter_h
#define uical_bycounter_h

#include "uICAL/types.h"
#include "uICAL/base.h"
#include "uICAL/datestamp.h"

namespace uICAL {
    class Counter : public Base {
        public:
            using ptr = std::shared_ptr<Counter>;

            Counter() {}
            virtual ~Counter() = default;

            virtual bool reset(const DateStamp& base);
            virtual bool next() { return true; }
            virtual DateStamp value() const { return this->base; }
            
            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const = 0;
            virtual const string name() const = 0;

            virtual void str(ostream& out) const;

        protected:
            virtual void wrap() = 0;

            DateStamp base;
    };

    bool operator > (const Counter::ptr& a, const Counter::ptr& b);
    bool operator < (const Counter::ptr& a, const Counter::ptr& b);
    bool operator <= (const Counter::ptr& a, const Counter::ptr& b);
    bool operator == (const Counter::ptr& a, const Counter::ptr& b);

    template <typename T>
    class CounterT : public Counter {
        protected:
            using value_t = T;
            using values_t = std::vector<T>;
            using it_t = typename values_t::const_iterator;

        public:
            virtual ~CounterT() = default;

            virtual bool next() {
                ++ this->it;
                if (this->it == this->values.end()) {
                    this->wrap();
                    return false;
                }
                return true;
            }

            virtual void str(ostream& out) const {
                out << "<" << this->name() << " " << this->values << ">";
            }

        protected:            
            CounterT(const values_t values) : values(values) {}

            virtual void wrap() {
                this->it = this->values.begin();
            }

            const values_t values;
            it_t it;
    };

    class BySecondCounter : protected CounterT<unsigned> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);
            
            BySecondCounter(const values_t values) : CounterT(values) {}

            DateStamp value() const;      

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;
            virtual const string name() const;
    };

    class ByMinuteCounter : protected CounterT<unsigned> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);
            
            ByMinuteCounter(const values_t values) : CounterT(values) {}

            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;      

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;
            virtual const string name() const;
    };

    class ByHourCounter : protected CounterT<unsigned> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);

            ByHourCounter(const values_t values) : CounterT(values) {}
            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;      

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;
            virtual const string name() const;
    };

    class ByMonthDayCounter : protected CounterT<int> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);

            ByMonthDayCounter(const values_t values) : CounterT(values) {}

            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;      
            virtual bool next();

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;
            virtual const string name() const;
    };

    class ByWeekNoCounter : protected CounterT<unsigned> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);

            ByWeekNoCounter(const values_t values) : CounterT(values) {}

            virtual DateStamp value() const;

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;
            virtual const string name() const;
    };

    class ByMonthCounter : protected CounterT<unsigned> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);

            ByMonthCounter(const values_t values) : CounterT(values) {}

            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;      

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;
            virtual const string name() const;
    };

    class ByYearDayCounter : protected CounterT<int> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);

            ByYearDayCounter(const values_t values) : CounterT(values) {}

            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;      
            virtual bool next();

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;
            virtual const string name() const;
    };

    class IncCounter : protected Counter {
        public:
            IncCounter(unsigned interval) : interval(interval) {}

            virtual void str(ostream& out) const;

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const { std::ignore = from; std::ignore = now; return true; }

        protected:
            virtual void wrap() {};

            const unsigned interval;
    };

    class SecondInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            SecondInc(unsigned interval) : IncCounter(interval) {}

            virtual bool next();

            virtual const string name() const;
    };

    class MinuteInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            MinuteInc(unsigned interval) : IncCounter(interval) {}

            virtual bool reset(const DateStamp& base);
            virtual bool next();

            virtual const string name() const;
    };

    class HourInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            HourInc(unsigned interval) : IncCounter(interval) {}

            virtual bool reset(const DateStamp& base);
            virtual bool next();

            virtual const string name() const;
    };

    class DayInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            DayInc(unsigned interval) : IncCounter(interval) {}

            virtual bool next();

            virtual const string name() const;
    };

    class WeekInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval, DateTime::Day wkst);

            WeekInc(unsigned interval, DateTime::Day wkst) : IncCounter(interval), wkst(wkst) {}

            virtual bool reset(const DateStamp& base);
            virtual bool next();

            virtual const string name() const;

        protected:
            DateTime::Day wkst;
    };

    class MonthInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            MonthInc(unsigned interval) : IncCounter(interval) {}

            virtual bool reset(const DateStamp& base);
            virtual bool next();

            virtual const string name() const;
    };

    class YearInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            YearInc(unsigned interval) : IncCounter(interval) {}

            virtual bool reset(const DateStamp& base);
            virtual bool next();

            virtual const string name() const;
    };
}
#endif
