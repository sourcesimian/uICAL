/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/util.h"
#include "uICAL/counter.h"
#include "uICAL/error.h"

namespace uICAL {

    bool Counter::reset(const DateStamp& base) {
        this->wrap();
        this->base = base;
        return true;
    }

    void Counter::str(std::ostream& out) const {
        out << "<" << this->name() << ">";
    }

    bool operator > (const Counter::ptr& a, const Counter::ptr& b) {
        return a->value() > b->value();
    }

    bool operator < (const Counter::ptr& a, const Counter::ptr& b) {
        return a->value() < b->value();
    }

    bool operator <= (const Counter::ptr& a, const Counter::ptr& b) {
        return a->value() <= b->value();
    }

    bool operator == (const Counter::ptr& a, const Counter::ptr& b) {
        return a->value() == b->value();
    }

    Counter::ptr BySecondCounter::init(const values_t values) {
        return Counter::ptr((Counter*)new BySecondCounter(values));
    }

    Counter::ptr BySecondCounter::init(const value_t value) {
        values_t values{value};
        return Counter::ptr((Counter*)new BySecondCounter(values));
    }

    DateStamp BySecondCounter::value() const {
        DateStamp now = this->base;
        now.second = *this->it;
        return now;
    }

    bool ByMinuteCounter::reset(const DateStamp& base) {
        DateStamp _base = base;
        _base.second = 0;
        return CounterT::reset(_base);
    }

    Counter::ptr ByMinuteCounter::init(const values_t values) {
        return Counter::ptr((Counter*)new ByMinuteCounter(values));
    }

    Counter::ptr ByMinuteCounter::init(const value_t value) {
        values_t values{value};
        return Counter::ptr((Counter*)new ByMinuteCounter(values));
    }

    DateStamp ByMinuteCounter::value() const {
        DateStamp now = this->base;
        now.minute = *this->it;
        return now;
    }

    bool ByHourCounter::reset(const DateStamp& base) {
        DateStamp _base = base;
        _base.second = 0;
        _base.minute = 0;
        return CounterT::reset(_base);
    }

    Counter::ptr ByHourCounter::init(const values_t values) {
        return Counter::ptr((Counter*)new ByHourCounter(values));
    }

    Counter::ptr ByHourCounter::init(const value_t value) {
        values_t values{value};
        return Counter::ptr((Counter*)new ByHourCounter(values));
    }

    DateStamp ByHourCounter::value() const {
        DateStamp now = this->base;
        now.hour = *this->it;
        return now;
    }

    Counter::ptr ByMonthDayCounter::init(const values_t values) {
        return Counter::ptr((Counter*)new ByMonthDayCounter(values));
    }

    Counter::ptr ByMonthDayCounter::init(const value_t value) {
        values_t values{value};
        return Counter::ptr((Counter*)new ByMonthDayCounter(values));
    }

    bool ByMonthDayCounter::reset(const DateStamp& base) {
        return CounterT::reset(base);
    }

    DateStamp ByMonthDayCounter::value() const {
        DateStamp now = this->base;
        if (*this->it > 0) {
            now.day = *this->it;
        }
        else {
            now.day = now.daysInMonth() + *this->it + 1;
        }
        return now;
    }

    bool ByMonthDayCounter::next() {
        bool ret = CounterT::next();
        if (ret) {
            // TODO
            if (*this->it > 28 || *this->it < -28) {
                if (*this->it > (int)this->base.daysInMonth()) {
                    this->wrap();
                    return false;
                }
            }
        }
        return ret;
    }

    Counter::ptr ByWeekNoCounter::init(const values_t values) {
        return Counter::ptr((Counter*)new ByWeekNoCounter(values));
    }

    Counter::ptr ByWeekNoCounter::init(const value_t value) {
        values_t values{value};
        return Counter::ptr((Counter*)new ByWeekNoCounter(values));
    }

    DateStamp ByWeekNoCounter::value() const {
        DateStamp now = this->base;
        now.setWeekNo(*this->it);
        return now;
    }

    Counter::ptr ByMonthCounter::init(const values_t values) {
        return Counter::ptr((Counter*)new ByMonthCounter(values));
    }

    Counter::ptr ByMonthCounter::init(const value_t value) {
        values_t values{value};
        return Counter::ptr((Counter*)new ByMonthCounter(values));
    }

    bool ByMonthCounter::reset(const DateStamp& base) {
        DateStamp _base = base;
        _base.day = 1;
        return CounterT::reset(_base);
    }

    DateStamp ByMonthCounter::value() const {
        DateStamp now = this->base;
        now.month = *this->it;
        return now;
    }

    Counter::ptr ByYearDayCounter::init(const values_t values) {
        return Counter::ptr((Counter*)new ByYearDayCounter(values));
    }

    Counter::ptr ByYearDayCounter::init(const value_t value) {
        values_t values{value};
        return Counter::ptr((Counter*)new ByYearDayCounter(values));
    }

    bool ByYearDayCounter::reset(const DateStamp& base) {
        DateStamp _base = base;
        _base.day = 1;
        _base.month = 1;
        return CounterT::reset(_base);
    }

    DateStamp ByYearDayCounter::value() const {
        DateStamp now = this->base;
        if (*this->it > 0) {
            now.incDay(*this->it - 1);
        }
        else {
            now.incDay(now.daysInYear() + *this->it + 1);
        }
        return now;
    }

    bool ByYearDayCounter::next() {
        bool ret = CounterT::next();
        if (ret) {
            // TODO
            if (*this->it > 365 || *this->it < -365) {
                if (*this->it > (int)this->base.daysInMonth()) {
                    this->wrap();
                    return false;
                }
            }
        }
        return ret;
    }

    Counter::ptr SecondInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new SecondInc(interval));
    }

    Counter::ptr MinuteInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new MinuteInc(interval));
    }

    Counter::ptr HourInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new HourInc(interval));
    }

    Counter::ptr DayInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new DayInc(interval));
    }

    Counter::ptr WeekInc::init(unsigned interval, DateTime::Day wkst) {
        return Counter::ptr((Counter*)new WeekInc(interval, wkst));
    }

    Counter::ptr MonthInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new MonthInc(interval));
    }

    Counter::ptr YearInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new YearInc(interval));
    }
}