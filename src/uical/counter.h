/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_bycounter_h
#define uical_bycounter_h

#include "uICAL/datestamp.h"

namespace uICAL {
    class Counter {
        public:
            using ptr = std::shared_ptr<Counter>;

            Counter() {}
            virtual ~Counter() {}

            virtual bool reset(const DateStamp& base);
            virtual bool next() { return true; }
            virtual DateStamp value() const { return this->base; }
            
            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const = 0;
            virtual const std::string name() const = 0;

            virtual void str(std::ostream& out) const;

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
            virtual ~CounterT() {}

            virtual bool next() {
                ++ this->it;
                if (this->it == this->values.end()) {
                    this->wrap();
                    return false;
                }
                return true;
            }

            virtual void str(std::ostream& out) const {
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

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const { return from.second <= now.second; }
            virtual const std::string name() const { return "BySecond"; }
    };

    class ByMinuteCounter : protected CounterT<unsigned> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);
            
            ByMinuteCounter(const values_t values) : CounterT(values) {}

            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;      

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const { return from.minute <= now.minute; }
            virtual const std::string name() const { return "ByMinute"; }
    };

    class ByHourCounter : protected CounterT<unsigned> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);

            ByHourCounter(const values_t values) : CounterT(values) {}
            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;      

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const { return from.hour <= now.hour; }
            virtual const std::string name() const { return "ByHour"; }
    };

    class ByMonthDayCounter : protected CounterT<int> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);

            ByMonthDayCounter(const values_t values) : CounterT(values) {}

            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;      
            virtual bool next();

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const { return from.day <= now.day; }
            virtual const std::string name() const { return "ByMonthDay"; }
    };

    class ByWeekNoCounter : protected CounterT<unsigned> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);

            ByWeekNoCounter(const values_t values) : CounterT(values) {}

            virtual DateStamp value() const;

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const { return from.weekNo() <= now.weekNo(); }
            virtual const std::string name() const { return "ByWeekNo"; }
    };

    class ByMonthCounter : protected CounterT<unsigned> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);

            ByMonthCounter(const values_t values) : CounterT(values) {}

            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;      

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const { return from.month <= now.month; }
            virtual const std::string name() const { return "ByMonth"; }
    };

    class ByYearDayCounter : protected CounterT<int> {
        public:
            static Counter::ptr init(const values_t values);
            static Counter::ptr init(const value_t value);

            ByYearDayCounter(const values_t values) : CounterT(values) {}

            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;      
            virtual bool next();

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const { return from.dayOfYear() == now.dayOfYear(); }
            virtual const std::string name() const { return "ByYearDay"; }
    };

    class IncCounter : protected Counter {
        public:
            IncCounter(unsigned interval) : interval(interval) {}

            virtual void str(std::ostream& out) const {
                out << "<" << this->name() << " " << this->interval << ">";
            }

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const { std::ignore = from; std::ignore = now; return true; }

        protected:
            virtual void wrap() {};

            const unsigned interval;
    };

    class SecondInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            SecondInc(unsigned interval) : IncCounter(interval) {}

            virtual bool next() {
                unsigned minute = this->base.minute;
                this->base.incSecond(this->interval);
                return minute == this->base.minute;
            }

            virtual const std::string name() const { return "SecondInc"; }
    };

    class MinuteInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            MinuteInc(unsigned interval) : IncCounter(interval) {}

            virtual bool reset(const DateStamp& base) {
                DateStamp _base = base;
                _base.second = 0;
                return IncCounter::reset(_base);
            }

            virtual bool next() {
                unsigned hour = this->base.hour;
                this->base.incMinute(this->interval);
                return hour == this->base.hour;
            }

            virtual const std::string name() const { return "MinuteInc"; }
    };

    class HourInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            HourInc(unsigned interval) : IncCounter(interval) {}

            virtual bool reset(const DateStamp& base) {
                DateStamp _base = base;
                _base.second = 0;
                _base.minute = 0;
                return IncCounter::reset(_base);
            }

            virtual bool next() {
                unsigned day = this->base.day;
                this->base.incHour(this->interval);
                return day == this->base.day;
            }

            virtual const std::string name() const { return "HourInc"; }
    };

    class DayInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            DayInc(unsigned interval) : IncCounter(interval) {}

            virtual bool next() {
                unsigned month = this->base.month;
                this->base.incDay(this->interval);
                return month == this->base.month;
            }

            virtual const std::string name() const { return "DayInc"; }
    };

    class WeekInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval, DateTime::Day wkst);

            WeekInc(unsigned interval, DateTime::Day wkst) : IncCounter(interval), wkst(wkst) {}

            virtual bool reset(const DateStamp& base) {
                DateStamp _base = base;
                unsigned weekNo = _base.weekNo();
                _base.setWeekNo(weekNo);
                return IncCounter::reset(_base);
            }

            virtual bool next() {
                unsigned year = this->base.year;
                this->base.incWeek(this->interval, this->wkst);
                return year == this->base.year;
            }

            virtual const std::string name() const { return "WeekInc"; }

        protected:
            DateTime::Day wkst;
    };

    class MonthInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            MonthInc(unsigned interval) : IncCounter(interval) {}

            virtual bool reset(const DateStamp& base) {
                DateStamp _base = base;
                _base.day = 1;
                return IncCounter::reset(_base);
            }

            virtual bool next() {
                unsigned year = this->base.year;
                this->base.day = 1;
                this->base.incMonth(this->interval);
                return year == this->base.year;
            }

            virtual const std::string name() const { return "MonthInc"; }
    };

    class YearInc : protected IncCounter {
        public:
            static Counter::ptr init(unsigned interval);

            YearInc(unsigned interval) : IncCounter(interval) {}

            virtual bool reset(const DateStamp& base) {
                DateStamp _base = base;
                _base.day = 1;
                _base.month = 1;
                return IncCounter::reset(_base);
            }

            virtual bool next() {
                this->base.day = 1;
                this->base.month = 1;
                this->base.incYear(this->interval);
                return true;
            }

            virtual const std::string name() const { return "YearInc"; }
    };
}
#endif
