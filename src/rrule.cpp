#include "uICAL/cppstl.h"
#include "uICAL/util.h"
#include "uICAL/bysetposcounter.h"
#include "uICAL/byweekdaycounter.h"
#include "uICAL/byandcounter.h"
#include "uICAL/counter.h"
#include "uICAL/error.h"
#include "uICAL/rrule.h"
#include "uICAL/rruleparser.h"

namespace uICAL {
    RRule::ptr RRule::init(const std::string rrule, const DateTime dtstart) {
        return RRule::ptr(new RRule(RRuleParser::init(rrule), dtstart));
    }

    RRule::RRule(const RRuleParser::ptr parser, const DateTime dtstart)
    : p(parser)
    , dtstart(dtstart)
    {
        this->count = 0;
    }

    void RRule::begin(const DateTime begin) {
        this->range_begin = begin;
    }

    void RRule::end(const DateTime end) {
        this->range_end = end;
    }

    void RRule::exclude(const DateTime exclude) {
        this->excludes.push_back(exclude);
    }

    bool RRule::init() {
        if (!this->range_end.empty() && this->dtstart > this->range_end)  {
            return false;
        }

        if (!this->range_begin.empty() && !this->until.empty() && this->until < this->range_begin) {
            return false;
        }

        DateStamp base = this->dtstart.datestamp();
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
        this->count = this->p->count;

        if (!this->range_begin.empty()) {
            while (this->now() < this->range_begin) {
                if (!this->next()) {
                    return false;
                }
            }
        }
        return true;
    }

    DateTime RRule::now() const {
        if (this->count != 0) {
            DateStamp now = this->counters.front()->value();
            return DateTime(now, this->dtstart.tz);
        }
        if (this->counters.size() == 0) {
            throw RecurrenceError("Not yet initialised, call next() first");
        }

        throw RecurrenceError("No more occurrences");
    }

    bool RRule::next() {
        if (this->counters.size() == 0) {
            return this->init();
        }

        if (this->count > 0) {
            this->count --;
        }
        if (this->count == 0) {
            return false;
        }

        if(!this->nextDate()) {
            return false;
        }

        if (this->excludes.size()) {
            for (;;) {
                DateTime now = this->now();
                auto it = std::find(this->excludes.begin(), this->excludes.end(), now);
                if (it == this->excludes.end()) {
                    break;
                }
                if (!this->nextDate()) {
                    return false;
                }
            }
        }

        if (this->expired(this->counters.front()->value())) {
            this->count = 0;
            return false;
        }

        return true;
    }

    bool RRule::expired(DateStamp current) const {
        
        if (!this->until.empty() && DateTime(current, this->dtstart.tz) > this->until) {
            return true;
        }
        if (!this->range_end.empty() && DateTime(current, this->dtstart.tz) > this->range_end) {
            return true;
        }
        return false;
    }

    void RRule::setupCounters(DateStamp base) {
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
        
        if (this->p->freq == RRuleParser::Freq::SECONDLY ||
            this->p->freq == RRuleParser::Freq::MINUTELY ||
            this->p->freq == RRuleParser::Freq::HOURLY)
        {
            // TODO: BYSETPOS
            if (this->p->bySecond.size())   this->counters.push_back(BySecondCounter::init(this->p->bySecond));
            else if (this->p->freq == RRuleParser::Freq::SECONDLY)
                                            this->counters.push_back(SecondInc::init(this->p->interval));
            else                            this->counters.push_back(BySecondCounter::init(base.second));

            if (this->p->byMinute.size())   this->counters.push_back(ByMinuteCounter::init(this->p->byMinute));
            else if (this->p->freq == RRuleParser::Freq::MINUTELY)
                                            this->counters.push_back(MinuteInc::init(this->p->interval));
            else                            this->counters.push_back(ByMinuteCounter::init(base.minute));

            if (this->p->byHour.size())     this->counters.push_back(ByHourCounter::init(this->p->byHour));
            else if (this->p->freq == RRuleParser::Freq::HOURLY)
                                            this->counters.push_back(HourInc::init(this->p->interval));

            std::vector<Counter::ptr> dayCounters;
            if (this->p->byDay.size())      dayCounters.push_back(ByWeekDayCounter::init(this->p->byDay, this->p));
            if (this->p->byMonthDay.size()) dayCounters.push_back(ByMonthDayCounter::init(this->p->byMonthDay));
            if (this->p->byYearDay.size())  dayCounters.push_back(ByYearDayCounter::init(this->p->byYearDay));
            if (dayCounters.size())         this->counters.push_back(ByAndCounter::init(dayCounters));

            if (this->p->byWeekNo.size())   throw ICalError("BYWEEKNO is N/A");
            if (this->p->byMonth.size())    this->counters.push_back(ByMonthCounter::init(this->p->byMonth));
        }

        else if (this->p->freq == RRuleParser::Freq::DAILY) {
            // TODO: BYSETPOS
            if (this->p->bySecond.size())   this->counters.push_back(BySecondCounter::init(this->p->bySecond));
            else                            this->counters.push_back(BySecondCounter::init(base.second));
            if (this->p->byMinute.size())   this->counters.push_back(ByMinuteCounter::init(this->p->byMinute));
            else                            this->counters.push_back(ByMinuteCounter::init(base.minute));
            if (this->p->byHour.size())     this->counters.push_back(ByHourCounter::init(this->p->byHour));
            else                            this->counters.push_back(ByHourCounter::init(base.hour));

            std::vector<Counter::ptr> dayCounters;
            if (this->p->byDay.size())      dayCounters.push_back(ByWeekDayCounter::init(this->p->byDay, this->p));
            if (this->p->byMonthDay.size()) dayCounters.push_back(ByMonthDayCounter::init(this->p->byMonthDay));
            if (this->p->byYearDay.size())  throw ICalError("BYYEARDAY is N/A");
            if (dayCounters.size() == 0)    dayCounters.push_back(DayInc::init(this->p->interval));
            if (dayCounters.size())         this->counters.push_back(ByAndCounter::init(dayCounters));

            if (this->p->byWeekNo.size())   throw ICalError("BYWEEKNO is N/A for SECONDLY");
            if (this->p->byMonth.size()) {
                                        this->counters.push_back(ByMonthCounter::init(this->p->byMonth));
                                        this->counters.push_back(YearInc::init(1));
            }
        }

        else if (this->p->freq == RRuleParser::Freq::WEEKLY) {
            // TODO: BYSETPOS
            if (this->p->bySecond.size())   this->counters.push_back(BySecondCounter::init(this->p->bySecond));
            else                            this->counters.push_back(BySecondCounter::init(base.second));
            if (this->p->byMinute.size())   this->counters.push_back(ByMinuteCounter::init(this->p->byMinute));
            else                            this->counters.push_back(ByMinuteCounter::init(base.minute));
            if (this->p->byHour.size())     this->counters.push_back(ByHourCounter::init(this->p->byHour));
            else                            this->counters.push_back(ByHourCounter::init(base.hour));

            std::vector<Counter::ptr> dayCounters;
            if (this->p->byDay.size())      dayCounters.push_back(ByWeekDayCounter::init(this->p->byDay, this->p));
            if (this->p->byMonthDay.size()) throw ICalError("BYMONTHDAY is N/A");
            if (this->p->byYearDay.size())  throw ICalError("BYYEARDAY is N/A");
            if (dayCounters.size())         this->counters.push_back(ByAndCounter::init(dayCounters));
            else                            this->counters.push_back(ByWeekDayCounter::init(RRuleParser::Day_pair(0, base.dayOfWeek()), this->p));

            this->counters.push_back(WeekInc::init(this->p->interval, this->p->wkst));

            if (this->p->byWeekNo.size())   throw ICalError("BYWEEKNO is N/A for SECONDLY");
            if (this->p->byMonth.size()) {
                                            this->counters.push_back(ByMonthCounter::init(this->p->byMonth));
                                            this->counters.push_back(YearInc::init(1));
            }
        }

        else if (this->p->freq == RRuleParser::Freq::MONTHLY) {
            if (this->p->bySecond.size())   this->counters.push_back(BySecondCounter::init(this->p->bySecond));
            else                            this->counters.push_back(BySecondCounter::init(base.second));
            if (this->p->byMinute.size())   this->counters.push_back(ByMinuteCounter::init(this->p->byMinute));
            else                            this->counters.push_back(ByMinuteCounter::init(base.minute));
            if (this->p->byHour.size())     this->counters.push_back(ByHourCounter::init(this->p->byHour));
            else                            this->counters.push_back(ByHourCounter::init(base.hour));

            std::vector<Counter::ptr> dayCounters;
            if (this->p->byDay.size())      dayCounters.push_back(ByWeekDayCounter::init(this->p->byDay, this->p));
            if (this->p->byMonthDay.size()) dayCounters.push_back(ByMonthDayCounter::init(this->p->byMonthDay));
            if (this->p->byYearDay.size())  throw ICalError("BYYEARDAY is N/A");
            if (dayCounters.size())         this->counters.push_back(BySetPosCounter::init(ByAndCounter::init(dayCounters), this->p->bySetPos));
            else                            this->counters.push_back(ByMonthDayCounter::init(base.day));

            if (this->p->byWeekNo.size())   throw ICalError("BYWEEKNO is N/A for SECONDLY");
            if (this->p->byMonth.size()) {
                                            this->counters.push_back(ByMonthCounter::init(this->p->byMonth));
                                            this->counters.push_back(YearInc::init(1));
            }
            else                            this->counters.push_back(MonthInc::init(this->p->interval));
        }

        else if (this->p->freq == RRuleParser::Freq::YEARLY) {
            // TODO: BYSETPOS
            if (this->p->bySecond.size())   this->counters.push_back(BySecondCounter::init(this->p->bySecond));
            else                            this->counters.push_back(BySecondCounter::init(base.second));
            if (this->p->byMinute.size())   this->counters.push_back(ByMinuteCounter::init(this->p->byMinute));
            else                            this->counters.push_back(ByMinuteCounter::init(base.minute));
            if (this->p->byHour.size())     this->counters.push_back(ByHourCounter::init(this->p->byHour));
            else                            this->counters.push_back(ByHourCounter::init(base.hour));

            std::vector<Counter::ptr> dayCounters;
            if (this->p->byDay.size())      dayCounters.push_back(ByWeekDayCounter::init(this->p->byDay, this->p));
            if (this->p->byMonthDay.size()) dayCounters.push_back(ByMonthDayCounter::init(this->p->byMonthDay));
            if (this->p->byYearDay.size())  dayCounters.push_back(ByYearDayCounter::init(this->p->byYearDay));
            if (dayCounters.size())         this->counters.push_back(ByAndCounter::init(dayCounters));
            else                            this->counters.push_back(ByMonthDayCounter::init(base.day));

            if (this->p->byWeekNo.size())   this->counters.push_back(ByWeekNoCounter::init(this->p->byWeekNo));
            if (this->p->byMonth.size())    this->counters.push_back(ByMonthCounter::init(this->p->byMonth));
            else if (!this->p->byWeekNo.size())
                                            this->counters.push_back(ByMonthCounter::init(base.month));

            this->counters.push_back(YearInc::init(this->p->interval));
        }
    }

    bool RRule::initCounters(DateStamp base, DateStamp begin) {
        counters_t::iterator it = this->counters.end();
        -- it;
        if(!(*it)->reset(base)) {
            return false;
        }

        return this->resetCascade(it, [&](counters_t::iterator it) {
            while (!(*it)->syncLock(begin, (*it)->value())) {
                if (!(*it)->next()) {
                    std::ostringstream out;
                    out << "Can not seek " << (*it)->name() << " (" << "base: " << base << " from: " << begin << ")";
                    throw ParseError(out.str());
                }
            }
        });
    }

    bool RRule::resetCascade(counters_t::iterator it, sync_f sync) {
        DateStamp base = (*it)->value();
        for (;;) {
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
            if (this->expired(base)) {
                return false;
            }
        } 
        return true;
    }

    bool RRule::nextDate() {
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

    void RRule::str(std::ostream& out) const {
        Joiner counters(',');
        for (auto it = this->counters.rbegin(); it != this->counters.rend(); ++it) {
            (*it)->str(counters.out());
            counters.next();
        }
        counters.str(out);
    }
}
