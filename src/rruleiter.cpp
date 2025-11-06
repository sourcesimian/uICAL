/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/error.h"
#include "uICAL/logging.h"
#include "uICAL/rrule.h"
#include "uICAL/rruleiter.h"
#include "uICAL/counter.h"
#include "uICAL/bysetposcounter.h"
#include "uICAL/byweekdaycounter.h"
#include "uICAL/byandcounter.h"
#include "uICAL/cascade.h"

namespace uICAL {
    RRuleIter::RRuleIter(const RRule_ptr rr, const DateTime& begin, const DateTime& end)
    : rr(rr)
    , cascade(new_ptr<Cascade>())
    , until(rr->until)
    {
        if (begin.valid() && end.valid() && begin > end) {
            throw ValueError("Begin and end describe a negative range");
        }

        this->count = 0;
        if (begin.valid())
            this->range_begin = begin;
        if (end.valid())
            this->range_end = end;
    }

    bool RRuleIter::start() {
        if (this->range_end.valid() && this->rr->dtstart > this->range_end)  {
            return false;
        }

        if (this->range_begin.valid() && this->until.valid() && this->until < this->range_begin) {
            return false;
        }

        DateStamp base = this->rr->dtstart.datestamp();
        this->setupCounters(base);

        /*
        DateStamp begin;
        if (!this->range_begin.empty()) {
            begin = this->range_begin.datestamp(this->dtstart.tz);
        }
        else {
            begin = base;
        }
        */
        DateStamp begin = base;

        if (!this->cascade->initCounters(base, begin)) {
            return false;
        }
        this->count = this->rr->count;
        this->setCurrentNow();

        if (this->range_begin.valid()) {
            while (this->now() < this->range_begin) {
                if (!this->next()) {
                    return false;
                }
            }
        }
        return true;
    }

    DateTime RRuleIter::now() const {
        if (this->count != 0) {
            if (!this->current_now.valid()) {
                throw ImplementationError("Now is invalid");
            }
            return this->current_now;
        }
        if (this->cascade->size() == 0) {
            throw RecurrenceError("Not yet initialised, call next() first");
        }

        throw RecurrenceError("No more occurrences");
    }

    bool RRuleIter::next() {
        if (this->cascade->size() == 0) {
            return this->start();
        }

        if (this->count > 0) {
            this->count --;
        }
        if (this->count == 0) {
            return false;
        }

        for (;;) {
            if (!this->nextExclude()) {
                return false;
            }
            if (this->now().valid()) {
                break;
            }
        }

        if (this->expired(this->now())) {
            this->count = 0;
            return false;
        }

        return true;
    }

    bool RRuleIter::expired(const DateTime& current) const {
        if (this->until.valid() && current > this->until) {
            return true;
        }
        if (this->range_end.valid() && current > this->range_end) {
            return true;
        }
        return false;
    }

    bool RRuleIter::nextExclude() {
        if (!this->nextNow()) {
            return false;
        }

        if (!this->rr->excludes.empty()) {
            for (;;) {
                if (!this->rr->excluded(this->now())) {
                    break;
                }
                if (!this->nextNow()) {
                    return false;
                }
            }
        }

        return true;
    }

    bool RRuleIter::nextNow() {
        if (!this->cascade->next()) {
            return false;
        }
        this->setCurrentNow();
        return true;
    }

    void RRuleIter::setCurrentNow() {
        DateStamp now = this->cascade->value();
        this->current_now = DateTime(now, this->rr->dtstart.tz);
    }

    /*
        +----------+--------+--------+-------+-------+------+-------+------+
        |          |SECONDLY|MINUTELY|HOURLY |DAILY  |WEEKLY|MONTHLY|YEARLY|
        +----------+--------+--------+-------+-------+------+-------+------+
        |BYMONTH   |Limit   |Limit   |Limit  |Limit  |Limit |Limit  |Expand|
        |BYWEEKNO  |N/A     |N/A     |N/A    |N/A    |N/A   |N/A    |Expand|
        |BYYEARDAY |Limit   |Limit   |Limit  |N/A    |N/A   |N/A    |Expand|
        |BYMONTHDAY|Limit   |Limit   |Limit  |Limit  |N/A   |Expand |Expand|
        |BYDAY     |Limit   |Limit   |Limit  |Limit  |Expand|Note 1 |Note 2|
        |BYHOUR    |Limit   |Limit   |Limit  |Expand |Expand|Expand |Expand|
        |BYMINUTE  |Limit   |Limit   |Expand |Expand |Expand|Expand |Expand|
        |BYSECOND  |Limit   |Expand  |Expand |Expand |Expand|Expand |Expand|
        |BYSETPOS  |Limit   |Limit   |Limit  |Limit  |Limit |Limit  |Limit |
        +----------+--------+--------+-------+-------+------+-------+------+
        ref: https://icalendar.org/iCalendar-RFC-5545/3-3-10-recurrence-rule.html
    */
    void RRuleIter::setupCounters(const DateStamp& base) {

        if (this->rr->freq == RRule::Freq::SECONDLY ||
            this->rr->freq == RRule::Freq::MINUTELY ||
            this->rr->freq == RRule::Freq::HOURLY)
        {
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |        |BYMONTH|BYWEEKNO|BYYEARDAY|BYMONTHDAY|BYDAY |BYHOUR|BYMINUTE|BYSECOND|BYSETPOS|
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |SECONDLY|Limit  |N/A     |Limit    |Limit     |Limit |Limit |Limit   |Limit   |Limit   |
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |MINUTELY|Limit  |N/A     |Limit    |Limit     |Limit |Limit |Limit   |Expand  |Limit   |
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |HOURLY  |Limit  |N/A     |Limit    |Limit     |Limit |Limit |Expand  |Expand  |Limit   |
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            if (this->rr->byWeekNo.size())  throw ICalError("BYWEEKNO is N/A");

            // TODO: BYSETPOS
            if (this->rr->bySecond.size())  this->cascade->add(BySecondCounter::init(this->rr->bySecond));
            else if (this->rr->freq == RRule::Freq::SECONDLY)
                                            this->cascade->add(SecondInc::init(this->rr->interval));

            if (this->rr->byMinute.size())  this->cascade->add(ByMinuteCounter::init(this->rr->byMinute));
            else if (this->rr->freq == RRule::Freq::MINUTELY)
                                            this->cascade->add(MinuteInc::init(this->rr->interval));

            if (this->rr->byHour.size())     this->cascade->add(ByHourCounter::init(this->rr->byHour));
            else if (this->rr->freq == RRule::Freq::HOURLY)
                                            this->cascade->add(HourInc::init(this->rr->interval));

            std::vector<Counter_ptr> andCounters;
            if (this->rr->byDay.size())      andCounters.push_back(ByWeekDayCounter::init(this->rr->byDay, this->rr));
            if (this->rr->byMonthDay.size()) andCounters.push_back(ByMonthDayCounter::init(this->rr->byMonthDay));
            if (this->rr->byYearDay.size())  andCounters.push_back(ByYearDayCounter::init(this->rr->byYearDay));
            if (andCounters.size())          this->cascade->add(ByAndCounter::init(andCounters));

            if (this->rr->byMonth.size())    this->cascade->add(ByMonthCounter::init(this->rr->byMonth));
        }

        else if (this->rr->freq == RRule::Freq::DAILY) {
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |        |BYMONTH|BYWEEKNO|BYYEARDAY|BYMONTHDAY|BYDAY |BYHOUR|BYMINUTE|BYSECOND|BYSETPOS|
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |DAILY   |Limit  |N/A     |N/A      |Limit     |Limit |Expand|Expand  |Expand  |Limit   |
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            if (this->rr->byWeekNo.size())  throw ICalError("BYWEEKNO is N/A");
            if (this->rr->byYearDay.size()) throw ICalError("BYYEARDAY is N/A");

            if (this->rr->bySecond.size())  this->cascade->add(BySecondCounter::init(this->rr->bySecond));
            if (this->rr->byMinute.size())  this->cascade->add(ByMinuteCounter::init(this->rr->byMinute));
            if (this->rr->byHour.size())    this->cascade->add(ByHourCounter::init(this->rr->byHour));

            std::vector<Counter_ptr> andCounters;
            if (this->rr->byDay.size())      andCounters.push_back(ByWeekDayCounter::init(this->rr->byDay, this->rr));
            if (this->rr->byMonthDay.size()) andCounters.push_back(ByMonthDayCounter::init(this->rr->byMonthDay));
            if (andCounters.size() == 0)     andCounters.push_back(DayInc::init(this->rr->interval));
            this->cascade->add(BySetPosCounter::init(ByAndCounter::init(andCounters), this->rr->bySetPos));

            if (this->rr->byMonth.size()) {
                this->cascade->add(ByMonthCounter::init(this->rr->byMonth));
                this->cascade->add(YearInc::init(1));
            }
        }

        else if (this->rr->freq == RRule::Freq::WEEKLY) {
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |        |BYMONTH|BYWEEKNO|BYYEARDAY|BYMONTHDAY|BYDAY |BYHOUR|BYMINUTE|BYSECOND|BYSETPOS|
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |WEEKLY  |Limit  |N/A     |N/A      |N/A       |Expand|Expand|Expand  |Expand  |Limit   |
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            if (this->rr->byWeekNo.size())   throw ICalError("BYWEEKNO is N/A");
            if (this->rr->byYearDay.size())  throw ICalError("BYYEARDAY is N/A");
            if (this->rr->byMonthDay.size()) throw ICalError("BYMONTHDAY is N/A");

            // TODO: BYSETPOS
            if (this->rr->bySecond.size())  this->cascade->add(BySecondCounter::init(this->rr->bySecond));
            if (this->rr->byMinute.size())  this->cascade->add(ByMinuteCounter::init(this->rr->byMinute));
            if (this->rr->byHour.size())    this->cascade->add(ByHourCounter::init(this->rr->byHour));

            if (this->rr->byDay.size())     this->cascade->add(ByWeekDayCounter::init(this->rr->byDay, this->rr));
            else                            this->cascade->add(ByWeekDayCounter::init(RRule::Day_pair(0, base.dayOfWeek()), this->rr));

            if (this->rr->byMonth.size()) {
                std::vector<Counter_ptr> andCounters;
                if (this->rr->interval > 1) {
                    throw NotImplementedError("RRULE:FREQ=WEEKLY;INTERVAL>1;BYMONTH=... not implemented");
                    andCounters.push_back(WeekInc::init(this->rr->interval, this->rr->wkst));
                }
                andCounters.push_back(ByMonthCounter::init(this->rr->byMonth));
                this->cascade->add(ByAndCounter::init(andCounters));
                this->cascade->add(YearInc::init(1));
            }
            else {
                this->cascade->add(WeekInc::init(this->rr->interval, this->rr->wkst));
            }
        }

        else if (this->rr->freq == RRule::Freq::MONTHLY) {
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |        |BYMONTH|BYWEEKNO|BYYEARDAY|BYMONTHDAY|BYDAY |BYHOUR|BYMINUTE|BYSECOND|BYSETPOS|
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |MONTHLY |Limit  |N/A     |N/A      |Expand    |Note 1|Expand|Expand  |Expand  |Limit   |
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // Note 1: Limit if BYMONTHDAY is present; otherwise, special expand for MONTHLY.

            if (this->rr->byWeekNo.size())  throw ICalError("BYWEEKNO is N/A");
            if (this->rr->byYearDay.size()) throw ICalError("BYYEARDAY is N/A");

            if (this->rr->bySecond.size())  this->cascade->add(BySecondCounter::init(this->rr->bySecond));
            if (this->rr->byMinute.size())  this->cascade->add(ByMinuteCounter::init(this->rr->byMinute));
            if (this->rr->byHour.size())    this->cascade->add(ByHourCounter::init(this->rr->byHour));

            if (this->rr->byDay.size()) {
                std::vector<Counter_ptr> andCounters;
                andCounters.push_back(ByWeekDayCounter::init(this->rr->byDay, this->rr));

                if (this->rr->byMonthDay.size()) {
                    // Limit: BYDAY
                    andCounters.push_back(ByMonthDayCounter::init(this->rr->byMonthDay));
                }
                this->cascade->add(BySetPosCounter::init(ByAndCounter::init(andCounters), this->rr->bySetPos));
            }
            else {
                if (this->rr->byMonthDay.size()) {
                    this->cascade->add(ByMonthDayCounter::init(this->rr->byMonthDay));
                }
            }

            if (this->rr->byMonth.size()) {
                this->cascade->add(ByMonthCounter::init(this->rr->byMonth));
                this->cascade->add(YearInc::init(1));
            }
            else {
                this->cascade->add(MonthInc::init(this->rr->interval));
            }
        }

        else if (this->rr->freq == RRule::Freq::YEARLY) {
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |        |BYMONTH|BYWEEKNO|BYYEARDAY|BYMONTHDAY|BYDAY |BYHOUR|BYMINUTE|BYSECOND|BYSETPOS|
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // |YEARLY  |Expand |Expand  |Expand   |Expand    |Note 2|Expand|Expand  |Expand  |Limit   |
            // +--------+-------+--------+---------+----------+------+------+--------+--------+--------+
            // Note 2: Limit if BYYEARDAY or BYMONTHDAY is present; otherwise, special expand for WEEKLY
            //         if BYWEEKNO present; otherwise, special expand for MONTHLY if BYMONTH present;
            //         otherwise, special expand for YEARLY.

            // TODO: BYSETPOS
            if (this->rr->bySecond.size())   this->cascade->add(BySecondCounter::init(this->rr->bySecond));
            if (this->rr->byMinute.size())   this->cascade->add(ByMinuteCounter::init(this->rr->byMinute));
            if (this->rr->byHour.size())     this->cascade->add(ByHourCounter::init(this->rr->byHour));

            if (this->rr->byDay.size()) {
                std::vector<Counter_ptr> andCounters;
                std::vector<Counter_ptr> extraCounters;
                andCounters.push_back(ByWeekDayCounter::init(this->rr->byDay, this->rr));

                if (this->rr->byYearDay.size()) {
                    // Limit: BYDAY
                    andCounters.push_back(ByYearDayCounter::init(this->rr->byYearDay));
                }
                if (this->rr->byMonthDay.size()) {
                    // Limit: BYDAY
                    andCounters.push_back(ByMonthDayCounter::init(this->rr->byMonthDay));
                }
                if (this->rr->byWeekNo.size()) {
                    // Expand: WEEKLY
                    extraCounters.push_back(ByWeekNoCounter::init(this->rr->byWeekNo));
                }
                if (this->rr->byMonth.size()) {
                    // Expand: MONTHLY
                    extraCounters.push_back(ByMonthCounter::init(this->rr->byMonth));
                }
                this->cascade->add(ByAndCounter::init(andCounters));
                this->cascade->add(extraCounters);
            }
            else {
                if (this->rr->byYearDay.size()) {
                    this->cascade->add(ByYearDayCounter::init(this->rr->byYearDay));
                }
                if (this->rr->byMonthDay.size()) {
                    this->cascade->add(ByMonthDayCounter::init(this->rr->byMonthDay));
                }
                if (this->rr->byWeekNo.size()) {
                    this->cascade->add(ByWeekNoCounter::init(this->rr->byWeekNo));
                }
                if (this->rr->byMonth.size()) {
                    this->cascade->add(ByMonthDayCounter::init(base.day));
                    this->cascade->add(ByMonthCounter::init(this->rr->byMonth));
                }
            }

            this->cascade->add(YearInc::init(this->rr->interval));
        }
    }

    void RRuleIter::str(ostream& out) const {
        this->cascade->str(out);
    }
}
