#include "uICAL/cppstl.h"
#include "uICAL/util.h"
#include "uICAL/datetime.h"
#include "uICAL/icalevent.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/tzbyid.h"

namespace uICAL {
    ICalEvent::ptr ICalEvent::init(VComponent::ptr& event, const TZIdMap::ptr& tzidmap) {
        return ptr(new ICalEvent(event, tzidmap));
    }

    ICalEvent::ICalEvent(VComponent::ptr& event, const TZIdMap::ptr& tzidmap) {

        VLine::ptr dtStart = event->getPropertyByName("DTSTART");
        VLine::ptr dtEnd = event->getPropertyByName("DTEND");
        VLine::ptr rRule = event->getPropertyByName("RRULE");
        VLine::ptr summary = event->getPropertyByName("SUMMARY");
        
        this->start = DateTime(DateStamp(dtStart->value), TZbyId::init(tzidmap, dtStart->getParam("TZID")));
        this->end = DateTime(DateStamp(dtEnd->value), TZbyId::init(tzidmap, dtStart->getParam("TZID")));
        
        this->rrule = RRuleIter::init(RRule::init(rRule->value, this->start));

        this->summary = summary->value;
    }

    bool ICalEvent::next() {
        return this->rrule->next();
    }

    DateTime ICalEvent::now() const {
        return this->rrule->now();
    }

    Entry ICalEvent::entry() const {
        return Entry(Entry::Type::EVENT,
                     this->summary,
                     this->rrule->now(),
                     this->end - this->start);
    }

    void ICalEvent::str(std::ostream& out) const {
        out << "VEVENT: " << this->summary << std::endl;
        out << " - start: " << this->start << std::endl;
        out << " - end: " << this->end << std::endl;
        out << " - rrule: " << this->rrule << std::endl;
    }

    std::ostream & operator << (std::ostream &out, const ICalEvent::ptr &e) {
        e->str(out);
        return out;
    }

    std::ostream & operator << (std::ostream &out, const ICalEvent &e) {
        e.str(out);
        return out;
    }
}
