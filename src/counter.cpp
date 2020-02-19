/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/counter.h"

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

    bool BySecondCounter::syncLock(const DateStamp& from, const DateStamp& now) const
    {
        return from.second <= now.second;
    }

    const string BySecondCounter::name() const { return "BySecond"; }

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

    bool ByMinuteCounter::syncLock(const DateStamp& from, const DateStamp& now) const {
        return from.minute <= now.minute;
    }

    const string ByMinuteCounter::name() const { return "ByMinute"; }

    Counter::ptr ByHourCounter::init(const values_t values) {
        return Counter::ptr((Counter*)new ByHourCounter(values));
    }

    Counter::ptr ByHourCounter::init(const value_t value) {
        values_t values{value};
        return Counter::ptr((Counter*)new ByHourCounter(values));
    }

    bool ByHourCounter::reset(const DateStamp& base) {
        DateStamp _base = base;
        _base.second = 0;
        _base.minute = 0;
        return CounterT::reset(_base);
    }

    DateStamp ByHourCounter::value() const {
        DateStamp now = this->base;
        now.hour = *this->it;
        return now;
    }

    bool ByHourCounter::syncLock(const DateStamp& from, const DateStamp& now) const {
        return from.hour <= now.hour;
    }

    const string ByHourCounter::name() const { return "ByHour"; }

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

    bool ByMonthDayCounter::syncLock(const DateStamp& from, const DateStamp& now) const {
        return from.day <= now.day;
    }

    const string ByMonthDayCounter::name() const { return "ByMonthDay"; }

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

    bool ByWeekNoCounter::syncLock(const DateStamp& from, const DateStamp& now) const {
        return from.weekNo() <= now.weekNo();
    }

    const string ByWeekNoCounter::name() const { return "ByWeekNo"; }

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

    bool ByMonthCounter::syncLock(const DateStamp& from, const DateStamp& now) const {
        return from.month <= now.month;
    }

    const string ByMonthCounter::name() const { return "ByMonth"; }

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

    bool ByYearDayCounter::syncLock(const DateStamp& from, const DateStamp& now) const {
        return from.dayOfYear() == now.dayOfYear();
    }

    const string ByYearDayCounter::name() const { return "ByYearDay"; }

    void IncCounter::str(std::ostream& out) const {
        out << "<" << this->name() << " " << this->interval << ">";
    }

    Counter::ptr SecondInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new SecondInc(interval));
    }

    bool SecondInc::next() {
        unsigned minute = this->base.minute;
        this->base.incSecond(this->interval);
        return minute == this->base.minute;
    }

    const string SecondInc::name() const { return "SecondInc"; }

    Counter::ptr MinuteInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new MinuteInc(interval));
    }

    bool MinuteInc::reset(const DateStamp& base) {
        DateStamp _base = base;
        _base.second = 0;
        return IncCounter::reset(_base);
    }

    bool MinuteInc::next() {
        unsigned hour = this->base.hour;
        this->base.incMinute(this->interval);
        return hour == this->base.hour;
    }

    const string MinuteInc::name() const { return "MinuteInc"; }

    Counter::ptr HourInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new HourInc(interval));
    }

    bool HourInc::reset(const DateStamp& base) {
        DateStamp _base = base;
        _base.second = 0;
        _base.minute = 0;
        return IncCounter::reset(_base);
    }

    bool HourInc::next() {
        unsigned day = this->base.day;
        this->base.incHour(this->interval);
        return day == this->base.day;
    }

    const string HourInc::name() const { return "HourInc"; }

    Counter::ptr DayInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new DayInc(interval));
    }

    bool DayInc::next() {
        unsigned month = this->base.month;
        this->base.incDay(this->interval);
        return month == this->base.month;
    }

    const string DayInc::name() const { return "DayInc"; }

    Counter::ptr WeekInc::init(unsigned interval, DateTime::Day wkst) {
        return Counter::ptr((Counter*)new WeekInc(interval, wkst));
    }

    bool WeekInc::reset(const DateStamp& base) {
        DateStamp _base = base;
        unsigned weekNo = _base.weekNo();
        _base.setWeekNo(weekNo);
        return IncCounter::reset(_base);
    }

    bool WeekInc::next() {
        unsigned year = this->base.year;
        this->base.incWeek(this->interval, this->wkst);
        return year == this->base.year;
    }

    const string WeekInc::name() const { return "WeekInc"; }

    Counter::ptr MonthInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new MonthInc(interval));
    }

    bool MonthInc::reset(const DateStamp& base) {
        DateStamp _base = base;
        _base.day = 1;
        return IncCounter::reset(_base);
    }

    bool MonthInc::next() {
        unsigned year = this->base.year;
        this->base.day = 1;
        this->base.incMonth(this->interval);
        return year == this->base.year;
    }

    const string MonthInc::name() const { return "MonthInc"; }

    Counter::ptr YearInc::init(unsigned interval) {
        return Counter::ptr((Counter*)new YearInc(interval));
    }

    bool YearInc::reset(const DateStamp& base) {
        DateStamp _base = base;
        _base.day = 1;
        _base.month = 1;
        return IncCounter::reset(_base);
    }

    bool YearInc::next() {
        this->base.day = 1;
        this->base.month = 1;
        this->base.incYear(this->interval);
        return true;
    }

    const string YearInc::name() const { return "YearInc"; }

}