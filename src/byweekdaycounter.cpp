/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/rrule.h"
#include "uICAL/byweekdaycounter.h"

namespace uICAL {
    const unsigned ByWeekDayCounter::no_offset = 400;

    Counter_ptr ByWeekDayCounter::init(const values_t& values, const RRule_ptr& p) {
        return Counter_ptr((Counter*)new ByWeekDayCounter(values, p));
    }

    Counter_ptr ByWeekDayCounter::init(const value_t value, const RRule_ptr& p) {
        values_t values{value};
        return Counter_ptr((Counter*)new ByWeekDayCounter(values, p));
    }

    ByWeekDayCounter::ByWeekDayCounter(const values_t& values, const RRule_ptr& p)
    : CounterT(values)
    , p(p)
    {
        for (RRule::Day_pair id : values) {
            int index;
            DateTime::Day dayOfWeek;
            unpack(id, index, dayOfWeek);

            if (index) {
                this->byIndexedDay.push_back(id);
            }
            else {
                this->byDayOfWeek.push_back(dayOfWeek);
            }
        }
        this->wrap();
    }

    bool ByWeekDayCounter::syncLock(const DateStamp& from, const DateStamp& now) const {
        return from.day <= now.day;
    }

    void ByWeekDayCounter::wrap() {
        this->cursor = 0;

        if (this->byIndexedDay.size()) {
            this->itIndex = this->byIndexedDay.begin();
        }
        else {
            this->itIndex = this->byIndexedDay.end();
        }
    }

    bool ByWeekDayCounter::reset(const DateStamp& base) {
        this->base = base;
        this->wrap();
        this->cursor = 0;

        if (this->p->byWeekNo.size()) {
            this->span = 7;
            this->base = this->base;
            this->base.decDay(DateTime::daysUntil(this->p->wkst, this->base.dayOfWeek()));
        }
        else if (this->p->byMonth.size()) {
            this->span = this->base.daysInMonth();
            this->base.day = 1;
        }
        else if (this->p->freq == RRule::Freq::DAILY ||
            this->p->freq == RRule::Freq::MONTHLY
        ) {
            this->span = this->base.daysInMonth();
            this->base.day = 1;
        }
        else if (this->p->freq == RRule::Freq::YEARLY) {
            this->span = this->base.daysInYear();
            this->base.day = 1;
            this->base.month = 1;
        }
        else if (this->p->freq == RRule::Freq::WEEKLY) {
            this->span = 7;
            this->base = this->base;
            this->base.decDay(DateTime::daysUntil(this->p->wkst, this->base.dayOfWeek()));
        }
        else {
            throw NotImplementedError("other not implemented");
        }
        this->firstDayOfSpan = this->base.dayOfWeek();

        if (!this->next()) {
            return false;
        }
        return true;
    }

    bool ByWeekDayCounter::next() {
        this->span_offset = no_offset;
        if (this->byDayOfWeek.size())
        {
            DateTime::Day cursorFirstDay = DateTime::dayOfWeekAfter(this->firstDayOfSpan, this->cursor);
            std::vector<unsigned> offsets;
            for (auto dayOfWeek : this->byDayOfWeek) {
                unsigned offset = DateTime::daysUntil(cursorFirstDay, dayOfWeek);
                if ((this->cursor + offset) < this->span) {
                    this->span_offset = std::min(this->span_offset, this->cursor + offset);
                }
            }
        }

        if (this->itIndex != this->byIndexedDay.end()) {
            unsigned offset;
            for (;;) {
                int index;
                DateTime::Day dayOfWeek;
                unpack(*this->itIndex, index, dayOfWeek);

                offset = DateTime::daysUntil(this->firstDayOfSpan, index, dayOfWeek, this->span);
                if (offset < this->cursor) { // In first cycle expect to start part way
                    ++this->itIndex;
                    if (this->itIndex != this->byIndexedDay.end()) {
                        continue;
                    }
                    offset = no_offset;
                }
                break;
            }

            if (offset != no_offset) {
                if (offset <= this->span_offset) {
                    this->span_offset = offset;
                    ++this->itIndex;
                }
            }
        }

        if (this->span_offset == no_offset) {
            this->wrap();
            return false;
        }
        this->cursor = this->span_offset + 1;

        return true;
    }

    DateStamp ByWeekDayCounter::value() const {
        if (this->span_offset == no_offset)
            throw RecurrenceError("No value avaiable");
        DateStamp ds = this->base;
        ds.incDay(this->span_offset);
        return ds;
    }
}
