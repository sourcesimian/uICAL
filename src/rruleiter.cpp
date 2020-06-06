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

namespace uICAL {
    RRuleIter::RRuleIter(const RRule_ptr rr, const DateTime& begin, const DateTime& end)
    : rr(rr)
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

        if (!this->initCounters(base, begin)) {
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
        if (this->counters.size() == 0) {
            throw RecurrenceError("Not yet initialised, call next() first");
        }

        throw RecurrenceError("No more occurrences");
    }

    bool RRuleIter::next() {
        if (this->counters.size() == 0) {
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

        if (this->rr->excludes.size()) {
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
        if (!this->nextDate()) {
            return false;
        }
        this->setCurrentNow();
        return true;
    }

    void RRuleIter::setCurrentNow() {
        DateStamp now = this->counters.front()->value();
        this->current_now = DateTime(now, this->rr->dtstart.tz);
    }

    void RRuleIter::setupCounters(const DateStamp& base) {
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

        if (this->rr->freq == RRule::Freq::SECONDLY ||
            this->rr->freq == RRule::Freq::MINUTELY ||
            this->rr->freq == RRule::Freq::HOURLY)
        {
            if (this->rr->byWeekNo.size())   throw ICalError("BYWEEKNO is N/A");

            // TODO: BYSETPOS
            if (this->rr->bySecond.size())  this->counters.push_back(BySecondCounter::init(this->rr->bySecond));
            else if (this->rr->freq == RRule::Freq::SECONDLY)
                                            this->counters.push_back(SecondInc::init(this->rr->interval));
            else                            this->counters.push_back(BySecondCounter::init(base.second));

            if (this->rr->byMinute.size())  this->counters.push_back(ByMinuteCounter::init(this->rr->byMinute));
            else if (this->rr->freq == RRule::Freq::MINUTELY)
                                            this->counters.push_back(MinuteInc::init(this->rr->interval));
            else if (this->rr->freq != RRule::Freq::SECONDLY)
                                            this->counters.push_back(ByMinuteCounter::init(base.minute));

            if (this->rr->byHour.size())     this->counters.push_back(ByHourCounter::init(this->rr->byHour));
            else if (this->rr->freq == RRule::Freq::HOURLY)
                                            this->counters.push_back(HourInc::init(this->rr->interval));

            std::vector<Counter_ptr> andCounters;
            if (this->rr->byDay.size())      andCounters.push_back(ByWeekDayCounter::init(this->rr->byDay, this->rr));
            if (this->rr->byMonthDay.size()) andCounters.push_back(ByMonthDayCounter::init(this->rr->byMonthDay));
            if (this->rr->byYearDay.size())  andCounters.push_back(ByYearDayCounter::init(this->rr->byYearDay));
            if (andCounters.size())          this->counters.push_back(ByAndCounter::init(andCounters));

            if (this->rr->byMonth.size())    this->counters.push_back(ByMonthCounter::init(this->rr->byMonth));
        }

        else if (this->rr->freq == RRule::Freq::DAILY) {
            if (this->rr->byWeekNo.size())   throw ICalError("BYWEEKNO is N/A");
            if (this->rr->byYearDay.size())  throw ICalError("BYYEARDAY is N/A");

            // TODO: BYSETPOS
            if (this->rr->bySecond.size())  this->counters.push_back(BySecondCounter::init(this->rr->bySecond));
            else                            this->counters.push_back(BySecondCounter::init(base.second));
            if (this->rr->byMinute.size())  this->counters.push_back(ByMinuteCounter::init(this->rr->byMinute));
            else                            this->counters.push_back(ByMinuteCounter::init(base.minute));
            if (this->rr->byHour.size())    this->counters.push_back(ByHourCounter::init(this->rr->byHour));
            else                            this->counters.push_back(ByHourCounter::init(base.hour));

            std::vector<Counter_ptr> andCounters;
            if (this->rr->byDay.size())      andCounters.push_back(ByWeekDayCounter::init(this->rr->byDay, this->rr));
            if (this->rr->byMonthDay.size()) andCounters.push_back(ByMonthDayCounter::init(this->rr->byMonthDay));
            if (andCounters.size() == 0)    andCounters.push_back(DayInc::init(this->rr->interval));
            if (andCounters.size())         this->counters.push_back(ByAndCounter::init(andCounters));

            if (this->rr->byMonth.size()) {
                                        this->counters.push_back(ByMonthCounter::init(this->rr->byMonth));
                                        this->counters.push_back(YearInc::init(1));
            }
        }

        else if (this->rr->freq == RRule::Freq::WEEKLY) {
            if (this->rr->byWeekNo.size())      throw ICalError("BYWEEKNO is N/A");
            if (this->rr->byYearDay.size())     throw ICalError("BYYEARDAY is N/A");
            if (this->rr->byMonthDay.size())    throw ICalError("BYMONTHDAY is N/A");

            // TODO: BYSETPOS
            if (this->rr->bySecond.size())   this->counters.push_back(BySecondCounter::init(this->rr->bySecond));
            else                            this->counters.push_back(BySecondCounter::init(base.second));
            if (this->rr->byMinute.size())   this->counters.push_back(ByMinuteCounter::init(this->rr->byMinute));
            else                            this->counters.push_back(ByMinuteCounter::init(base.minute));
            if (this->rr->byHour.size())     this->counters.push_back(ByHourCounter::init(this->rr->byHour));
            else                            this->counters.push_back(ByHourCounter::init(base.hour));

            if (this->rr->byDay.size())     this->counters.push_back(ByWeekDayCounter::init(this->rr->byDay, this->rr));
            else                            this->counters.push_back(ByWeekDayCounter::init(RRule::Day_pair(0, base.dayOfWeek()), this->rr));

            if (this->rr->byMonth.size())   {
                this->counters.push_back(ByMonthCounter::init(this->rr->byMonth));
                this->counters.push_back(YearInc::init(1));
            }
            else {
                this->counters.push_back(WeekInc::init(this->rr->interval, this->rr->wkst));
            }
        }

        else if (this->rr->freq == RRule::Freq::MONTHLY) {
            if (this->rr->byWeekNo.size())   throw ICalError("BYWEEKNO is N/A");
            if (this->rr->byYearDay.size())  throw ICalError("BYYEARDAY is N/A");

            if (this->rr->bySecond.size())  this->counters.push_back(BySecondCounter::init(this->rr->bySecond));
            else                            this->counters.push_back(BySecondCounter::init(base.second));
            if (this->rr->byMinute.size())  this->counters.push_back(ByMinuteCounter::init(this->rr->byMinute));
            else                            this->counters.push_back(ByMinuteCounter::init(base.minute));
            if (this->rr->byHour.size())    this->counters.push_back(ByHourCounter::init(this->rr->byHour));
            else                            this->counters.push_back(ByHourCounter::init(base.hour));

            std::vector<Counter_ptr> andCounters;
            if (this->rr->byDay.size())      andCounters.push_back(ByWeekDayCounter::init(this->rr->byDay, this->rr));
            if (this->rr->byMonthDay.size()) andCounters.push_back(ByMonthDayCounter::init(this->rr->byMonthDay));
            if (andCounters.size())         this->counters.push_back(BySetPosCounter::init(ByAndCounter::init(andCounters), this->rr->bySetPos));
            else                            this->counters.push_back(ByMonthDayCounter::init(base.day));

            if (this->rr->byMonth.size()) {
                                            this->counters.push_back(ByMonthCounter::init(this->rr->byMonth));
                                            this->counters.push_back(YearInc::init(1));
            }
            else                            this->counters.push_back(MonthInc::init(this->rr->interval));
        }

        else if (this->rr->freq == RRule::Freq::YEARLY) {
            // TODO: BYSETPOS
            if (this->rr->bySecond.size())   this->counters.push_back(BySecondCounter::init(this->rr->bySecond));
            else                            this->counters.push_back(BySecondCounter::init(base.second));
            if (this->rr->byMinute.size())   this->counters.push_back(ByMinuteCounter::init(this->rr->byMinute));
            else                            this->counters.push_back(ByMinuteCounter::init(base.minute));
            if (this->rr->byHour.size())     this->counters.push_back(ByHourCounter::init(this->rr->byHour));
            else                            this->counters.push_back(ByHourCounter::init(base.hour));

            std::vector<Counter_ptr> andCounters;
            if (this->rr->byDay.size())      andCounters.push_back(ByWeekDayCounter::init(this->rr->byDay, this->rr));
            if (this->rr->byMonthDay.size()) andCounters.push_back(ByMonthDayCounter::init(this->rr->byMonthDay));
            if (this->rr->byYearDay.size())  andCounters.push_back(ByYearDayCounter::init(this->rr->byYearDay));
            if (andCounters.size())         this->counters.push_back(ByAndCounter::init(andCounters));
            else                            this->counters.push_back(ByMonthDayCounter::init(base.day));

            if (this->rr->byWeekNo.size())   this->counters.push_back(ByWeekNoCounter::init(this->rr->byWeekNo));
            if (this->rr->byMonth.size())    this->counters.push_back(ByMonthCounter::init(this->rr->byMonth));
            else if (!this->rr->byWeekNo.size())
                                            this->counters.push_back(ByMonthCounter::init(base.month));

            this->counters.push_back(YearInc::init(this->rr->interval));
        }
    }

    bool RRuleIter::initCounters(const DateStamp& base, const DateStamp& begin) {
        counters_t::iterator it = this->counters.end();
        -- it;
        if(!(*it)->reset(base)) {
            return false;
        }

        return this->resetCascade(it, [&](counters_t::iterator it) {
            while (!(*it)->syncLock(begin, (*it)->value())) {
                if (!(*it)->next()) {
                    ostream out;
                    out << "Can not seek " << (*it)->name() << " (" << "base: " << base << " from: " << begin << ")";
                    throw ParseError(out);
                }
            }
        });
    }

    bool RRuleIter::resetCascade(counters_t::iterator it, sync_f sync) {
        DateStamp base = (*it)->value();
        for (;;) {
            if (it == this->counters.begin())
                break;
            -- it;
            if (!(*it)->reset(base)) {
                ++ it;
                if(!(*it)->next()) {
                    ++ it;
                    if (it != this->counters.end()) {
                        throw ImplementationError("No next available from upper level counter");
                    }
                    -- it;
                }
            }

            if (sync != nullptr) {
                sync(it);
            }

            if (it == this->counters.begin())
                break;
            base = (*it)->value();
            // if (this->expired(base)) {
            //     return false;
            // }
        }
        return true;
    }

    bool RRuleIter::nextDate() {
        counters_t::iterator it = this->counters.begin();

        if(!(*it)->next()) {
            for (;;) {
                ++ it;
                if (it == this->counters.end()) {
                    -- it;
                    if (it == this->counters.begin())
                        return true;
                    break;
                }
                if ((*it)->next())
                    break;
            }
            if (!this->resetCascade(it, nullptr)) {
                return false;
            }
        }
        return true;
    }

    void RRuleIter::str(ostream& out) const {
        Joiner counters(',');
        for (auto counter : this->counters) {
            counter->str(counters.out());
            counters.next();
        }
        counters.str(out);
    }
}
