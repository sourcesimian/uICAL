/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/datetime.h"
#include "uICAL/icalevent.h"
#include "uICAL/icalcomponent.h"

namespace uICAL {
    ICalEvent::ptr ICalEvent::init(VComponent::ptr& event, const TZMap::ptr& tzmap) {
        return ptr(new ICalEvent(event, tzmap));
    }

    ICalEvent::ICalEvent(VComponent::ptr& event, const TZMap::ptr& tzmap) {

        VLine::ptr dtStart = event->getPropertyByName("DTSTART");
        VLine::ptr dtEnd = event->getPropertyByName("DTEND");
        VLine::ptr rRule = event->getPropertyByName("RRULE");
        VLine::ptr summary = event->getPropertyByName("SUMMARY");
        
        this->start = DateTime(dtStart->value + dtStart->getParam("TZID"), tzmap);
        this->end = DateTime(dtEnd->value + dtStart->getParam("TZID"), tzmap);
        
        this->summary = summary->value;

        this->rrule = RRule::init(rRule->value, this->start);
    }

    string ICalEvent::str() const {
        std::ostringstream out;
        this->str(out);
        return out;
    }

    void ICalEvent::str(std::ostream& out) const {
        out << "VEVENT: " << this->summary << uICAL::endl;
        out << " - start: " << this->start << uICAL::endl;
        out << " - end: " << this->end << uICAL::endl;
        out << " - rrule: " << this->rrule << uICAL::endl;
    }

    std::ostream& operator << (std::ostream& out, const ICalEvent::ptr& e) {
        e->str(out);
        return out;
    }

    std::ostream& operator << (std::ostream& out, const ICalEvent& e) {
        e.str(out);
        return out;
    }

    ICalEventIter::ptr ICalEventIter::init(const ICalEvent::ptr ice, DateTime begin, DateTime end) {
        return ptr(new ICalEventIter(ice, begin, end));
    }

    ICalEventIter::ICalEventIter(const ICalEvent::ptr ice, DateTime begin, DateTime end)
    : ice(ice)
    {
        this->range_begin = begin;
        this->rrule = RRuleIter::init(this->ice->rrule, DateTime(), end);
    }

    bool ICalEventIter::next() {
        if (! this->range_begin.valid()) {
            return this->rrule->next();
        }
        DatePeriod span = this->ice->end - this->ice->start;
        while (this->rrule->next())
        {
            DateTime end = this->rrule->now() + span;

            if (end <= this->range_begin)
                continue;
            return true;
        }
        return false;
    }

    DateTime ICalEventIter::now() const {
        return this->rrule->now();
    }

    CalendarEntry::ptr ICalEventIter::entry() const {
        return CalendarEntry::init(CalendarEntry::Type::EVENT,
                     this->ice->summary,
                     this->rrule->now(),
                     this->ice->end - this->ice->start);
    }

    bool operator < (const ICalEventIter::ptr& a, const ICalEventIter::ptr& b) {
        return a->rrule->now() < b->rrule->now();
    }

}
