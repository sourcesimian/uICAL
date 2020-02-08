#include "uICAL/cppstl.h"
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
        
        // this->start = DateTime(DateStamp(dtStart->value), TZbyId::init(tzmap, dtStart->getParam("TZID")));
        // this->end = DateTime(DateStamp(dtEnd->value), TZbyId::init(tzmap, dtStart->getParam("TZID")));
        this->start = DateTime(dtStart->value + dtStart->getParam("TZID"), tzmap);
        this->end = DateTime(dtEnd->value + dtStart->getParam("TZID"), tzmap);
        
        this->summary = summary->value;

        this->rrule = RRule::init(rRule->value, this->start);
    }

    void ICalEvent::str(std::ostream& out) const {
        out << "VEVENT: " << this->summary << std::endl;
        out << " - start: " << this->start << std::endl;
        out << " - end: " << this->end << std::endl;
        out << " - rrule: " << this->rrule << std::endl;
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
        this->rrule = RRuleIter::init(this->ice->rrule, begin, end);
    }

    bool ICalEventIter::next() {
        return this->rrule->next();
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
