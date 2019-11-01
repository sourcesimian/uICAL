#include "uICAL/cppstl.h"
#include "uICAL/datetime.h"
#include "uICAL/event.h"
#include "uICAL/icalcomponent.h"

namespace uICAL {

    Event::Event(VComponent& event, Timezones& timezones) {

        VLine::ptr dtStart = event.getPropertyByName("DTSTART");
        VLine::ptr dtEnd = event.getPropertyByName("DTEND");
        VLine::ptr rRule = event.getPropertyByName("RRULE");
        VLine::ptr summary = event.getPropertyByName("SUMMARY");
        
        
        this->start = DateTime(DateStamp(dtStart->value), TZ::init(timezones.getOffset(dtStart->getParam("TZID"))));
        this->end = DateTime(DateStamp(dtEnd->value), TZ::init(timezones.getOffset(dtStart->getParam("TZID"))));
        this->recurrence = RRuleParser::init(rRule->value);

        this->summary = summary->value;
    }

    std::string Event::str() const {
        std::ostringstream out;
        this->str(out);
        return out.str();
    }

    void Event::str(std::ostream& out) const {
        out << "EVENT: " << this->summary << std::endl;
        out << " - start: " << this->start << std::endl;
        out << " - end: " << this->end << std::endl;
        out << " - recurrence: " << this->recurrence << std::endl;
    }

    std::ostream & operator << (std::ostream &out, const Event::ptr &e) {
        e->str(out);
        return out;
    }

    std::ostream & operator << (std::ostream &out, const Event &e) {
        e.str(out);
        return out;
    }
}
