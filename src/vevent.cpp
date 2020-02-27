/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/datetime.h"
#include "uICAL/vevent.h"
#include "uICAL/vcomponent.h"

namespace uICAL {
    VEvent::ptr VEvent::init(const VComponent::ptr& event, const TZMap::ptr& tzmap) {
        return ptr(new VEvent(event, tzmap));
    }

    VEvent::VEvent(const VComponent::ptr& event, const TZMap::ptr& tzmap) {

        VLine::ptr dtStart = event->getPropertyByName("DTSTART");
        VLine::ptr dtEnd = event->getPropertyByName("DTEND");
        VLine::ptr rRule = event->getPropertyByName("RRULE");
        VLine::ptr summary = event->getPropertyByName("SUMMARY");
        
        this->start = DateTime(dtStart->value + dtStart->getParam("TZID"), tzmap);
        this->end = DateTime(dtEnd->value + dtStart->getParam("TZID"), tzmap);
        
        this->summary = summary->value;

        this->rrule = RRule::init(rRule->value, this->start);
    }

    void VEvent::str(ostream& out) const {
        out << "VEVENT: " << this->summary << uICAL::endl;
        out << " - start: " << this->start << uICAL::endl;
        out << " - end: " << this->end << uICAL::endl;
        out << " - rrule: " << this->rrule << uICAL::endl;
    }

    VEventIter::ptr VEventIter::init(const VEvent::ptr ice, DateTime begin, DateTime end) {
        return ptr(new VEventIter(ice, begin, end));
    }

    VEventIter::VEventIter(const VEvent::ptr ice, DateTime begin, DateTime end)
    : ice(ice)
    {
        this->range_begin = begin;
        this->rrule = RRuleIter::init(this->ice->rrule, DateTime(), end);
    }

    bool VEventIter::next() {
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

    DateTime VEventIter::now() const {
        return this->rrule->now();
    }

    CalendarEntry::ptr VEventIter::entry() const {
        return CalendarEntry::init(CalendarEntry::Type::EVENT,
                     this->ice->summary,
                     this->rrule->now(),
                     this->ice->end - this->ice->start);
    }

    bool operator < (const VEventIter::ptr& a, const VEventIter::ptr& b) {
        return a->rrule->now() < b->rrule->now();
    }

}
